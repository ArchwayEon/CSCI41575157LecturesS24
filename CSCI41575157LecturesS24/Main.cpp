#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Material {
    float ambientIntensity;  // 0 to 1
    float specularIntensity; // 0 to 1
    float shininess;         // 0 to infinity
};


struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float attenuationCoef;
};

struct SphericalCoordinate {
    float phi = 0.0f, theta = 0.0f, rho = 1.0f;

    glm::mat4 ToMat4() {
        float thetaRadians = glm::radians(theta);
        float phiRadians = glm::radians(phi);
        float sinPhi = sin(phiRadians);
        glm::vec3 zAxis{};
        zAxis.x = rho * sin(thetaRadians) * sinPhi;
        zAxis.y = rho * cos(phiRadians);
        zAxis.z = rho * cos(thetaRadians) * sinPhi;

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 xAxis = glm::normalize(glm::cross(up, zAxis));
        glm::vec3 yAxis = glm::cross(zAxis, xAxis);
        glm::mat4 orientation(1.0f);
        orientation[0] = glm::vec4(xAxis, 0.0f);
        orientation[1] = glm::vec4(yAxis, 0.0f);
        orientation[2] = glm::vec4(zAxis, 0.0f);
        return orientation;
    }
};



static void OnWindowSizeChanged(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

struct MouseParams {
    SphericalCoordinate spherical{};
    double x = 0, y = 0;
    double windowX = 0, windowY = 0;
    int windowWidth = 0, windowHeight = 0;
    float fieldOfView = 60.0f;
};

// Eek! A global mouse!
MouseParams mouse;

static void OnMouseMove(GLFWwindow* window, double mouseX, double mouseY)
{
    mouse.x = mouseX;
    mouse.y = mouseY;

    float xPercent = static_cast<float>(mouse.x / mouse.windowWidth);
    float yPercent = static_cast<float>(mouse.y / mouse.windowHeight);

    mouse.spherical.theta = 90.0f - (xPercent * 180); // left/right
    mouse.spherical.phi = 180.0f - (yPercent * 180); // up/down
}

static void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    mouse.fieldOfView -= static_cast<float>(yoffset * 2);
    if (mouse.fieldOfView < 1.0f) mouse.fieldOfView = 1.0f;
    if (mouse.fieldOfView > 60.0f) mouse.fieldOfView = 60.0f;
}

static void ProcessInput(GLFWwindow* window, double elapsedSeconds, glm::vec3& axis, glm::mat4& cameraFrame)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        axis = glm::vec3(0.0f, 1.0f, 0.0f);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        axis = glm::vec3(1.0f, 0.0f, 0.0f);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        axis = glm::vec3(0.0f, 0.0f, 1.0f);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 forward = -cameraFrame[2];
        glm::vec3 position = cameraFrame[3];
        forward = forward * static_cast<float>(10.0f * elapsedSeconds);
        position = position + forward;
        cameraFrame[3] = glm::vec4(position, 1.0f);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 toLeft = -cameraFrame[0];
        glm::vec3 position = cameraFrame[3];
        toLeft = toLeft * static_cast<float>(10.0f * elapsedSeconds);        
        position = position + toLeft;
        cameraFrame[3] = glm::vec4(position, 1.0f);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 backward = cameraFrame[2];
        glm::vec3 position = cameraFrame[3];
        backward = backward * static_cast<float>(10.0f * elapsedSeconds);
        position = position + backward;
        cameraFrame[3] = glm::vec4(position, 1.0f);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 toRight = cameraFrame[0];
        glm::vec3 position = cameraFrame[3];
        toRight = toRight * static_cast<float>(10.0f * elapsedSeconds);
        position = position + toRight;
        cameraFrame[3] = glm::vec4(position, 1.0f);
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        glm::vec3 yAxis = cameraFrame[1];
        float turnDelta = static_cast<float>(-90.0f * elapsedSeconds);
        cameraFrame = glm::rotate(cameraFrame, glm::radians(turnDelta), yAxis);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        glm::vec3 yAxis = cameraFrame[1];
        float turnDelta = static_cast<float>(90.0f * elapsedSeconds);
        cameraFrame = glm::rotate(cameraFrame, glm::radians(turnDelta), yAxis);
        return;
    }
}

struct Result {
    bool isSuccess;
    std::string message;

    Result() : isSuccess(true) {}
};

struct VertexDataPC {
    glm::vec3 position;
    glm::vec3 color;
};

struct VertexDataPCT {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 tex;
};

struct VertexDataPCNT {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 normal;
    glm::vec2 tex;
};

static void Log(std::stringstream& log, const std::vector<char>& message)
{
    std::copy(message.begin(), message.end(), std::ostream_iterator<char>(log, ""));
}

static unsigned int CompileShaderSource(
    const std::string& shaderSource, int type, std::stringstream& logger, Result& result)
{
    unsigned shaderId = glCreateShader(type);

    // Send the vertex shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    const char* source = (const char*)shaderSource.c_str();
    glShaderSource(shaderId, 1, &source, 0);

    glCompileShader(shaderId);

    int isCompiled = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        int maxLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<char> infoLog(maxLength);
        glGetShaderInfoLog(shaderId, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(shaderId);

        Log(logger, infoLog);
        result.isSuccess = false;
        result.message = logger.str();
        return -1;
    }
    result.isSuccess = true;
    result.message = "Success!";
    return shaderId;
}

static Result CreateShaderProgram(
    std::string& vertexSource, std::string& fragmentSource, unsigned int& program)
{
    std::stringstream logger;
    Result result;

    unsigned int vertexShader =
        CompileShaderSource(vertexSource, GL_VERTEX_SHADER, logger, result);
    if (result.isSuccess == false) return result;

    unsigned int fragmentShader =
        CompileShaderSource(fragmentSource, GL_FRAGMENT_SHADER, logger, result);
    if (result.isSuccess == false) return result;

    // Time to link the shaders together into a program.

    program = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    int isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        Log(logger, infoLog);
        result.isSuccess = false;
        result.message = logger.str();
        return result;
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    result.isSuccess = true;
    result.message = "Successfully created the shader!";
    return result;
}

static glm::mat4 CreateViewMatrix(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up)
{
    glm::vec3 right = glm::cross(direction, up);
    right = glm::normalize(right);

    glm::vec3 vUp = glm::cross(right, direction);
    vUp = glm::normalize(vUp);

    glm::mat4 view(1.0f);
    view[0] = glm::vec4(right, 0.0f);
    view[1] = glm::vec4(up, 0.0f);
    view[2] = glm::vec4(direction, 0.0f);
    view[3] = glm::vec4(position, 1.0f);
    return glm::inverse(view);
}

static void EnableAttribute(int attribIndex, int elementCount, int sizeInBytes, void* offset)
{
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribPointer(
        attribIndex,
        elementCount, 
        GL_FLOAT,     
        GL_FALSE,
        sizeInBytes, // The number of bytes to the next element
        offset       // Byte offset of the first position in the array
    );
}

static void EnablePCNTAttributes()
{
    // Positions
    EnableAttribute(0, 3, sizeof(VertexDataPCNT), (void*)0);
    // Colors
    EnableAttribute(1, 4, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 3));
    // Normals
    EnableAttribute(2, 3, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 7));
    // Texture Coords
    EnableAttribute(3, 2, sizeof(VertexDataPCNT), (void*)(sizeof(float) * 10));
}

static void EnablePCTAttributes()
{
    // Positions
    EnableAttribute(0, 3, sizeof(VertexDataPCT), (void*)0);
    // Colors
    EnableAttribute(1, 3, sizeof(VertexDataPCT), (void*)(sizeof(float) * 3));
    // Texture Coords
    EnableAttribute(2, 2, sizeof(VertexDataPCT), (void*)(sizeof(float) * 6));
}

static void EnablePCAttributes()
{
    // Positions
    EnableAttribute(0, 3, sizeof(VertexDataPC), (void*)0);
    // Colors
    EnableAttribute(1, 3, sizeof(VertexDataPC), (void*)(sizeof(float) * 3));
}

void Trim(std::string& str)
{
    const std::string delimiters = " \f\n\r\t\v";
    str.erase(str.find_last_not_of(delimiters) + 1);
    str.erase(0, str.find_first_not_of(delimiters));
}

std::string ReadFromFile(const std::string& filePath)
{
    std::stringstream ss;
    std::ifstream fin{};
    fin.open(filePath.c_str());
    if (fin.fail()) {
        ss << "Could not open: " << filePath << std::endl;
        return ss.str();
    }

    std::string line;
    while (!fin.eof()) {
        getline(fin, line);
        Trim(line);
        if (line != "") { // Skip blank lines
            ss << line << std::endl;
        }
    }
    fin.close();
    return ss.str();
}

unsigned char* LoadTextureDataFromFile(
    const std::string& filePath, int& width, int& height, int& numChannels)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = 
        stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
    return data;
}

unsigned int Create2DTexture(
    unsigned char* textureData, unsigned int width, unsigned int height)
{
    // Generate the texture id
    unsigned int textureId;
    glGenTextures(1, &textureId);
    // Select the texture 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // Apply texture parameters 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Send the texture to the GPU 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    return textureId;
}

unsigned int CreateTextureFromFile(const std::string& filePath)
{
    int textureWidth, textureHeight, numChannels;
    unsigned char* textureData =
        LoadTextureDataFromFile(
            filePath, textureWidth, textureHeight, numChannels);
    unsigned int textureId =
        Create2DTexture(textureData, textureWidth, textureHeight);
    stbi_image_free(textureData);
    textureData = nullptr;
    return textureId;
}

VertexDataPCNT* CreateCubeVertexData()
{
    // Front face
    VertexDataPCNT A = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} };
    VertexDataPCNT B = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} };
    VertexDataPCNT C = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} };
    VertexDataPCNT D = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} };
    // Right face
    VertexDataPCNT E = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} };
    VertexDataPCNT F = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} };
    VertexDataPCNT G = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} };
    VertexDataPCNT H = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} };
    // Back face
    VertexDataPCNT I = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} };
    VertexDataPCNT J = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} };
    VertexDataPCNT K = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} };
    VertexDataPCNT L = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} };
    // Left face
    VertexDataPCNT M = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} };
    VertexDataPCNT N = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} };
    VertexDataPCNT O = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} };
    VertexDataPCNT P = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} };
    // Top face
    VertexDataPCNT Q = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} };
    VertexDataPCNT R = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} };
    VertexDataPCNT S = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} };
    VertexDataPCNT T = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} };
    // Bottom face
    VertexDataPCNT U = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} };
    VertexDataPCNT V = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} };
    VertexDataPCNT W = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} };
    VertexDataPCNT X = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} };

    // 3 vertex per triangle, 2 triangles per face, 6 faces
    // 3 * 2 * 6 = 36 vertices
    VertexDataPCNT* vertexData{ new VertexDataPCNT[36]{
        // Front face
        A, B, C, A, C, D,
        // Right face
        E, F, G, E, G, H,
        // Back face
        I, J, K, I, K, L,
        // Left face
        M, N, O, M, O, P,
        // Top face
        Q, R, S, Q, S, T,
        // Bottom face
        U, V, W, U, W, X
    } };
    return vertexData;
}

VertexDataPCT* CreateXYPlanePCT(
    float width = 1.0f, float height = 1.0f, 
    glm::vec3 color = {1.0f, 1.0f, 1.0f},
    glm::vec2 tex = {1.0f, 1.0f})
{
    float hw = width / 2;
    float hh = height / 2;
    // Front face
    VertexDataPCT A = { {-hw, hh, 0.0f}, color, {0.0f, tex.t} };
    VertexDataPCT B = { {-hw,-hh, 0.0f}, color, {0.0f, 0.0f} };
    VertexDataPCT C = { { hw,-hh, 0.0f}, color, {tex.s, 0.0f} };
    VertexDataPCT D = { { hw, hh, 0.0f}, color, {tex.s, tex.t} };

    VertexDataPCT* vertexData{ new VertexDataPCT[6]{
        A, B, C, A, C, D
    } };
    return vertexData;
}

VertexDataPCNT* CreateXZPlanePCNT(
    float width, float depth, 
    glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f },
    float repeatS = 1.0f, float repeatT = 1.0f)
{
    float x = width / 2;
    float z = depth / 2;
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    // Front face
    VertexDataPCNT A = { {-x, 0.0f,-z}, color, normal, {0.0f, repeatT} };
    VertexDataPCNT B = { {-x, 0.0f, z}, color, normal, {0.0f, 0.0f} };
    VertexDataPCNT C = { { x, 0.0f, z}, color, normal, {repeatS, 0.0f} };
    VertexDataPCNT D = { { x, 0.0f,-z}, color, normal, {repeatS, repeatT} };

    VertexDataPCNT* vertexData{ new VertexDataPCNT[6]{
        A, B, C, A, C, D
    } };
    return vertexData;
}

struct PCData {
    VertexDataPC* vertexData;
    int vertexCount;
};

PCData CreateXYCirclePC(float radius, glm::vec3 color, int steps = 10)
{
    PCData pcData{};
    pcData.vertexCount = (static_cast<int>(360.0 / steps) + 1) * 2;
    pcData.vertexData = new VertexDataPC[pcData.vertexCount];
    int vi = 0;
    float x, y, thetaRadians;
    for (float theta = 0; theta <= 360; theta += steps) {
        thetaRadians = glm::radians(theta);
        x = radius * cosf(thetaRadians);
        y = radius * sinf(thetaRadians);
        pcData.vertexData[vi++] = { {x, y, 0.0f}, color};
        thetaRadians = glm::radians(theta + steps);
        x = radius * cosf(thetaRadians);
        y = radius * sinf(thetaRadians);
        pcData.vertexData[vi++] = { {x, y, 0.0f}, color };
    }
    return pcData;
}

void PointAt(glm::mat4& referenceFrame, const glm::vec3& point)
{
    glm::vec3 position = referenceFrame[3];
    glm::vec3 zAxis = glm::normalize(point - position);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 xAxis = glm::normalize(glm::cross(up, zAxis));
    glm::vec3 yAxis = glm::cross(zAxis, xAxis);
    referenceFrame[0] = glm::vec4(xAxis, 0.0f);
    referenceFrame[1] = glm::vec4(yAxis, 0.0f);
    referenceFrame[2] = glm::vec4(zAxis, 0.0f);
}

std::string GetOpenGLError()
{
    std::stringstream log;
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        log << err << std::endl;
    }
    return log.str();
}

struct GraphicsObject {
    VertexDataPCNT* vertexDataPCNT = nullptr;
    VertexDataPCT* vertexDataPCT = nullptr;
    VertexDataPC* vertexDataPC = nullptr;
    std::size_t sizeOfBuffer = 0;
    unsigned int numberOfVertices = 0;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int shaderProgram = 0;
    unsigned int textureId = 0;
    glm::mat4 referenceFrame = glm::mat4(1.0f);
    Material material{};
};

static unsigned int AllocateVertexBufferPCNT(GraphicsObject& object)
{
    glBindVertexArray(object.vao);
    glGenBuffers(1, &object.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    glBufferData(
        GL_ARRAY_BUFFER, object.sizeOfBuffer, object.vertexDataPCNT, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] object.vertexDataPCNT;
    object.vertexDataPCNT = nullptr;
    glBindVertexArray(0);
    return object.vbo;
}

static unsigned int AllocateVertexBufferPCT(GraphicsObject& object)
{
    glBindVertexArray(object.vao);
    glGenBuffers(1, &object.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    glBufferData(
        GL_ARRAY_BUFFER, object.sizeOfBuffer, object.vertexDataPCT, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] object.vertexDataPCT;
    object.vertexDataPCT = nullptr;
    glBindVertexArray(0);
    return object.vbo;
}

static unsigned int AllocateVertexBufferPC(GraphicsObject& object)
{
    glBindVertexArray(object.vao);
    glGenBuffers(1, &object.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    glBufferData(
        GL_ARRAY_BUFFER, object.sizeOfBuffer, object.vertexDataPC, 
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] object.vertexDataPC;
    object.vertexDataPC = nullptr;
    glBindVertexArray(0);
    return object.vbo;
}

struct LightingShaderLocation {
    unsigned int worldLoc = 0;
    unsigned int projectionLoc = 0;
    unsigned int viewLoc = 0;
    unsigned int materialAmbientLoc = 0;
    unsigned int materialSpecularLoc = 0;
    unsigned int materialShininessLoc = 0;
    unsigned int globalLightPositionLoc = 0;
    unsigned int globalLightColorLoc = 0;
    unsigned int globalLightIntensityLoc = 0;
    unsigned int localLightPositionLoc = 0;
    unsigned int localLightColorLoc = 0;
    unsigned int localLightIntensityLoc = 0;
    unsigned int localLightAttenuationLoc = 0;
    unsigned int viewPositionLoc = 0;
};

struct BasicShaderLocation {
    unsigned int worldLoc = 0;
    unsigned int projectionLoc = 0;
    unsigned int viewLoc = 0;
};

static void RenderObjectPCNT(
    GraphicsObject& object, LightingShaderLocation& location,
    glm::mat4& projection, glm::mat4& view, 
    Light& globalLight, Light& localLight,
    glm::vec3& cameraPosition, unsigned int primitive)
{
    glUseProgram(object.shaderProgram);
    glUniformMatrix4fv(
        location.projectionLoc, 1, GL_FALSE,
        glm::value_ptr(projection));
    glUniformMatrix4fv(location.viewLoc, 1, GL_FALSE,
        glm::value_ptr(view));
    glUniform3fv(
        location.globalLightPositionLoc, 1,
        glm::value_ptr(globalLight.position));
    glUniform3fv(
        location.globalLightColorLoc, 1,
        glm::value_ptr(globalLight.color));
    glUniform1f(
        location.globalLightIntensityLoc,
        globalLight.intensity);
    glUniform3fv(
        location.localLightPositionLoc, 1,
        glm::value_ptr(localLight.position));
    glUniform3fv(
        location.localLightColorLoc, 1,
        glm::value_ptr(localLight.color));
    glUniform1f(
        location.localLightIntensityLoc,
        localLight.intensity);
    glUniform1f(
        location.localLightAttenuationLoc,
        localLight.attenuationCoef);
    glUniform3fv(
        location.viewPositionLoc, 1,
        glm::value_ptr(cameraPosition));

    glBindVertexArray(object.vao);
    glUniformMatrix4fv(
        location.worldLoc, 1, GL_FALSE,
        glm::value_ptr(object.referenceFrame));
    glUniform1f(
        location.materialAmbientLoc,
        object.material.ambientIntensity);
    glUniform1f(
        location.materialSpecularLoc,
        object.material.specularIntensity);
    glUniform1f(
        location.materialShininessLoc,
        object.material.shininess);
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    EnablePCNTAttributes();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object.textureId);
    glDrawArrays(primitive, 0, object.numberOfVertices);
}

static void RenderObjectPCT(
    GraphicsObject& object, BasicShaderLocation& location,
    glm::mat4& projection, glm::mat4& view, unsigned int primitive)
{
    glUseProgram(object.shaderProgram);
    glUniformMatrix4fv(
        location.projectionLoc, 1, GL_FALSE,
        glm::value_ptr(projection));
    glUniformMatrix4fv(location.viewLoc, 1, GL_FALSE,
        glm::value_ptr(view));

    glBindVertexArray(object.vao);
    glUniformMatrix4fv(
        location.worldLoc, 1, GL_FALSE,
        glm::value_ptr(object.referenceFrame));
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    EnablePCTAttributes();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object.textureId);
    glDrawArrays(primitive, 0, object.numberOfVertices);
}

static void RenderObjectPC(
    GraphicsObject& object, BasicShaderLocation& location,
    glm::mat4& projection, glm::mat4& view, unsigned int primitive)
{
    glUseProgram(object.shaderProgram);
    glUniformMatrix4fv(
        location.projectionLoc, 1, GL_FALSE,
        glm::value_ptr(projection));
    glUniformMatrix4fv(location.viewLoc, 1, GL_FALSE,
        glm::value_ptr(view));

    glBindVertexArray(object.vao);
    glUniformMatrix4fv(
        location.worldLoc, 1, GL_FALSE,
        glm::value_ptr(object.referenceFrame));
    glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
    EnablePCAttributes();
    glDrawArrays(primitive, 0, object.numberOfVertices);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 800, "ETSU Computing Interactive Graphics", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1200, 800);
    glfwSetFramebufferSizeCallback(window, OnWindowSizeChanged);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetScrollCallback(window, OnScroll);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMaximizeWindow(window);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cull back faces and use counter-clockwise winding of front faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    std::string vertexSource1 = ReadFromFile("lighting.vert.glsl");
    std::string fragmentSource1 = ReadFromFile("lighting.frag.glsl");
    std::string vertexSource2 = ReadFromFile("basic.vert.glsl");
    std::string fragmentSource2 = ReadFromFile("basic.frag.glsl");
    std::string pcVertexSource = ReadFromFile("pc.vert.glsl");
    std::string pcFragmentSource = ReadFromFile("pc.frag.glsl");

    unsigned int lightingShaderProgram;
    Result result1 = CreateShaderProgram(vertexSource1, fragmentSource1, lightingShaderProgram);
    unsigned int textureShaderProgram;
    Result result2 = CreateShaderProgram(vertexSource2, fragmentSource2, textureShaderProgram);
    unsigned int pcShaderProgram;
    Result resultPC = 
        CreateShaderProgram(pcVertexSource, pcFragmentSource, pcShaderProgram);
    std::string message = result1.message + result2.message + resultPC.message;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // Get the uniform locations
    LightingShaderLocation lightingLocation;
    lightingLocation.projectionLoc = 
        glGetUniformLocation(lightingShaderProgram, "projection");
    lightingLocation.viewLoc = 
        glGetUniformLocation(lightingShaderProgram, "view");
    lightingLocation.worldLoc = 
        glGetUniformLocation(lightingShaderProgram, "world");
    BasicShaderLocation textureLocation;
    textureLocation.projectionLoc = 
        glGetUniformLocation(textureShaderProgram, "projection");
    textureLocation.viewLoc =
        glGetUniformLocation(textureShaderProgram, "view");
    textureLocation.worldLoc = 
        glGetUniformLocation(textureShaderProgram, "world");
    BasicShaderLocation pcLocation;
    pcLocation.projectionLoc =
        glGetUniformLocation(pcShaderProgram, "projection");
    pcLocation.viewLoc =
        glGetUniformLocation(pcShaderProgram, "view");
    pcLocation.worldLoc =
        glGetUniformLocation(pcShaderProgram, "world");

    // Create the texture data
    unsigned char* textureData = new unsigned char[] {
          0, 0, 0, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 0, 255,
          0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 0, 255,
          0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 0, 255,
          0, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 0, 0, 0, 255
    };

    unsigned int customTextureId = Create2DTexture(textureData, 4, 4);
    delete[] textureData;
    textureData = nullptr;

    unsigned int lightingVAO;
    glGenVertexArrays(1, &lightingVAO);

    GraphicsObject cube;
    cube.vertexDataPCNT = CreateCubeVertexData();
    cube.sizeOfBuffer = 36 * sizeof(VertexDataPCNT);
    cube.numberOfVertices = 36;
    cube.vao = lightingVAO;
    cube.shaderProgram = lightingShaderProgram;
    cube.vbo = AllocateVertexBufferPCNT(cube);
    cube.textureId = customTextureId;
    cube.material.ambientIntensity = 0.1f;
    cube.material.specularIntensity = 0.5f;
    cube.material.shininess = 16.0f;
    cube.referenceFrame[3] = glm::vec4(0.0f, 0.0f, -25.0f, 1.0f);

    GraphicsObject floor;
    floor.vertexDataPCNT = 
        CreateXZPlanePCNT(50.0f, 50.0f, { 1.0f, 1.0f, 1.0f, 1.0f }, 5.0f, 5.0f);
    floor.sizeOfBuffer = 6 * sizeof(VertexDataPCNT);
    floor.numberOfVertices = 6;
    floor.vao = lightingVAO;
    floor.shaderProgram = lightingShaderProgram;
    floor.vbo = AllocateVertexBufferPCNT(floor);
    floor.textureId = CreateTextureFromFile("stone-road-texture.jpg");
    floor.referenceFrame[3] = glm::vec4(0.0f, -5.0f, 0.0f, 1.0f);
    floor.material.ambientIntensity = 0.1f;
    floor.material.specularIntensity = 0.5f;
    floor.material.shininess = 16.0f;

    unsigned int basicTextureVAO;
    glGenVertexArrays(1, &basicTextureVAO);

    GraphicsObject lightBulb;
    lightBulb.vertexDataPCT = CreateXYPlanePCT();
    lightBulb.sizeOfBuffer = 6 * sizeof(VertexDataPCT);
    lightBulb.numberOfVertices = 6;
    lightBulb.vao = basicTextureVAO;
    lightBulb.shaderProgram = textureShaderProgram;
    lightBulb.vbo = AllocateVertexBufferPCT(lightBulb);
    lightBulb.textureId = CreateTextureFromFile("lightbulb.png");

    unsigned int pcTextureVAO;
    glGenVertexArrays(1, &pcTextureVAO);

    GraphicsObject circle;
    PCData pcData = CreateXYCirclePC(5.0f, { 1.0f, 1.0f, 1.0f });
    circle.vertexDataPC = pcData.vertexData;
    circle.sizeOfBuffer = pcData.vertexCount * sizeof(VertexDataPC);
    circle.numberOfVertices = pcData.vertexCount;
    circle.vao = pcTextureVAO;
    circle.shaderProgram = pcShaderProgram;
    circle.vbo = AllocateVertexBufferPC(circle);

    float cubeYAngle = 0;
    float cubeXAngle = 0;
    float cubeZAngle = 0;
    float left = -20.0f;
    float right = 20.0f;
    float bottom = -20.0f;
    float top = 20.0f;
    int width, height;
    
    

    float aspectRatio;
    float nearPlane = 1.0f;
    float farPlane = 100.0f;
    float fieldOfView = 60;

    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f };

    lightingLocation.materialAmbientLoc = 
        glGetUniformLocation(lightingShaderProgram, "materialAmbientIntensity");
    lightingLocation.materialSpecularLoc =
        glGetUniformLocation(lightingShaderProgram, "materialSpecularIntensity");
    lightingLocation.materialShininessLoc =
        glGetUniformLocation(lightingShaderProgram, "materialShininess");

    // Lights
    Light globalLight{};
    globalLight.position = glm::vec3(100.0f, 100.0f, 0.0f); 
    globalLight.color = glm::vec3(1.0f, 1.0f, 1.0f); // White light
    globalLight.intensity = 0.05f;
    lightingLocation.globalLightPositionLoc =
        glGetUniformLocation(lightingShaderProgram, "globalLightPosition");
    lightingLocation.globalLightColorLoc =
        glGetUniformLocation(lightingShaderProgram, "globalLightColor");
    lightingLocation.globalLightIntensityLoc =
        glGetUniformLocation(lightingShaderProgram, "globalLightIntensity");
    Light localLight{};
    localLight.position = glm::vec3(0.0f, 0.0f, -17.0f);
    localLight.color = glm::vec3(1.0f, 1.0f, 1.0f); // White light
    localLight.intensity = 0.5f;
    localLight.attenuationCoef = 0.0f;
    lightingLocation.localLightPositionLoc =
        glGetUniformLocation(lightingShaderProgram, "localLightPosition");
    lightingLocation.localLightColorLoc =
        glGetUniformLocation(lightingShaderProgram, "localLightColor");
    lightingLocation.localLightIntensityLoc =
        glGetUniformLocation(lightingShaderProgram, "localLightIntensity");
    lightingLocation.localLightAttenuationLoc =
        glGetUniformLocation(lightingShaderProgram, "localLightAttenuationCoef");
    lightingLocation.viewPositionLoc =
        glGetUniformLocation(lightingShaderProgram, "viewPosition");

    glm::mat4 lookFrame(1.0f);
    glm::mat4 cameraFrame(1.0f);
    cameraFrame[3] = glm::vec4(0.0f, 3.0f, 30.0f, 1.0f);
    glm::vec3 cameraForward;
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    float speed = 90.0f;
    double elapsedSeconds;
    float deltaAngle;
    bool lookWithMouse = false;
    bool resetCameraPosition = false;
    bool correctGamma = false;
    Timer timer;
    while (!glfwWindowShouldClose(window)) {
        elapsedSeconds = timer.GetElapsedTimeInSeconds();
        ProcessInput(window, elapsedSeconds, axis, cameraFrame);
        glfwGetWindowSize(window, &width, &height);
        mouse.windowWidth = width;
        mouse.windowHeight = height;

        if (correctGamma) {
            glEnable(GL_FRAMEBUFFER_SRGB);
        }
        else {
            glDisable(GL_FRAMEBUFFER_SRGB);
        }

        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        deltaAngle = static_cast<float>(speed * elapsedSeconds);
        cube.referenceFrame = glm::rotate(cube.referenceFrame, glm::radians(deltaAngle), axis);

        if (resetCameraPosition) {
            cameraFrame = glm::mat4(1.0f);
            cameraFrame[3] = glm::vec4(0.0f, 3.0f, 30.0f, 1.0f);
            resetCameraPosition = false;
            lookWithMouse = false;
        }
        if (lookWithMouse) {
            lookFrame = mouse.spherical.ToMat4();
            cameraFrame[0] = lookFrame[0];
            cameraFrame[1] = lookFrame[1];
            cameraFrame[2] = lookFrame[2];
        }

        cameraPosition = cameraFrame[3];
        cameraForward = -cameraFrame[2];
        cameraTarget = cameraPosition + cameraForward;
        cameraUp = cameraFrame[1];
        view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

        lightBulb.referenceFrame[3] = glm::vec4(localLight.position, 1.0f);
        PointAt(lightBulb.referenceFrame, cameraPosition);
        
        if (width >= height) {
            aspectRatio = width / (height * 1.0f);
        }
        else {
            aspectRatio = height / (width * 1.0f);
        }
        projection = glm::perspective(
            glm::radians(mouse.fieldOfView), aspectRatio, nearPlane, farPlane);

        // Render the object
        if (result1.isSuccess){
            RenderObjectPCNT(
                cube, lightingLocation, projection, view, 
                globalLight, localLight, cameraPosition, GL_TRIANGLES);
            RenderObjectPCNT(
                floor, lightingLocation, projection, view,
                globalLight, localLight, cameraPosition, GL_TRIANGLES);
        }

        if (result2.isSuccess)
        {
            RenderObjectPCT(
                lightBulb, textureLocation, projection, view, GL_TRIANGLES);
        }

        if (resultPC.isSuccess)
        {
            RenderObjectPC(
                circle, pcLocation, projection, view, GL_LINES);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Computing Interactive Graphics");
        ImGui::Text(message.c_str());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Elapsed seconds: %.3f", elapsedSeconds);
        ImGui::Text("Mouse: (%.0f, %.0f)", mouse.x, mouse.y);
        ImGui::Text("Field of View: %.0f", mouse.fieldOfView);
        ImGui::Text("Theta:%.1f, Phi:%.1f)", 
            mouse.spherical.theta, mouse.spherical.phi);
        ImGui::ColorEdit3("Background color", (float*)&clearColor.r);
        ImGui::SliderFloat("Speed", &speed, 0, 360);
        ImGui::Checkbox("Use mouse to look", &lookWithMouse);
        ImGui::Checkbox("Reset camera position", &resetCameraPosition);
        ImGui::SliderFloat("Global Intensity", &globalLight.intensity, 0, 1);
        ImGui::SliderFloat("Local Intensity", &localLight.intensity, 0, 1);
        ImGui::Checkbox("Correct gamma", &correctGamma);
        ImGui::SliderFloat("Attenuation", &localLight.attenuationCoef, 0, 1);
        //ImGui::SliderFloat("Ambient Intensity", &material.ambientIntensity, 0, 1);
        //ImGui::SliderFloat("Specular", &material.specularIntensity, 0, 1);
        //ImGui::SliderFloat("Shininess", &material.shininess, 0, 100);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}


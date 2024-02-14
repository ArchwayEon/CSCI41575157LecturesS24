#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void OnWindowSizeChanged(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

struct Result {
    bool isSuccess;
    std::string message;

    Result() : isSuccess(true) {}
};

struct VertexData {
    glm::vec3 position, color;
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
    //glfwMaximizeWindow(window);

    // Cull back faces and use counter-clockwise winding of front faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    std::string vertexSource =
        "#version 430\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec3 color;\n"
        "layout(location = 2) in vec2 texCoord;\n"
        "out vec4 fragColor;\n"
        "out vec2 fragTexCoord;\n"
        "uniform mat4 world;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * world * vec4(position, 1.0);\n"
        "   fragColor = vec4(color, 1.0);\n"
        "   fragTexCoord = texCoord;\n"
        "}\n";

    std::string fragmentSource =
        "#version 430\n"
        "in vec4 fragColor;\n"
        "in vec2 fragTexCoord;\n"
        "out vec4 color;\n"
        "uniform sampler2D tex;\n"
        "void main()\n"
        "{\n"
        "   vec4 texFragColor = texture(tex, fragTexCoord) * fragColor;\n"
        "   color = texFragColor;\n"
        "}\n";

    unsigned int shaderProgram;
    Result result = CreateShaderProgram(vertexSource, fragmentSource, shaderProgram);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    // Get the uniform locations
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int worldLoc = glGetUniformLocation(shaderProgram, "world");

    // Create the texture data
    unsigned char* textureData = new unsigned char[] {
          0, 0, 0, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 0, 255,
          0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 0, 255,
          0, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 0, 255,
          0, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 0, 0, 0, 255
    };

    // Generate the texture id
    GLuint textureId;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Front face
    VertexData A = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};
    VertexData B = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};
    VertexData C = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}};
    VertexData D = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};
    // Right face
    VertexData E = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}};
    VertexData F = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}};
    VertexData G = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}};
    VertexData H = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}};
    // Back face
    VertexData I = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} };
    VertexData J = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} };
    VertexData K = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} };
    VertexData L = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} };
    // Left face
    VertexData M = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} };
    VertexData N = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} };
    VertexData O = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} };
    VertexData P = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} };
    // Top face
    VertexData Q = { {-5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} };
    VertexData R = { {-5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} };
    VertexData S = { { 5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} };
    VertexData T = { { 5.0f, 5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} };
    // Bottom face
    VertexData U = { { 5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} };
    VertexData V = { { 5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} };
    VertexData W = { {-5.0f,-5.0f, 5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} };
    VertexData X = { {-5.0f,-5.0f,-5.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} };

    // 3 vertex per triangle, 2 triangles per face, 6 faces
    // 3 * 2 * 6 = 36 vertices
    VertexData vertexData[36]{
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
    };

    unsigned int vaoId, vboId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float cubeYAngle = 0;
    float cubeXAngle = 0;
    float cubeZAngle = 0;
    float left = -20.0f;
    float right = 20.0f;
    float bottom = -20.0f;
    float top = 20.0f;
    int width, height;
    
    std::string message = result.message;

    float aspectRatio;
    float nearPlane = 1.0f;
    float farPlane = 50.0f;
    float fieldOfView = 60;

    glm::vec3 cameraPosition(15.0f, 15.0f, 20.0f);
    glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 referenceFrame(1.0f);
    glm::vec3 clearColor = { 0.2f, 0.3f, 0.3f };

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);
        glfwGetWindowSize(window, &width, &height);

        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        referenceFrame = glm::rotate(glm::mat4(1.0f), glm::radians(cubeYAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        referenceFrame = glm::rotate(referenceFrame, glm::radians(cubeXAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        referenceFrame = glm::rotate(referenceFrame, glm::radians(cubeZAngle), glm::vec3(0.0f, 0.0f, 1.0f));
       
        view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
        
        if (width >= height) {
            aspectRatio = width / (height * 1.0f);
        }
        else {
            aspectRatio = height / (width * 1.0f);
        }
        projection = glm::perspective(
            glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);

        // Render the object
        if (result.isSuccess)
        {
            glUseProgram(shaderProgram);
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(referenceFrame));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glBindVertexArray(vaoId);
            glBindBuffer(GL_ARRAY_BUFFER, vboId);
            // Positions
            EnableAttribute(0, 3, sizeof(VertexData), (void*)0);
            // Colors
            EnableAttribute(1, 3, sizeof(VertexData), (void*)sizeof(glm::vec3));
            // Texture Coords
            EnableAttribute(2, 2, sizeof(VertexData), (void*)(sizeof(glm::vec3)*2));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glUseProgram(0);
            glBindVertexArray(0);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Computing Interactive Graphics");
        ImGui::Text(message.c_str());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / io.Framerate, io.Framerate);
        ImGui::ColorEdit3("Background color", (float*)&clearColor.r);
        ImGui::SliderFloat("X Angle", &cubeXAngle, 0, 360);
        ImGui::SliderFloat("Y Angle", &cubeYAngle, 0, 360);
        ImGui::SliderFloat("Z Angle", &cubeZAngle, 0, 360);
        ImGui::SliderFloat("Camera X", &cameraPosition.x, left, right);
        ImGui::SliderFloat("Camera Y", &cameraPosition.y, bottom, top);
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

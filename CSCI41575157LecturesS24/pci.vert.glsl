#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 worldPosition;
layout(location = 3) in vec3 instanceColor;
out vec4 fragColor;
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
void main()
{
   mat4 toWorld = world;
   toWorld[3] = vec4(worldPosition, 1.0f);
   gl_Position = projection * view * toWorld * vec4(position, 1.0);
   fragColor = vec4(instanceColor, 1.0);
}
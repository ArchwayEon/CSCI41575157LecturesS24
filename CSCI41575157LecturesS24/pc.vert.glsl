#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec4 fragColor;
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
void main()
{
   gl_Position = projection * view * world * vec4(position, 1.0);
   fragColor = vec4(color, 1.0);
}
#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
out vec4 fragColor;
out vec2 fragTexCoord;
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
void main()
{
   gl_Position = projection * view * world * vec4(position, 1.0);
   fragColor = vec4(color, 1.0);
   fragTexCoord = texCoord;
}
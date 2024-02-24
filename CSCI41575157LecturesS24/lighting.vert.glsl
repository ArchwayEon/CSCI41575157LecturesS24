#version 430
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 texCoord;

out vec4 fragColor;
out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragTexCoord;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
void main()
{
   vec4 worldPosition = world * vec4(position, 1.0);
   gl_Position = projection * view * worldPosition;

   vec3 worldNormal = mat3(world) * vertexNormal;

   fragPosition = vec3(worldPosition);
   fragNormal = normalize(worldNormal);
   fragColor = vertexColor;
   fragTexCoord = texCoord;
}

#version 430
in vec4 fragColor;
in vec2 fragTexCoord;

out vec4 color;
uniform float materialAmbientIntensity;
uniform sampler2D tex;

void main()
{
   vec4 texFragColor = texture(tex, fragTexCoord) * fragColor;
   vec4 ambientColor = materialAmbientIntensity * vec4(1.0f, 1.0f, 1.0f, 1.0f);
   color = ambientColor * texFragColor;
}

#version 430
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

out vec4 color;

uniform float materialAmbientIntensity;

uniform vec3 globalLightPosition;
uniform vec3 globalLightColor;
uniform float globalLightIntensity;

uniform sampler2D tex;

void main()
{
   vec3 toGlobalLightDir = normalize(globalLightPosition - fragPosition);
   float cosAngIncidence = dot(fragNormal, toGlobalLightDir);
   cosAngIncidence = clamp(cosAngIncidence, 0.0f, 1.0f);
   vec4 globalDiffuse = cosAngIncidence * globalLightIntensity * vec4(globalLightColor, 1.0f);

   vec4 texFragColor = texture(tex, fragTexCoord) * fragColor;
   vec4 ambientColor = materialAmbientIntensity * vec4(1.0f, 1.0f, 1.0f, 1.0f);
   color = (ambientColor + globalDiffuse) * texFragColor;
}

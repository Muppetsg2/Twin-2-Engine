#version 430

in vec3 position;
in vec2 texCoords;
in vec3 normal;

flat in uint materialIndex;

out vec4 FragColor;

uniform vec4 uColor;
uniform bool uNoTexture = true;
uniform sampler2D texture_diffuse1;

struct MaterialInput
{
   vec4 color1;
   vec4 color2;
};

layout (std140, binding = 1) uniform WindowData
{
    vec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[8];
};

void main()
{
	//FragColor = uColor;
	//FragColor = color1 + color2;
	//FragColor = materialInput[materialIndex].color1 + materialInput[materialIndex].color2;
	FragColor = vec4(vec3(vec3(pow(materialInput[materialIndex].color1.rgb * materialInput[materialIndex].color2.rgb, vec3(gamma)))), 1.0);
}
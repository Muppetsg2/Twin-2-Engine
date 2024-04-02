#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

//in vec4 color1;
//in vec4 color2;

layout (location = 3) flat in uint materialIndex;

layout (location = 0) out vec4 FragColor;

//uniform vec4 uColor;
//uniform bool uNoTexture = true;
//uniform sampler2D texture_diffuse1;

struct MaterialInput
{
   vec3 color1;
   vec3 color2;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[3];
};

void main()
{
	//FragColor = uColor;
	//FragColor = color1 + color2;
	//FragColor = materialInput[materialIndex].color1 + materialInput[materialIndex].color2;
	FragColor = vec4(materialInput[materialIndex].color1 + materialInput[materialIndex].color2, 1.0);
}
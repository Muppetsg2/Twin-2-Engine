#version 430

in vec3 position;
in vec3 normal;
in vec2 texCoords;

in vec4 color1;
in vec4 color2;

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

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[];
};

void main()
{
	//FragColor = uColor;
	//FragColor = color1 + color2;
	FragColor = materialInput[materialIndex].color1 + materialInput[materialIndex].color2;
}
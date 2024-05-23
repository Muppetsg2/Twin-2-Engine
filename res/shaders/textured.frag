#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

layout (location = 3) flat in uint materialIndex;

layout (location = 0) out vec4 FragColor;


struct MaterialInput
{
	int empty;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[8];
};

struct TextureInput
{
	sampler2D texture1;
};

layout(location = 0) uniform TextureInput texturesInput[8];

void main()
{
	FragColor = texture(texturesInput[materialIndex].texture1, texCoords);
}
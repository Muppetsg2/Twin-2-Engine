#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

layout (location = 3) flat in uint materialIndex;

layout (location = 0) out vec4 FragColor;

// Material Input
struct MaterialInput
{
	vec4 color;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[8];
};

// Texture Input
struct TextureInput
{
	sampler2D texture;
};

layout(location = 0) uniform TextureInput texturesInput[8];

void main()
{
	FragColor = texture(texturesInput[materialIndex].texture, texCoords) * materialInput[materialIndex].color;
}
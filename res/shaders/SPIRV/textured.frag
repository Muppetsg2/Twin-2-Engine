#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

layout (location = 3) flat in uint materialIndex;

layout (location = 0) out vec4 FragColor;


struct MaterialInput
{
	int textureIndex;
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
	//FragColor = vec4(materialInput[materialIndex].color1 + materialInput[materialIndex].color2, 1.0);
	//FragColor = texture(materialInput[materialIndex].texture1, texCoords);

	FragColor = texture(texturesInput[materialIndex].texture1, texCoords);
}
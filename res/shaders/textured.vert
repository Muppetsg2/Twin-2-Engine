#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[1024];
} instanceData;

layout (std430, binding = 1) buffer MaterialIndexes {
    int materialIndex[1024];
} materialIndexes;


layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

struct MaterialInput
{
	int empty;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[8];
};


layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec2 texCoords;


layout (location = 3) flat out uint materialIndex;


void main()
{
    uint instanceId = gl_InstanceID;

    position = vec3(instanceData.transform[instanceId] * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(instanceData.transform[instanceId]))) * aNormal; 
    texCoords = aTexCoords;
    

    gl_Position = projection * view * instanceData.transform[instanceId] * vec4(aPos, 1.0);

    materialIndex = materialIndexes.materialIndex[instanceId];
}
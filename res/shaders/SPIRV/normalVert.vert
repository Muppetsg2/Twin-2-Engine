#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[3];
} instanceData;

layout (std430, binding = 1) buffer MaterialIndexes {
    int materialIndex[3];
} materialIndexes;


struct MaterialInput
{
   vec3 color1;
   vec3 color2;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[3];
};

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec2 texCoords;

//out vec4 color1;
//out vec4 color2;

layout (location = 3) flat out uint materialIndex;

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

void main()
{
    uint instanceId = gl_InstanceID;
    mat4 view = mat4(1);
    mat4 projection = mat4(1);

    position = vec3(instanceData.transform[instanceId] * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(instanceData.transform[instanceId]))) * aNormal; 
    texCoords = aTexCoords;
    

    gl_Position = projection * view * instanceData.transform[instanceId] * vec4(aPos, 1.0);

    //color1 = materialInput[materialIndexes.materialIndex[instanceId]].color1;
    //color2 = materialInput[materialIndexes.materialIndex[instanceId]].color2;

    materialIndex = materialIndexes.materialIndex[instanceId];
}
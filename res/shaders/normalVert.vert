#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[];
} instanceData;

layout (std430, binding = 1) buffer MaterialIndexes {
    int materialIndex[];
} materialIndexes;


struct MaterialInput
{
   vec4 color1;
   vec4 color2;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[];
};

out vec3 position;
out vec3 normal;
out vec2 texCoords;

out vec4 color1;
out vec4 color2;

flat out uint materialIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    uint instanceId = gl_InstanceID;

    position = vec3(model * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal; 
    texCoords = aTexCoords;
    

    gl_Position = projection * view * instanceData.transform[instanceId] * vec4(aPos, 1.0);

    color1 = materialInput[materialIndexes.materialIndex[instanceId]].color1;
    color2 = materialInput[materialIndexes.materialIndex[instanceId]].color2;

    materialIndex = materialIndexes.materialIndex[instanceId];
}
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
};

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[8];
} instanceData;

layout (std430, binding = 1) buffer MaterialIndexes {
    int materialIndex[8];
} materialIndexes;

struct MaterialInput
{
   vec4 color1;
   vec4 color2;
};

out vec3 position;
out vec2 texCoords;
out vec3 normal;

flat out uint materialIndex;

void main()
{
    uint instanceId = gl_InstanceID;

    position = vec3(instanceData.transform[instanceId] * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(instanceData.transform[instanceId]))) * aNormal; 
    texCoords = aTexCoords;

    gl_Position = projection * view * instanceData.transform[instanceId] * vec4(aPos, 1.0);

    materialIndex = materialIndexes.materialIndex[instanceId];
}
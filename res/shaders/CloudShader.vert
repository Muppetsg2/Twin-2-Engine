#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

//out vec3 FragPos;
out vec2 TexCoords;
//out vec3 Normal;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[8];
} instanceData;


void main()
{
    uint instanceId = gl_InstanceID;

    //FragPos = vec3(instanceData.transform[instanceId] * vec4(aPos, 1.0));
    //Normal = mat3(transpose(inverse(instanceData.transform[instanceId]))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * instanceData.transform[instanceId] * vec4(aPos, 1.0);
}

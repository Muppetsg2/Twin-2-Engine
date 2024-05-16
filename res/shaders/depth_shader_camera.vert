#version 430 core
layout (location = 0) in vec3 aPos;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[1024];
} instanceData;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

void main()
{
    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    gl_Position = projection * view * instanceData.transform[gl_InstanceID] * vec4(aPos, 1.0);
}
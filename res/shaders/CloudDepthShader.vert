#version 430 core
//CloudDepthShader.vert
layout (location = 0) in vec3 aPos;

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

//uniform mat4 lightSpaceMatrix;
//uniform mat4 model;
out vec3 FragPos;

void main()
{
    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    FragPos = vec3(instanceData.transform[gl_InstanceID] * vec4(aPos, 1.0));
    gl_Position = projection * view * instanceData.transform[gl_InstanceID] * vec4(aPos, 1.0);
}
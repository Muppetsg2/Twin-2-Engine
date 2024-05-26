#version 430 core
//CloudLightDepthShader.vert
layout (location = 0) in vec3 aPos;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[8];
} instanceData;

uniform mat4 projectionViewMatrix;
out vec3 FragPos;

void main()
{
    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    FragPos = vec3(instanceData.transform[gl_InstanceID] * vec4(aPos, 1.0));
    gl_Position = projectionViewMatrix * instanceData.transform[gl_InstanceID] * vec4(aPos, 1.0);
}
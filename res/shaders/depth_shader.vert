#version 430 core
layout (location = 0) in vec3 aPos;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[8];
} instanceData;

uniform mat4 lightSpaceMatrix;
//uniform mat4 model;

void main()
{
    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    gl_Position = lightSpaceMatrix * instanceData.transform[gl_InstanceID] * vec4(aPos, 1.0);
}
#version 450 core
layout (location = 0) in vec3 aPos;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[1024];
} instanceData;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instanceData.transform[gl_InstanceID] * vec4(aPos, 1.0);
}
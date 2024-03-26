#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std430, binding = 0) buffer PerInstanceData {
    mat4 transform[4];
    // Add other per-instance data as needed
} instanceData;

out vec3 position;
out vec3 normal;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    position = vec3(model * vec4(aPos, 1.0));
    //position = aPos;
    normal = mat3(transpose(inverse(model))) * aNormal; 
    //normal = aNormal; 
    texCoords = aTexCoords;
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_Position = projection * view * instanceData.transform[gl_InstanceID] * vec4(aPos, 1.0);
    //gl_Position = vec4(aPos, 1.0);
}
#version 450 core
layout (std140, binding = 1) uniform WindowData
{
    vec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

in VS_OUT {
    vec2 texCoord;
    vec3 normal;
    vec3 fragPos;
} fs_in;

out vec4 Color;

void main() 
{
    //Color = vec4(1.0);
    Color = vec4(pow(vec3(1.0), vec3(gamma)), 1.0);
}
#version 450 core

in VS_OUT {
    vec2 texCoord;
    vec3 normal;
    vec3 fragPos;
} fs_in;

out vec4 Color;

void main() 
{
    Color = vec4(1.0, 1.0, 1.0, 1.0);
}
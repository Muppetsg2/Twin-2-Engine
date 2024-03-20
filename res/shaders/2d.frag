#version 450 core

in VS_OUT {
    vec2 texCoord;
    vec3 normal;
    vec3 fragPos;
} fs_in;

out vec4 Color;

uniform vec3 color;

void main() 
{
    Color = vec4(color, 1.0);
}
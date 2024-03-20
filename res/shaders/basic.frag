#version 450 core

in VS_OUT {
    vec2 texCoord;
    vec3 normal;
    vec3 fragPos;
} fs_in;

out vec4 Color;

uniform vec3 color;
uniform vec3 lightDirection;

void main() 
{
    vec3 norm = normalize(fs_in.normal);
    float diff = max(dot(norm, normalize(-lightDirection)), 0.0);

    vec3 ambient = 0.25 * color;
    vec3 diffuse = diff * color;

    Color = vec4(ambient + diffuse, 1.0);
}
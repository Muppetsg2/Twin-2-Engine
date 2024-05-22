#version 450 core 

in VS_OUT {
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
	vec4 clipSpacePos;
    flat uint materialIndex; // flat = nie interpolowane
} fs_in;

out vec4 Color;

void main()  
{
	Color = vec4(fs_in.normal, 1.0);
}
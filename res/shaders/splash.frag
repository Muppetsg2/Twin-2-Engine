#version 450 core
in vec2 TexCoord;

out vec4 Color;

uniform sampler2D splashTexture;

void main()
{
	Color = vec4(pow(texture(splashTexture, TexCoord.xy).rgb, vec3(1.0/2.2)), 1.0);
}
#version 460

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

uniform vec4 uColor;
uniform bool uNoTexture = true;
uniform sampler2D texture_diffuse1;

void main()
{
	FragColor = uColor;
}
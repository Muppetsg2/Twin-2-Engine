#version 430

in vec3 position;
in vec3 normal;
in vec2 texCoords;

in vec4 color1;
in vec4 color2;

out vec4 FragColor;

uniform vec4 uColor;
uniform bool uNoTexture = true;
uniform sampler2D texture_diffuse1;


void main()
{
	//FragColor = uColor;
	FragColor = color1 + color2;
}
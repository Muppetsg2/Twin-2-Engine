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
	if (uNoTexture)
	{
		FragColor = vec4(0.0, 1.0, 1.0, 1.0);
	}
	else
	{
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);// * texture(texture_diffuse1, texCoords);
	}
}
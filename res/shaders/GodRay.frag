#version 430 core
//GodRay.frag

layout (location = 0) in vec3 FragModelPos;
//layout (location = 0) in vec3 position;
layout (location = 1) in vec3 fNormal;
//layout (location = 1) in vec3 normal;

layout (location = 0) out vec4 FragColor;

uniform float heigth;
uniform float heigthLowerTreshhold;
uniform float heigthHigherTreshhold;
uniform vec3 ray_color;
uniform sampler3D noiseTexture3d;

uniform float POS_MULT;
uniform vec3 NOISE_D_VEL_3D;
uniform float time;

void main() {
	vec3 N = normalize(fNormal);
	float alpha = texture(noiseTexture3d, N * POS_MULT + NOISE_D_VEL_3D * time).r;
	float alpha2 = 1;
	if (FragModelPos.y < heigthLowerTreshhold) {
		alpha2 = FragModelPos.y / heigthLowerTreshhold;
	}
	if (FragModelPos.y > heigthHigherTreshhold) {
		alpha2 = (FragModelPos.y - heigth) / (heigthLowerTreshhold - heigth);
	}

	FragColor = vec4(ray_color, alpha * alpha2);
}
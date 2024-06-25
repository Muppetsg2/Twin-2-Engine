#version 430 core
//GodRay.frag

//layout (location = 0) in vec3 FragModelPos;
//layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 fNormal;
//layout (location = 1) in vec3 normal;

layout (location = 0) in float FragModelPosY;
layout (location = 1) in vec3 fNormal;
layout (location = 2) in vec3 fPos;

layout (location = 0) out vec4 FragColor;

uniform float heigth;
uniform float heigthLowerTreshhold;
uniform float heigthHigherTreshhold;
uniform vec3 ray_color;
uniform sampler3D noiseTexture3d;

uniform float POS_MULT;
uniform vec3 NOISE_D_VEL_3D;
uniform float time;
uniform float minAlpha;
uniform float na;

layout(std140, binding = 3) uniform LightingData {
    vec3 AmbientLight;
	vec3 ViewerPosition;
	int shadingType;
};

void main() {
	vec3 N = normalize(fNormal);
	vec3 V = normalize(ViewerPosition - fPos);
	float edgeAlpha = 1.0 - pow(1 - dot(N, V), na);
	float alpha = minAlpha + (1.0 - minAlpha) * texture(noiseTexture3d, N * POS_MULT + NOISE_D_VEL_3D * time).r;
	float alpha2 = 1;
	if (FragModelPosY < heigthLowerTreshhold) {
		alpha2 = FragModelPosY / heigthLowerTreshhold;
	}
	if (FragModelPosY > heigthHigherTreshhold) {
		alpha2 = (FragModelPosY - heigth) / (heigthLowerTreshhold - heigth);
	}

	FragColor = vec4(ray_color, alpha * alpha2 * edgeAlpha);
}
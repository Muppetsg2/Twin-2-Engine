#version 430 core
//CityLights.frag

layout (location = 0) in vec3 fPos;
layout (location = 1) in vec3 fNormal;

layout (location = 0) out vec4 FragColor;

uniform vec3 light_color;
uniform vec3 back_light_color;
uniform float light_alpha;

uniform float kr = 1.0;
uniform float nr = 1.0;

layout(std140, binding = 3) uniform LightingData {
    vec3 AmbientLight;
	vec3 ViewerPosition;
	int shadingType;
};

void main() {
	vec3 V = normalize(ViewerPosition - fPos);
	vec3 N = normalize(fNormal);
	float blendf = kr * pow(1 - dot(N, V), nr);
	FragColor = vec4(light_color * (1.0 - blendf) + back_light_color * blendf, light_alpha);
	//FragColor = vec4(light_color, light_alpha);
}
#version 430 core
//CityLights.frag

//layout (location = 0) in vec3 FragModelPos;
//layout (location = 1) in vec3 fNormal;

layout (location = 0) out vec4 FragColor;

uniform vec3 light_color;
uniform float light_alpha;


void main() {
	FragColor = vec4(light_color, light_alpha);
}
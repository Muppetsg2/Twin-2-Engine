#version 430 core

layout (location = 0) in vec3 FragPos;

layout (location = 0) out vec4 FragColor;



vec3 cloudColor = vec3(1.0, 1.0, 1.0);
float ABSORPTION = 4.0;

uniform sampler2D viewerBackDepthMap;
uniform sampler2D lightFrontDepthMap;

//uniform vec3 lightDir;
//uniform vec3 cameraPos;
//uniform float stepSize;
//uniform float densityFactor;
float stepSize = 0.1;
float densityFactor = 0.7;


//LIGHTING BEGIN
struct PointLight {
	vec3 position;      // Position of the point light in world space
	vec3 color;         // Color of the point light
	float power;		// Light source power
	float constant;     // Constant attenuation
	float linear;       // Linear attenuation
	float quadratic;    // Quadratic attenuation
};

struct SpotLight {
	vec3 position;      // Position of the spot light in world space
	vec3 direction;     // Direction of the spot light
	float power;		// Light source power
	vec3 color;         // Color of the spot light
	float cutOff;       // Inner cutoff angle (in radians)
	float outerCutOff;  // Outer cutoff angle (in radians)
	float constant;     // Constant attenuation
	float linear;       // Linear attenuation
	float quadratic;    // Quadratic attenuation
};

struct DirectionalLight {
	vec3 direction;     // Direction of the spot light
	vec3 color;         // Color of the spot light
	mat4 lightSpaceMatrix;
	float power;		  // Light source power
};

layout (std430, binding = 2) buffer Lights {
	uint numberOfPointLights;
	uint numberOfSpotLights;
	uint numberOfDirLights;
    PointLight pointLights[8];
    SpotLight spotLights[8];
    DirectionalLight directionalLights[4];
};

layout(std140, binding = 3) uniform LightingData {
    vec3 AmbientLight;
	float shininness;
	vec3 ViewerPosition;
	int shadingType;
};

//LIGHTING END



void main()
{	
    vec2 viewerDepthCoord = gl_FragCoord * 0.5 + 0.5;
	float totalDistance = length(texture(viewerBackDepthMap, viewerDepthCoord).xyz - FragPos);
	float alpha = 1.0 - exp(-totalDistance * ABSORPTION);

    FragColor = vec4(cloudColor, alpha);
}
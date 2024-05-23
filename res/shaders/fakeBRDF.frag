#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) out vec4 clipSpacePos;

layout (location = 4) flat in uint materialIndex;

out vec4 FragColor;

layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
    float time;
    float deltaTime;
};

//struct MaterialInput
//{
//   vec4 color1;
//   vec4 color2;
//};

//layout(std140, binding = 2) uniform MaterialInputBuffer {
//    MaterialInput materialInput[8];
//};

struct TextureInput
{
	sampler2D brdfTexture;
};

layout(location = 0) uniform TextureInput texturesInput[8];

layout(std140, binding = 3) uniform LightingData {
    vec3 AmbientLight;
	vec3 ViewerPosition;
	int shadingType;
};

//shadow maps
uniform sampler2D DirLightShadowMaps[4];
uniform sampler2D occlusionMap;

//LIGHTING BEGIN
struct PointLight {
	vec3 position;      // Position of the point light in world space
	vec3 color;         // Color of the point light
	float power;		  // Light source power
	float constant;     // Constant attenuation
	float linear;       // Linear attenuation
	float quadratic;    // Quadratic attenuation
};

struct SpotLight {
	vec3 position;      // Position of the spot light in world space
	vec3 color;         // Color of the spot light
	vec3 direction;     // Direction of the spot light
	float power;		  // Light source power
	float innerCutOff;       // Inner cutoff angle (in radians)
	float outerCutOff;  // Outer cutoff angle (in radians)
	float constant;     // Constant attenuation
	float linear;       // Linear attenuation
	float quadratic;    // Quadratic attenuation
};
struct DirectionalLight {
	mat4 lightSpaceMatrix;
	vec3 color;         // Color of the spot light
	vec3 direction;     // Direction of the spot light
	float power;		  // Light source power
	uint shadowMapFBO;
	uint shadowMap;
};

layout (std430, binding = 2) buffer Lights {
	uint numberOfPointLights;
	uint numberOfSpotLights;
	uint numberOfDirLights;
	PointLight pointLights[8];
    SpotLight spotLights[8];
    DirectionalLight directionalLights[4];
};

// FAKE BRDF

vec4 fakeBRDF(sampler2D brdfTex, vec3 lightDirection, vec3 viewer)
{
	return texture(brdfTex, vec2(clamp(dot(normal, normalize(lightDirection)), 0.0, 1.0), dot(viewer, normal) * 0.5 + 0.5));
}

void main()
{

	//vec3 L = vec3(0.0);
    //vec3 N = normalize(normal);
    vec3 E = normalize(ViewerPosition - position);

	//float attenuation = 0.0;
	//float lambertian = 0.0;
	//float specular = 0.0;
	//float distance = 0.0;

	vec3 LightColor;

	for (uint i = 0; i < numberOfDirLights; ++i)
	{
        LightColor += fakeBRDF(texturesInput[materialIndex].brdfTexture, -directionalLights[i].direction, E).rgb;
    }
	
    FragColor = vec4(LightColor, 1.0);
    //FragColor = vec4(LightColor + AmbientLight, 1.0);
	//FragColor = vec4(pow(FragColor.rgb, vec3(gamma)), 1.0);
}
#version 430 core
//CloudShader.frag

layout (location = 0) in vec3 FragPos;

layout (location = 0) out vec4 FragColor;



vec3 cloudColor = vec3(1.0, 1.0, 1.0);
const float ABSORPTION = 5;
const float DENSITY_FAC = 0.7;
const float SEGMENT_LENGTH = 0.01;
const float SEGMENT_OFFSET = 0.001;
const float NOISE_D_VEL = 0.02;
vec3 NOISE_D_VEL_3D = vec3(-0.02, 0.0, 0.0);

const float LIGHT_STEP_LENGTH = 0.01;
const float CLOUD_LIGHT_MULTIPLIER = 50.0;

uniform sampler2D viewerBackDepthMap;
uniform sampler2D noiseTexture;
uniform sampler3D noiseTexture3d;
uniform sampler2D lightFrontDepthMap;
uniform float	  time;

//uniform vec3 lightDir;
//uniform vec3 cameraPos;
//uniform float stepSize;
//uniform float densityFactor;


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

//layout (std140, binding = 1) uniform WindowData
//{
//    ivec2 windowSize;
//    float nearPlane;
//    float farPlane;
//    float gamma;
//};

//LIGHTING END

float density_i(vec2 pos, float offset) {
	//return texture(noiseTexture, pos + offset + NOISE_D_VEL * time).r;
	return texture(noiseTexture, FragPos.xy * 0.1 + offset + NOISE_D_VEL * time).r * texture(noiseTexture, FragPos.xz * 0.1 + offset + NOISE_D_VEL * time).r * texture(noiseTexture, FragPos.zy * 0.1 + offset + NOISE_D_VEL * time).r;
	//return texture(noiseTexture, pos * 3.0 + offset + NOISE_D_VEL * time).r;
	//return texture(noiseTexture, mod(gl_FragCoord.xy, 0.5) + offset + NOISE_D_VEL * time).r * texture(noiseTexture, pos * 5.0 + offset + NOISE_D_VEL * time).r;
	//return texture(noiseTexture, vec2(fract(length(FragPos)), fract(distance(FragPos, ViewerPosition))) + offset + NOISE_D_VEL * time).r;
}

float density3d(vec3 pos) {
	float f = 2.07;
	//return texture(noiseTexture, pos.xy * f + NOISE_D_VEL_3D.xy * time).r * texture(noiseTexture, pos.xz * f +  NOISE_D_VEL_3D.xz * time).r * texture(noiseTexture, pos.zy * f + NOISE_D_VEL_3D.zy * time).r;
	//return sqrt(texture(noiseTexture, pos.xy * f + NOISE_D_VEL_3D.xy * time).r * texture(noiseTexture, pos.xz * f +  NOISE_D_VEL_3D.xz * time).r * texture(noiseTexture, pos.zy * f + NOISE_D_VEL_3D.zy * time).r);
	return texture(noiseTexture3d, pos * 0.7 + NOISE_D_VEL_3D * time).r * DENSITY_FAC;
}

vec4 probeLightDepthTex(vec3 pos) {
	vec4 projPos = directionalLights[0].lightSpaceMatrix * vec4(FragPos , 1.0);
    vec3 projCoords = projPos.xyz / projPos.w;
    projCoords = projCoords * 0.5 + 0.5;

	return texture(lightFrontDepthMap, projCoords.xy);
}

void main()
{	
    vec2 viewerDepthCoord = gl_FragCoord.xy;
	viewerDepthCoord.x = viewerDepthCoord.x / 1920.0;
	viewerDepthCoord.y = viewerDepthCoord.y / 1055.0;

	vec4 outPos = texture(viewerBackDepthMap, viewerDepthCoord);

	if (outPos.w == 0.0) {
		discard;
	}

	vec3 dir = normalize(FragPos - ViewerPosition);

	float totalDistance = distance(vec3(outPos), FragPos);
	float dist = 0.0;
	float number_of_steps = floor(totalDistance / SEGMENT_LENGTH);
	
	vec4 lightPos = probeLightDepthTex(FragPos);
	float lightTotalDistance = distance(vec3(lightPos), FragPos);

	//for (float i = 0.0; i < number_of_steps; ) {
	//	dist += SEGMENT_LENGTH * density_i(viewerDepthCoord, SEGMENT_OFFSET * i);
	//	i += 1.0;
	//}
	//dist += (totalDistance - number_of_steps * SEGMENT_LENGTH) * density_i(viewerDepthCoord, SEGMENT_OFFSET * number_of_steps);
	

	for (float i = 0.0; i < number_of_steps; ) {
		dist += SEGMENT_LENGTH * density3d(ViewerPosition + dir * SEGMENT_LENGTH * i);
		i += 1.0;
	}	
	dist += (totalDistance - number_of_steps * SEGMENT_LENGTH) * density3d(ViewerPosition + dir * totalDistance);

	


	float alpha = 1.0 - exp(-dist * ABSORPTION);
	//float alpha = exp(-dist * ABSORPTION);
	//float alpha = 1.0 - exp(-totalDistance * ABSORPTION);
	//float alpha = 1.0 - pow(exp(-totalDistance * ABSORPTION), 3.17);
	
	if (alpha < 0.3) {
		discard;
	}

    FragColor = vec4(cloudColor, alpha);

	//float n = density_i(viewerDepthCoord, SEGMENT_OFFSET);
    //FragColor = vec4(n, n, n, 1.0);
    //FragColor = vec4(probeLightDepthTex(FragPos), 1.0);
}
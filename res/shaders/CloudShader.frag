#version 430 core
//CloudShader.frag

layout (location = 0) in vec3 FragPos;

layout (location = 0) out vec4 FragColor;


uniform float ABSORPTION		= 7.0;
uniform float DENSITY_FAC		= 0.7;
uniform float NUMBER_OF_STEPS	= 200;
uniform float CLOUD_LIGHT_STEPS	= 200;	//
uniform float CLOUD_LIGHT_MULTIPLIER = 50.0;
uniform float CLOUD_EXPOSURE = 1.0;
//uniform float EXTINCTION_MULT	= 4;	//
uniform float POS_MULT			= 2.0;
uniform float APLPHA_TRESHOLD	= 0.25;
uniform float DENSITY_TRESHOLD	= 0.2;
uniform vec3  NOISE_D_VEL_3D	= vec3(-0.05, 0.0, 0.0);

vec3 cloudColor = vec3(1.0, 1.0, 1.0);
const float PI = 3.14159265;
const float DUAL_LOBE_WEIGHT = 0.7;
const vec3 EXTINCTION_MULT = vec3(0.8, 0.8, 1.0);
const float NOISE_D_VEL = 0.02;

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


vec4 probeLightDepthTex(vec3 pos) {
	vec4 projPos = directionalLights[0].lightSpaceMatrix * vec4(FragPos , 1.0);
    vec3 projCoords = projPos.xyz / projPos.w;
    projCoords = projCoords * 0.5 + 0.5;

	return texture(lightFrontDepthMap, projCoords.xy);
}

float density_i(vec2 pos, float offset) {
	return texture(noiseTexture, FragPos.xy * 0.1 + offset + NOISE_D_VEL * time).r * texture(noiseTexture, FragPos.xz * 0.1 + offset + NOISE_D_VEL * time).r * texture(noiseTexture, FragPos.zy * 0.1 + offset + NOISE_D_VEL * time).r;
}

float density3d(vec3 pos) {
	return texture(noiseTexture3d, pos * POS_MULT + NOISE_D_VEL_3D * time).r * DENSITY_FAC;
}


float remap(float v, float minOld, float maxOld, float minNew, float maxNew) {
    return minNew + (v-minOld) * (maxNew - minNew) / (maxOld-minOld);
}

float HenyeyGreenstein(float g, float mu) {
	float gg = g * g;
	return (1.0 / (4.0 * PI))  * ((1.0 - gg) / pow(1.0 + gg - 2.0 * g * mu, 1.5));
}

float DualHenyeyGreenstein(float g, float costh) {
	return mix(HenyeyGreenstein(-g, costh), HenyeyGreenstein(g, costh), DUAL_LOBE_WEIGHT);
}

float PhaseFunction(float g, float costh) {
	return DualHenyeyGreenstein(g, costh);
}

vec3 MultipleOctaveScattering(float density, float mu) {
	float attenuation = 0.2;
	float contribution = 0.2;
	float phaseAttenuation = 0.5;
	
	float a = 1.0;
	float b = 1.0;
	float c = 1.0;
	float g = 0.85;
	const float scatteringOctaves = 4.0;
	
	vec3 luminance = vec3(0.0);
	
	for (float i = 0.0; i < scatteringOctaves; ++i) {
		float phaseFunction = PhaseFunction(0.3 * c, mu);
		vec3 beers = vec3(exp(-density * EXTINCTION_MULT * a));
		
		luminance += b * phaseFunction * beers;
		
		a *= attenuation;
		b *= contribution;
		c *= (1.0 - phaseAttenuation);
	}
	return luminance;
}

vec3 CalculateLightEnergy(vec3 lightOrigin, vec3 rayDir) 
{
	float maxDistance = 0.05;
	vec4 lightInPos = probeLightDepthTex(lightOrigin);
	if (lightInPos.w != 0.0) {
		maxDistance = distance(lightInPos.xyz, lightOrigin);
	}

	vec3 lightDirection = normalize(lightInPos.xyz - lightOrigin);
	float mu = dot(rayDir, lightDirection);

	float stepLength = maxDistance / CLOUD_LIGHT_STEPS;
	float lightRayDensity = 0.0;
	
	for(float j = 0.0; j < CLOUD_LIGHT_STEPS; j++) {
		lightRayDensity += stepLength * density3d(lightOrigin + lightDirection * stepLength * j);
	}

	vec3 beersLaw = MultipleOctaveScattering(lightRayDensity, mu);
	vec3 powder = 1.0 - exp(-lightRayDensity * 2.0 * EXTINCTION_MULT);

	return beersLaw * mix(2.0 * powder, vec3(1.0), remap(mu, -1.0, 1.0, 0.0, 1.0));
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
	float step_len = totalDistance / NUMBER_OF_STEPS;
	
	//vec4 lightPos = probeLightDepthTex(FragPos);
	//float lightTotalDistance = distance(vec3(lightPos), FragPos);


	vec3 transmittance = vec3(1.0, 1.0, 1.0);
	vec3 scattering = vec3(0.0, 0.0, 0.0);
	vec3 sunLight = directionalLights[0].color * CLOUD_LIGHT_MULTIPLIER;


	float density_i = 0.0;
	vec3 samplePos = vec3(0.0, 0.0, 0.0);
	for (float i = 0.0; i < NUMBER_OF_STEPS; ++i) {
		samplePos = FragPos + dir * step_len * i;
		density_i = density3d(samplePos);

		if (density_i  > DENSITY_TRESHOLD) {
			dist += step_len * density_i;

			if (density_i > 0.01) {
				vec3 luminance = AmbientLight + sunLight * CalculateLightEnergy(samplePos, dir);
				vec3 transmit = exp(-density_i * step_len * EXTINCTION_MULT);
				vec3 integScatt = density_i * (luminance - luminance * transmit) / density_i;
				
				scattering += transmittance * integScatt;
				transmittance *= transmit;  
				
				if (length(transmittance) <= 0.01) {
					transmittance = vec3(0.0);
					break;
				}
			}
		}
		//dist += step_len * density3d(FragPos + dir * step_len * i);
	}	


	float alpha = 1.0 - exp(-dist * ABSORPTION);
	
	if (alpha < APLPHA_TRESHOLD) {
		discard;
	}

    //FragColor = vec4(cloudColor, alpha);

	vec3 colour = transmittance + scattering * CLOUD_EXPOSURE;
	FragColor = vec4(colour, alpha);
	//FragColor = vec4(colour, 1.0);
	//FragColor = vec4(colour, 1.0 - transmittance);
}
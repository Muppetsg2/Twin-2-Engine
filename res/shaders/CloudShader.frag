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
//const float CLOUD_LIGHT_MULTIPLIER = 50.0;

vec2 resolution = vec2(1920.0, 1055.0);

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


float PI = 3.14159265359;

float saturate(float x) {
	 return clamp(x, 0.0, 1.0);
}

vec3 saturate3(vec3 x) {
	return clamp(x, vec3(0.0), vec3(1.0));
}


float linearstep(float minValue, float maxValue, float v) {
	return clamp((v - minValue) / (maxValue - minValue), 0.0, 1.0);
}

float inverseLerp(float minValue, float maxValue, float v) {
	return (v - minValue) / (maxValue - minValue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax) {
	float t = inverseLerp(inMin, inMax, v);
	return mix(outMin, outMax, t);
}

float circularOut(float t) {
	return sqrt((2.0 - t) * t);
}

float sdfBox( vec3 p, vec3 b ) {
	vec3 q = abs(p) - b;
	return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdCutSphere( vec3 p, float r, float h )
{
	// sampling independent computations (only depend on shape)
	float w = sqrt(r*r-h*h);
	
	// sampling dependant computations
	vec2 q = vec2( length(p.xz), p.y );
	float s = max( (h-r)*q.x*q.x+w*w*(h+r-2.0*q.y), h*q.x-w*q.y );
	return (s<0.0) ? length(q)-r :
	       (q.x<w) ? h - q.y     :
	                 length(q-vec2(w,h));
}

vec3 ACESToneMap(vec3 color){	
	mat3 m1 = mat3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
	);
	mat3 m2 = mat3(
        1.60475, -0.10208, -0.00327,
        -0.53108,  1.10813, -0.07276,
        -0.07367, -0.00605,  1.07602
	);
	vec3 v = m1 * color;    
	vec3 a = v * (v + 0.0245786) - 0.000090537;
	vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
	return saturate3(m2 * (a / b));	
}

struct AABB {
	vec3 min;
	vec3 max;
};

struct AABBIntersectResult {
	float near;
	float far;
};

bool insideAABB(vec3 rayOrigin, AABB box) {
	return all(lessThanEqual(rayOrigin, box.max)) && all(lessThan(box.min, rayOrigin));
}

// https://gist.github.com/DomNomNom/46bb1ce47f68d255fd5d
// Compute the near and far intersections using the slab method.
// No intersection if tNear > tFar.
AABBIntersectResult intersectAABB(vec3 rayOrigin, vec3 rayDir, AABB box) {
    vec3 tMin = (box.min - rayOrigin) / rayDir;
    vec3 tMax = (box.max - rayOrigin) / rayDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return AABBIntersectResult(tNear, tFar);
}

const float GOLDEN_RATIO = 1.61803398875;
const vec3 EXTINCTION_MULT = vec3(0.8, 0.8, 1.0);

const float DUAL_LOBE_WEIGHT = 0.7;
const float AMBIENT_STRENGTH = 0.1;

const float CLOUD_LIGHT_MULTIPLIER = 50.0;
vec3 CLOUD_LIGHT_DIR = normalize(vec3(-1.0, 0.0, 0.0));

const float CLOUD_EXPOSURE = 1.0;
const float CLOUD_STEPS_MIN = 16.0;
const float CLOUD_STEPS_MAX = 128.0;
const float CLOUD_LIGHT_STEPS = 12.0;
const float CLOUD_DENSITY = 0.5;

const vec3 CLOUD_OFFSET = vec3(0.0, 0.0, 0.0);

vec3 CLOUD_SIZE = vec3(4000.0);
vec3 CLOUD_BOUNDS_MIN;
vec3 CLOUD_BOUNDS_MAX;

const float CLOUD_BASE_STRENGTH = 0.8;
const float CLOUD_DETAIL_STRENGTH = 0.2;

const vec3 CLOUD_COLOUR = vec3(1.0);
const float CLOUD_FALLOFF = 25.0;


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


vec4 SamplePerlinWorleyNoise(vec3 pos) {
	vec3 coord = pos.xzy * vec3(1.0 / 32.0, 1.0 / 32.0, 1.0 / 64.0) * 1.0;
	vec4 s = texture(noiseTexture3d, coord);
	
	return s;
}


float SampleLowResolutionCloudMap(vec3 p) {
	float sdfValue = sdfBox(p, vec3(50.0));
	
	sdfValue = sdCutSphere(p, 60.0, -40.0);
	sdfValue = min(sdfValue, sdCutSphere(p - vec3(60.0, -20.0, 0.0), 40.0, -20.0));
	sdfValue = min(sdfValue, sdCutSphere(p - vec3(-60.0, -20.0, -50.0), 20.0, -20.0));
	
	return sdfValue;
}

float SampleHighResolutionCloudDetail(float cloudSDF, vec3 worldPos, vec3 cameraOrigin, float curTime) {
	float cloud = circularOut(linearstep(0.0, -CLOUD_FALLOFF, cloudSDF)) * 0.85;
	
	if(cloud > 0.0) {
		vec3 samplePos = worldPos + vec3(-2.0 * time, 0.0, time) * 1.5;
		
		float shapeSize = 0.4;
		vec4 perlinWorleySample = SamplePerlinWorleyNoise(samplePos * shapeSize);
		
		float shapeStrength = CLOUD_BASE_STRENGTH;
		cloud = saturate(remap(cloud, shapeStrength * perlinWorleySample.x, 1.0, 0.0, 1.0));
		
		if(cloud > 0.0) {
			float distToSample = distance(ViewerPosition, worldPos);
			float t_detailDropout = smoothstep(1000.0, 800.0, distToSample);
			
			if (t_detailDropout > 0.0) {
				samplePos += vec3(4.0 * time, 3.0 * time, 2.0 * time) * 0.01;
				
				float detailSize = 1.8;
				float detailStrength = CLOUD_DETAIL_STRENGTH * t_detailDropout;
				float detail = SamplePerlinWorleyNoise(detailSize * samplePos).y;
				cloud = saturate(remap(cloud, detailStrength * detail, 1.0, 0.0, 1.0));
			}
		}
	}
	
	return cloud * CLOUD_DENSITY;
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
	
	for (float i = 0.0; i < scatteringOctaves; i++) {
		float phaseFunction = PhaseFunction(0.3 * c, mu);
		vec3 beers = exp(-density * EXTINCTION_MULT * a);
		
		luminance += b * phaseFunction * beers;
		
		a *= attenuation;
		b *= contribution;
		c *= (1.0 - phaseAttenuation);
	}
	return luminance;
}

vec4 probeLightDepthTex(vec3 pos) {
	vec4 projPos = directionalLights[0].lightSpaceMatrix * vec4(FragPos , 1.0);
    vec3 projCoords = projPos.xyz / projPos.w;
    projCoords = projCoords * 0.5 + 0.5;

	return texture(lightFrontDepthMap, projCoords.xy);
}

vec3 CalculateLightEnergy(vec3 lightOrigin, vec3 lightDirection, vec3 cameraOrigin, float mu, float maxDistance, float curTime) 
{
	vec4 lightInPos = probeLightDepthTex(lightOrigin);
	if (lightInPos.w == 0.0) {
		maxDistance = 0.05;
	}
	else {
		maxDistance = distance(lightInPos.xyz, lightOrigin);
	}

	float stepLength = maxDistance / CLOUD_LIGHT_STEPS;
	float lightRayDensity = 0.0;
	float distAccumulated = 0.0;

	for(float j = 0.0; j < CLOUD_LIGHT_STEPS; j++) {
		vec3 lightSamplePos = lightOrigin + lightDirection * distAccumulated;
		
		float cloudSDF = SampleLowResolutionCloudMap(lightSamplePos);
		
		lightRayDensity += SampleHighResolutionCloudDetail(cloudSDF, lightSamplePos, cameraOrigin, curTime) * stepLength;
		distAccumulated += stepLength;
	}

	vec3 beersLaw = MultipleOctaveScattering(lightRayDensity, mu);
	vec3 powder = 1.0 - exp(-lightRayDensity * 2.0 * EXTINCTION_MULT);

	return beersLaw * mix(2.0 * powder, vec3(1.0), remap(mu, -1.0, 1.0, 0.0, 1.0));
}

struct ScatteringTransmittance {
	  vec3 scattering;
	  vec3 transmittance;
}; //noiseTexture


ScatteringTransmittance CloudMarch(vec2 pixelCoords, vec3 cameraOrigin, vec3 cameraDirection, float curTime) 
{
	ScatteringTransmittance result;
	result.scattering = vec3(0.0);
	result.transmittance = vec3(1.0);

	vec3 sunDirection = CLOUD_LIGHT_DIR;
	vec3 sunLightColour = vec3(1.0);
	vec3 sunLight = sunLightColour * CLOUD_LIGHT_MULTIPLIER;
	vec3 ambient = vec3(AMBIENT_STRENGTH * sunLightColour);
	
	vec2 fragNDCoord = gl_FragCoord.xy / resolution;
	vec4 FragOutPos = texture(viewerBackDepthMap, fragNDCoord);
	if (FragOutPos.w == 0.0) {
		discard;
	}
	vec3 dir = normalize(FragOutPos.xyz - FragPos);
	
	// TODO: Cap steps based on distance
	vec2 aspect = vec2(1.0, resolution.y / resolution.x);
	float blueNoiseSample = texture2D(noiseTexture, (pixelCoords / resolution + 0.5) * aspect * (resolution.x / 32.0)).x;

	blueNoiseSample = fract(blueNoiseSample + float(int(curTime * 144.0) % 32) * GOLDEN_RATIO);
	
	float mu = dot(dir, sunDirection);
	float phaseFunction = PhaseFunction(0.3, mu);

	
	float distNearToFar = distance(FragPos, FragOutPos.xyz);//rayCloudIntersection.far - rayCloudIntersection.near;
	float stepDropoff = linearstep(1.0, 0.0, pow(dot(vec3(0.0, 1.0, 0.0), dir), 4.0));
	
	const int NUM_COUNT = 16;
	float lqStepLength = distNearToFar / CLOUD_STEPS_MIN; 
	float hqStepLength = lqStepLength / float(NUM_COUNT);
	float numCloudSteps = CLOUD_STEPS_MAX;
	
	float offset = lqStepLength * blueNoiseSample;
	float distTravelled = 0.0;
	
	int hqMarcherCountdown = 0;
	float previousStepLength = 0.0;
	
	vec3 samplePos = FragPos;
	for (float i = 0.0; i < numCloudSteps; i++) {
		if (distTravelled > distNearToFar) {
			break;
		}
		
		//float cloudMapSDFSample = SampleLowResolutionCloudMap(samplePos);
		float currentStepLength = SampleLowResolutionCloudMap(samplePos);//cloudMapSDFSample;
		
		if (hqMarcherCountdown <= 0) {
			if (currentStepLength < hqStepLength) {
				// Hit some clouds, step back
				hqMarcherCountdown = NUM_COUNT;
				
				distTravelled += hqStepLength * blueNoiseSample;
			} else {
				distTravelled += currentStepLength;
				continue;
			}
		}
		
		if (hqMarcherCountdown > 0) {
			hqMarcherCountdown--;
			
			if (currentStepLength < 0.0) {
				hqMarcherCountdown = NUM_COUNT;
				
				float extinction = SampleHighResolutionCloudDetail(currentStepLength, samplePos, cameraOrigin, curTime);
				
				if (extinction > 0.01) {
					vec3 luminance = ambient + sunLight * CalculateLightEnergy(samplePos, sunDirection, cameraOrigin, mu, 50.0, curTime);
					vec3 transmittance = exp(-extinction * hqStepLength * EXTINCTION_MULT);
					vec3 integScatt = extinction * (luminance - luminance * transmittance) / extinction;
					
					result.scattering += result.transmittance * integScatt;
					result.transmittance *= transmittance;  
					
					if (length(result.transmittance) <= 0.01) {
						result.transmittance = vec3(0.0);
						break;
					}
				}
			}
			
			distTravelled += hqStepLength;
		}
		
		samplePos = FragPos + dir * distTravelled;
		previousStepLength = currentStepLength;
	}
	
	result.scattering = result.scattering * CLOUD_COLOUR; //col3(result.scattering) * CLOUD_COLOUR;
	result.transmittance = saturate3(result.transmittance);
	return result;
}

mat3 MakeCamera(vec3 ro, vec3 rd, vec3 ru) {
	vec3 z = normalize(rd - ro);
	vec3 cp = ru;
	vec3 x = normalize(cross(z, cp));
	vec3 y = cross(x, z);
  return mat3(x, y, z);
}


void main() 
{
	vec2 pixelCoords = gl_FragCoord.xy;
	
	CLOUD_SIZE = vec3(100.0);
	CLOUD_BOUNDS_MIN = CLOUD_OFFSET - CLOUD_SIZE;
	CLOUD_BOUNDS_MAX = CLOUD_OFFSET + CLOUD_SIZE;
	CLOUD_LIGHT_DIR = directionalLights[0].direction;
	
	vec3 rayOrigin = vec3(200.0, 50.0, -150.0) * 0.75;
	vec3 rayLookAt = vec3(80.0, -10.0, 45.0) + CLOUD_LIGHT_DIR * 150.0;
	mat3 camera = MakeCamera(rayOrigin, rayLookAt, vec3(0.0, 1.0, 0.0));
	
	vec2 rayCoords = (2.0 * (gl_FragCoord.xy - 0.5) - resolution) / resolution.y;
	vec3 rayDir = normalize(vec3(rayCoords, 2.0));

	ScatteringTransmittance scatterTransmittance = CloudMarch(pixelCoords, rayOrigin, normalize(camera * rayDir), time);
	
	vec3 colour;
	
	colour = scatterTransmittance.transmittance + scatterTransmittance.scattering * CLOUD_EXPOSURE;
	colour = ACESToneMap(colour);

	FragColor = vec4(colour, 1.0);
}


//void main()
//{	
//    vec2 viewerDepthCoord = gl_FragCoord.xy;
//	viewerDepthCoord.x = viewerDepthCoord.x / 1920.0;
//	viewerDepthCoord.y = viewerDepthCoord.y / 1055.0;
//
//	vec4 outPos = texture(viewerBackDepthMap, viewerDepthCoord);
//
//	if (outPos.w == 0.0) {
//		discard;
//	}
//
//	vec3 dir = normalize(FragPos - ViewerPosition);
//
//	float totalDistance = distance(vec3(outPos), FragPos);
//	float dist = 0.0;
//	float number_of_steps = floor(totalDistance / SEGMENT_LENGTH);
//	
//	vec4 lightPos = probeLightDepthTex(FragPos);
//	float lightTotalDistance = distance(vec3(lightPos), FragPos);
//
//	//for (float i = 0.0; i < number_of_steps; ) {
//	//	dist += SEGMENT_LENGTH * density_i(viewerDepthCoord, SEGMENT_OFFSET * i);
//	//	i += 1.0;
//	//}
//	//dist += (totalDistance - number_of_steps * SEGMENT_LENGTH) * density_i(viewerDepthCoord, SEGMENT_OFFSET * number_of_steps);
//	
//
//	for (float i = 0.0; i < number_of_steps; ) {
//		dist += SEGMENT_LENGTH * density3d(ViewerPosition + dir * SEGMENT_LENGTH * i);
//		i += 1.0;
//	}	
//	dist += (totalDistance - number_of_steps * SEGMENT_LENGTH) * density3d(ViewerPosition + dir * totalDistance);
//
//	
//
//
//	float alpha = 1.0 - exp(-dist * ABSORPTION);
//	//float alpha = exp(-dist * ABSORPTION);
//	//float alpha = 1.0 - exp(-totalDistance * ABSORPTION);
//	//float alpha = 1.0 - pow(exp(-totalDistance * ABSORPTION), 3.17);
//	
//	if (alpha < 0.3) {
//		discard;
//	}
//
//    FragColor = vec4(cloudColor, alpha);
//
//	//float n = density_i(viewerDepthCoord, SEGMENT_OFFSET);
//    //FragColor = vec4(n, n, n, 1.0);
//    //FragColor = vec4(probeLightDepthTex(FragPos), 1.0);
//}
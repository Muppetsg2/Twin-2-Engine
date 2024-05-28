#version 430 core
//CloudShader.frag

layout (location = 0) in vec3 FragPos;

layout (location = 0) out vec4 FragColor;



//vec3 cloudColor = vec3(1.0, 1.0, 1.0);
//const float ABSORPTION = 5;
//const float DENSITY_FAC = 0.7;
//const float SEGMENT_LENGTH = 0.01;
//const float SEGMENT_OFFSET = 0.001;
//const float NOISE_D_VEL = 0.02;
//vec3 NOISE_D_VEL_3D = vec3(-0.02, 0.0, 0.0);

//const float LIGHT_STEP_LENGTH = 0.01;
////const float CLOUD_LIGHT_MULTIPLIER = 50.0;

//vec2 resolution = vec2(1920.0, 1055.0);

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


// Shape settings
//vec4 params;
//int3 mapSize;
float densityMultiplier = 1.07;
float densityOffset = -4.27;
float scale = 0.62;
float detailNoiseScale = 3;
float detailNoiseWeight = 3.42;
vec3 detailWeights = vec3(1, 0.5, 0.5);
vec4 shapeNoiseWeights = vec4(1, 0.48, 0.15, 0);
float forwardScattering = 0.811;
float backScattering = 0.33;
float baseBrightness = 1;
float phaseFactor = 0.488;
vec4 phaseParams = vec4(forwardScattering, backScattering, baseBrightness, phaseFactor);

// March settings
int numStepsLight = 8;
float rayOffsetStrength = 10;

vec3 boundsMin = vec3(-5.0, 5.0, -5.0);
vec3 boundsMax = vec3(5.0, 9.0, 5.0);

vec3 shapeOffset  = vec3(190.44, 0, 0);//vec3(190.44, 0, 0);
vec3 detailOffset = vec3(51.25, 0, 0);//vec3(51.25, y: 0, z: 0);

// Light settings
float lightAbsorptionTowardSun = 1.21;
float lightAbsorptionThroughCloud = 0.75;
float darknessThreshold = 0.15;
vec4 _LightColor0 = vec4(1.0, 1.0, 1.0, 1.0);
vec4 colA = vec4(0.8915094, 0.94486547, 1, 1);
vec4 colB = vec4(0.44313726, 0.6431373, 0.8, 1);

// Animation settings
float timeScale = 1;
float baseSpeed = 0.5;
float detailSpeed = 1;


float saturate(float x) {
	 return clamp(x, 0.0, 1.0);
}

//vec3 MultipleOctaveScattering(float density, float mu) {
//	float attenuation = 0.2;
//	float contribution = 0.2;
//	float phaseAttenuation = 0.5;
//	
//	float a = 1.0;
//	float b = 1.0;
//	float c = 1.0;
//	float g = 0.85;
//	const float scatteringOctaves = 4.0;
//	
//	vec3 luminance = vec3(0.0);
//	
//	for (float i = 0.0; i < scatteringOctaves; i++) {
//		float phaseFunction = PhaseFunction(0.3 * c, mu);
//		vec3 beers = exp(-density * EXTINCTION_MULT * a);
//		
//		luminance += b * phaseFunction * beers;
//		
//		a *= attenuation;
//		b *= contribution;
//		c *= (1.0 - phaseAttenuation);
//	}
//	return luminance;
//}

float remap(float v, float minOld, float maxOld, float minNew, float maxNew) {
    return minNew + (v-minOld) * (maxNew - minNew) / (maxOld-minOld);
}

vec4 probeLightDepthTex(vec3 pos) {
	vec4 projPos = directionalLights[0].lightSpaceMatrix * vec4(FragPos , 1.0);
    vec3 projCoords = projPos.xyz / projPos.w;
    projCoords = projCoords * 0.5 + 0.5;

	return texture(lightFrontDepthMap, projCoords.xy);
}

vec2 rayBoxDst(vec3 boundsMin, vec3 boundsMax, vec3 rayOrigin, vec3 invRaydir) {
    // Adapted from: http://jcgt.org/published/0007/03/04/
    vec3 t0 = (boundsMin - rayOrigin) * invRaydir;
    vec3 t1 = (boundsMax - rayOrigin) * invRaydir;
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);
    
    float dstA = max(max(tmin.x, tmin.y), tmin.z);
    float dstB = min(tmax.x, min(tmax.y, tmax.z));

    // CASE 1: ray intersects box from outside (0 <= dstA <= dstB)
    // dstA is dst to nearest intersection, dstB dst to far intersection

    // CASE 2: ray intersects box from inside (dstA < 0 < dstB)
    // dstA is the dst to intersection behind the ray, dstB is dst to forward intersection

    // CASE 3: ray misses box (dstA > dstB)

    float dstToBox = max(0, dstA);
    float dstInsideBox = max(0, dstB - dstToBox);
    return vec2(dstToBox, dstInsideBox);
}


// Henyey-Greenstein
float hg(float a, float g) {
    float g2 = g * g;
    return (1 - g2) / (4 * 3.1415 * pow(1 + g2 - 2 * g * (a), 1.5));
}

float phase(float a) {
    float blend = 0.5;
    float hgBlend = hg(a, phaseParams.x) * (1 - blend) + hg(a, -phaseParams.y) * blend;
    return phaseParams.z + hgBlend*phaseParams.w;
}



vec2 squareUV(vec2 uv) {
    //float width = _ScreenParams.x;
    //float height =_ScreenParams.y;
    //float minDim = min(width, height);
    float scale = 1000;
    //float x = uv.x * width;
    //float y = uv.y * height;
    return gl_FragCoord.xy / scale;//float2 (x/scale, y/scale);
}

float density3d(vec3 pos) {
	float f = 2.07;
	//return texture(noiseTexture, pos.xy * f + NOISE_D_VEL_3D.xy * time).r * texture(noiseTexture, pos.xz * f +  NOISE_D_VEL_3D.xz * time).r * texture(noiseTexture, pos.zy * f + NOISE_D_VEL_3D.zy * time).r;
	//return sqrt(texture(noiseTexture, pos.xy * f + NOISE_D_VEL_3D.xy * time).r * texture(noiseTexture, pos.xz * f +  NOISE_D_VEL_3D.xz * time).r * texture(noiseTexture, pos.zy * f + NOISE_D_VEL_3D.zy * time).r);
	//return texture(noiseTexture3d, pos * 0.7 + NOISE_D_VEL_3D * time).r * DENSITY_FAC;
	return texture(noiseTexture3d, pos * 0.7).r;
}


float sampleDensity(vec3 rayPos) {
    // Constants:
    const int mipLevel = 0;
    const float baseScale = 1/1000.0;
    const float offsetSpeed = 1/100.0;

    // Calculate texture sample positions
    float _time = time * timeScale;
    vec3 size = boundsMax - boundsMin;
    vec3 boundsCentre = (boundsMin + boundsMax) * 0.5;
    vec3 uvw = (size * 0.5 + rayPos) * baseScale * scale;
    vec3 shapeSamplePos = uvw + shapeOffset * offsetSpeed + vec3(_time, _time * 0.1, _time * 0.2) * baseSpeed;

    // Calculate falloff at along x/z edges of the cloud container
    const float containerEdgeFadeDst = 0.5; //50
    float dstFromEdgeX = min(containerEdgeFadeDst, min(rayPos.x - boundsMin.x, boundsMax.x - rayPos.x));
    float dstFromEdgeZ = min(containerEdgeFadeDst, min(rayPos.z - boundsMin.z, boundsMax.z - rayPos.z));
    float edgeWeight = min(dstFromEdgeZ, dstFromEdgeX) / containerEdgeFadeDst;
    
    // Calculate height gradient from weather map
    float gMin = 0.2;
    float gMax = 0.7;
    float heightPercent = (rayPos.y - boundsMin.y) / size.y;
    float heightGradient = saturate(remap(heightPercent, 0.0, gMin, 0, 1)) * saturate(remap(heightPercent, 1, gMax, 0, 1));
    heightGradient *= edgeWeight;

    // Calculate base shape density
    //vec4 shapeNoise = NoiseTex.SampleLevel(samplerNoiseTex, shapeSamplePos, mipLevel);
    vec4 normalizedShapeWeights = normalize(shapeNoiseWeights);//shapeNoiseWeights / dot(shapeNoiseWeights, 1);
    //float shapeFBM = density3d(shapeSamplePos) * heightGradient;//dot(vec4(density3d(shapeSamplePos), 0.0, 0.0, 0.0), normalizedShapeWeights) * heightGradient;
    float shapeFBM = dot(vec4(density3d(shapeSamplePos), 0.0, 0.0, 0.0), normalizedShapeWeights) * heightGradient;
    float baseShapeDensity = shapeFBM + densityOffset * 0.1;

    // Save sampling from detail tex if shape density <= 0
    if (baseShapeDensity > 0) {
        //// Sample detail noise
        //vec3 detailSamplePos = uvw * detailNoiseScale + detailOffset * offsetSpeed + vec3(time*.4,-time,time*0.1)*detailSpeed;
        //vec4 detailNoise = DetailNoiseTex.SampleLevel(samplerDetailNoiseTex, detailSamplePos, mipLevel);
        //vec3 normalizedDetailWeights = detailWeights / dot(detailWeights, 1);
        //float detailFBM = dot(detailNoise, normalizedDetailWeights);
		//
        //// Subtract detail noise from base shape (weighted by inverse density so that edges get eroded more than centre)
        //float oneMinusShape = 1 - shapeFBM;
        //float detailErodeWeight = oneMinusShape * oneMinusShape * oneMinusShape;
        float cloudDensity = baseShapeDensity;// - (1-detailFBM) * detailErodeWeight * detailNoiseWeight;

        return cloudDensity * densityMultiplier * 0.1;
    }
    return 0;
}

// Calculate proportion of light that reaches the given point from the lightsource
float lightmarch(vec3 position) {
    vec3 dirToLight = -directionalLights[0].direction;
    float dstInsideBox = rayBoxDst(boundsMin, boundsMax, position, 1/dirToLight).y;
    //float dstInsideBox = 0.0;
	//vec4 lightInPos = probeLightDepthTex(position);
	////if (lightInPos.w == 0.0) {
	////	dstInsideBox = 0.05;
	////}
	////else {
	////	dstInsideBox = distance(lightInPos.xyz, position);
	////}
	//dstInsideBox = distance(lightInPos.xyz, position);
    
    float stepSize = dstInsideBox / numStepsLight;
    float totalDensity = 0;

    for (int step = 0; step < numStepsLight; step ++) {
        position += dirToLight * stepSize;
        totalDensity += max(0, sampleDensity(position) * stepSize);
    }

    float transmittance = exp(-totalDensity * lightAbsorptionTowardSun);
    return darknessThreshold + transmittance * (1-darknessThreshold);
}


void main()
{	
	vec2 viewerDepthCoord = gl_FragCoord.xy;
	viewerDepthCoord.x = viewerDepthCoord.x / 1920.0;
	viewerDepthCoord.y = viewerDepthCoord.y / 1055.0;

	// Create ray
	vec3 rayPos = ViewerPosition;
	//float viewLength = length(i.viewVector);
	//vec3 rayDir = i.viewVector / viewLength;
	vec4 FratOutPos = texture(viewerBackDepthMap, viewerDepthCoord);
	//if (FratOutPos.w == 0.0) {
	//	discard;
	//}
	vec3 rayDir = normalize(FratOutPos.xyz - FragPos);
	
	// Depth and cloud container intersection info:
	//float nonlin_depth = SAMPLE_DEPTH_TEXTURE(_CameraDepthTexture, i.uv);
	//float depth = LinearEyeDepth(nonlin_depth) * viewLength;
	vec2 rayToContainerInfo = rayBoxDst(boundsMin, boundsMax, rayPos, 1/rayDir);
	float dstToBox = rayToContainerInfo.x;
	float dstInsideBox = rayToContainerInfo.y;
	//float dstToBox = distance(rayPos, FragPos);
	//float dstInsideBox = distance(FratOutPos.xyz, FragPos);
	
	// point of intersection with the cloud container
	vec3 entryPoint = FragPos;
	
	// random starting offset (makes low-res results noisy rather than jagged/glitchy, which is nicer)
	float randomOffset = texture(noiseTexture, squareUV(viewerDepthCoord.xy * 3.0)).r;// BlueNoise.SampleLevel(samplerBlueNoise, squareUV(i.uv*3), 0);
	randomOffset *= rayOffsetStrength;
	
	// Phase function makes clouds brighter around sun
	float cosAngle = dot(rayDir, -directionalLights[0].direction);
	float phaseVal = phase(cosAngle);
	
	float dstTravelled = randomOffset;
	float dstLimit = min(distance(FratOutPos.xyz, FragPos), dstInsideBox);//dstInsideBox;//min(depth - dstToBox, dstInsideBox);
	
	
	
	const float stepSize = 0.01;//11;
	
	// March through volume:
	float transmittance = 1;
	vec3 lightEnergy = vec3(0.0, 0.0, 0.0);
	
	while (dstTravelled < dstLimit) {
	    rayPos = entryPoint + rayDir * dstTravelled;
	    float density = sampleDensity(rayPos);
	    
	    if (density > 0) {
	        float lightTransmittance = lightmarch(rayPos);
	        lightEnergy += density * stepSize * transmittance * lightTransmittance * phaseVal;
	        transmittance *= exp(-density * stepSize * lightAbsorptionThroughCloud);
	    
	        // Exit early if T is close to zero as further samples won't affect the result much
	        if (transmittance < 0.0001) {
	            break;
	        }
	    }
	    dstTravelled += stepSize;
	}

	// Add clouds to background
	vec3 cloudCol = lightEnergy * _LightColor0.xyz;
	//vec3 backgroundCol = vec3(0.0, 0.0, 0.0); //texture(_MainTex, viewerDepthCoord).xyz;
	//vec3 col = backgroundCol * transmittance + cloudCol;	
	//FragColor = vec4(col, 1.0);
	FragColor = vec4(cloudCol, 1.0 - transmittance);
}
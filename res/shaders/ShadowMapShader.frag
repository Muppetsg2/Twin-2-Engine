#version 450
//ShadowMapShader.frag

layout(early_fragment_tests) in;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (location = 0) out vec4 FragColor;


//shadow maps
uniform sampler2D DirLightShadowMaps[4];

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
	vec3 direction;     // Direction of the spot light
	vec3 color;         // Color of the spot light
	float power;		  // Light source power
	//float cutOff;       // Inner cutoff angle (in radians)
	float outerCutOff;  // Outer cutoff angle (in radians)
	float constant;     // Constant attenuation
	float linear;       // Linear attenuation
	float quadratic;    // Quadratic attenuation
};

struct DirectionalLight {
	vec3 position;      // Position of the spot light in world space
	vec3 direction;     // Direction of the spot light
	mat4 lightSpaceMatrix;
	vec3 color;         // Color of the spot light
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

layout(std140, binding = 4) uniform LightingData {
    vec3 AmbientLight;
	vec3 ViewerPosition;
	float highlightParam;
	//float gamma;
};


//LIGHTING END

//zero w cieniu ; 1 - oœwietlone
float ShadowCalculation(vec4 fragPosLightSpace, vec3 N, uint shadowMapId)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(DirLightShadowMaps[shadowMapId], projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
   
    //vec3 lightDir = normalize(directionalLights[shadowMapId].position - position);
    //float bias = max(0.001 * (1.0 - dot(N, lightDir)), 0.0005);
    //float bias = 0.005;
    // check whether current frag pos is in shadow
    //float shadow = (currentDepth) < closestDepth  ? 1.0 : 0.0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 0.25 * 1.0 / textureSize(DirLightShadowMaps[shadowMapId], 0);
    float pcfDepth = 0.0;
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            pcfDepth = texture(DirLightShadowMaps[shadowMapId], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth  < pcfDepth  ? 1.0 : 0.0;        
            //shadow += (currentDepth - bias) < pcfDepth  ? 1.0 : 0.0;
            }    
    }
    shadow /= 25.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 1.0;
        
    return shadow;
}

float countLambertianPart(vec3 L, vec3 N) {
    return max(dot(N, L), 0.0f);
}

float countBlinnPhongPart(vec3 L, vec3 E, vec3 N) {
    vec3 H = normalize(L + E);
    float specAngle = max(dot(H, N), 0.0);
    return pow(specAngle, highlightParam); //<---------
}

void main()
{
    vec3 LightColor = vec3(0.0);
	
	vec3 L = vec3(0.0);
    vec3 N = normalize(normal);
    vec3 E = normalize(ViewerPosition - position);
    
	float lambertian = 0.0;
	float specular = 0.0;
    vec3 SpecCol = vec3(1.0, 1.0, 1.0);

	for (uint i = 0; i < numberOfDirLights; ++i) {
        L = -directionalLights[i].direction;

        lambertian = countLambertianPart(L, N);
		
		specular = 0.0;
        if (lambertian > 0.0) {
            specular = countBlinnPhongPart(L, E, N);
        }

        LightColor += (lambertian + specular) * directionalLights[i].color * directionalLights[i].power * ShadowCalculation(directionalLights[i].lightSpaceMatrix * vec4(position , 1.0), N, i);
        //FragColor = vec4(ShadowCalculation(directionalLights[i].lightSpaceMatrix * vec4(position , 1.0), N, i), 0.0, 0.0, 0.0);
    }
    FragColor = vec4(LightColor + AmbientLight, 1.0);
}
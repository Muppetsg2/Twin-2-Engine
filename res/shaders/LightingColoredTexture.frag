#version 430
//LightingColoredTexture.frag

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) out vec4 clipSpacePos;

layout (location = 4) flat in uint materialIndex;

layout (location = 0) out vec4 FragColor;

// Material Input
struct MaterialInput
{
	vec4 color;
};

//shadow maps
uniform sampler2D DirLightShadowMaps[4];
uniform sampler2D DirLightingMap;
uniform sampler2D occlusionMap;

layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[8];
};

// Texture Input
struct TextureInput
{
	sampler2D texture;
};

layout(location = 0) uniform TextureInput texturesInput[8];



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

layout (std140, binding = 3) buffer Lights {
    PointLight pointLights[8];
    SpotLight spotLights[8];
    DirectionalLight directionalLights[4];
	uint numberOfPointLights;
	uint numberOfSpotLights;
	uint numberOfDirLights;
};

layout(std140, binding = 3) uniform LightingData {
    vec3 AmbientLight;
	float shininness;
	vec3 ViewerPosition;
	int shadingType;
};

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

//LIGHTING END

//zero w cieniu ; 1 - o�wietlone
float ShadowCalculation(vec4 fragPosLightSpace, vec3 N, uint shadowMapId)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(DirLightShadowMaps[shadowMapId], projCoords.xy).r; 
    // calculate bias (based on depth map resolution and slope)
    //vec3 lightDir = normalize(directionalLights[shadowMapId].position - position);
    //float bias = max(0.001 * (1.0 - dot(N, lightDir)), 0.0005);
    //float bias = 0.005;
    // check whether current frag pos is in shadow
    //float shadow = (currentDepth) < closestDepth  ? 1.0 : 0.0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 0.5 * 1.0 / textureSize(DirLightShadowMaps[shadowMapId], 0);
    float pcfDepth = 0.0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            pcfDepth = texture(DirLightShadowMaps[shadowMapId], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth  < pcfDepth  ? 1.0 : 0.0;        
            //shadow += (currentDepth - bias) < pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    //shadow /= 9.0;
    shadow *= 0.11;
    
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
    return pow(specAngle, shininness); //<---------
}

void main()
{
	FragColor = texture(texturesInput[materialIndex].texture, texCoords) * materialInput[materialIndex].color;

    vec3 LightColor = vec3(0.0);
	
	vec3 L = vec3(0.0);
    vec3 N = normalize(normal);
    vec3 E = normalize(ViewerPosition - position);

	float attenuation = 0.0;
	float lambertian = 0.0;
	float specular = 0.0;
	float distance = 0.0;

    for (uint i = 0; i < numberOfPointLights; ++i) {
        L = pointLights[i].position - position;
        distance = length(L);
        L = normalize(L);

        attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);

        lambertian = countLambertianPart(L, N);

		specular = 0.0;
        if (lambertian > 0.0) {
            specular = countBlinnPhongPart(L, E, N);
        }

        LightColor += (lambertian + specular) * pointLights[i].color * pointLights[i].power * attenuation;
		//FragColor = vec4((lambertian + specular) * pointLights[i].color * pointLights[i].power * attenuation, 1.0f);
    }

	{
		float theta = 0;
		float epsilon = 0;
		float intensity = 0;

		for (uint i = 0; i < numberOfSpotLights; ++i) {
			L = spotLights[i].position - position;
			distance = length(L);
			L = normalize(L);

			attenuation = 1.0 / (spotLights[i].constant + spotLights[i].linear * distance + spotLights[i].quadratic * distance * distance);

			lambertian = countLambertianPart(L, N);

			specular = 0.0;
			if (lambertian > 0.0) {
			    specular = countBlinnPhongPart(L, E, N);
			}

		    theta = dot(L, normalize(-spotLights[i].direction));
		    //epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
		    epsilon = -spotLights[i].outerCutOff;
		    intensity = smoothstep(spotLights[i].outerCutOff, 0.0, theta);
			
			LightColor += (lambertian + specular) * spotLights[i].color * spotLights[i].power * attenuation * intensity;
			//FragColor = vec4((lambertian + specular) * spotLights[i].color * spotLights[i].power * attenuation * intensity, 1.0f);
		}
	}

	for (uint i = 0; i < numberOfDirLights; ++i) {
        L = -directionalLights[i].direction;

        lambertian = countLambertianPart(L, N);
		
		//specular = 0.0;
        //if (lambertian > 0.0) {
        //    specular = countBlinnPhongPart(L, E, N);
        //}
    
        //LightColor += lambertian * directionalLights[i].color * directionalLights[i].power;
        //LightColor += (lambertian + specular) * directionalLights[i].color * directionalLights[i].power * ShadowCalculation(directionalLights[i].lightSpaceMatrix * vec4(position , 1.0), N, i);
        LightColor += lambertian * directionalLights[i].color * directionalLights[i].power * ShadowCalculation(directionalLights[i].lightSpaceMatrix * vec4(position , 1.0), N, i);
    }
	
    vec2 NDCSpaceFragPos = clipSpacePos.xy / clipSpacePos.w;
    vec2 textureLookupPos = NDCSpaceFragPos * 0.5 + 0.5;
    float visibility_factor = isSSAO ? texture(occlusionMap, textureLookupPos).r : 1.0;

    FragColor *= vec4(LightColor + AmbientLight * visibility_factor, 1.0); //
	FragColor = vec4(pow(FragColor.rgb, vec3(gamma)), 1.0);
}
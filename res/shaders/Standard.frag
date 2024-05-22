#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

layout (location = 3) flat in uint materialIndex;

layout (location = 0) out vec4 FragColor;

// Material Input

struct MaterialInput
{
   vec4 ambientColor;
   vec4 diffuseColor;
   vec4 specularColor;
   vec4 emissiveColor;
   float shininess;
   uint ambientTexturesCount;
   uint diffuseTexturesCount;
   uint specularTexturesCount;
   uint normalsTexturesCount;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[8];
};

//shadow maps
uniform sampler2D DirLightShadowMaps[4];

layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
    float time;
    float deltaTime;
};


// Texture Input
//struct TextureInput
//{
//	sampler2D texture;
//};
//layout(location = 0) uniform TextureInput texturesInput[8];
layout(location = 0) uniform sampler2D texturesInput[8];

sampler2D GetAmbientMap(uint index)
{
	return textureInput[materialInput[materialIndex] + index];
}
sampler2D GetDiffuseMap(uint index)
{
	return textureInput[materialInput[materialIndex].ambientTexturesCount + index];
}
sampler2D GetSpecularMap(uint index)
{
	return textureInput[materialInput[materialIndex].ambientTexturesCount + materialInput[materialIndex].diffuseTexturesCount  + index];
}
sampler2D GetNormalMap(uint index)
{
	return textureInput[materialInput[materialIndex].ambientTexturesCount + materialInput[materialIndex].diffuseTexturesCount + materialInput[materialIndex].specularTexturesCount + index];
}



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

layout(std140, binding = 3) uniform LightingData {
    vec3 AmbientLight;
	float shininness;
	vec3 ViewerPosition;
	int shadingType;
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
	vec3 LightColor = vec3(0.0);
	
	vec3 L = vec3(0.0);
    vec3 N = normalize(normal);
    vec3 E = normalize(ViewerPosition - position);

	float attenuation = 0.0;
	float lambertian = 0.0;
	float specular = 0.0;
	float distance = 0.0;

    for (uint index = 0; index < materialInput[materialIndex].ambientTexturesCount; index++)
    {
        LightColor += (texture(GetAmbientMap(index), texCoords)).xyz;
    }
    for (uint index = 0; index < materialInput[materialIndex].diffuseTexturesCount; index++)
    {
        LightColor += (texture(GetDiffusetMap(index), texCoords)).xyz;
    }
    for (uint index = 0; index < materialInput[materialIndex].specularTexturesCount; index++)
    {
        LightColor += (texture(GetSpeculartMap(index), texCoords)).xyz;
    }

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

        LightColor += (lambertian * materialInput[materilIndex].diffuseColor + specular * materialInput[materilIndex].specularColor) * pointLights[i].color * pointLights[i].power * attenuation;
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
			
			LightColor += (lambertian * materialInput[materilIndex].diffuseColor + specular * materialInput[materilIndex].specularColor) * spotLights[i].color * spotLights[i].power * attenuation * intensity;
			//FragColor = vec4((lambertian + specular) * spotLights[i].color * spotLights[i].power * attenuation * intensity, 1.0f);
		}
	}

	for (uint i = 0; i < numberOfDirLights; ++i) {
        L = -directionalLights[i].direction;

        lambertian = countLambertianPart(L, N);
		
		specular = 0.0;
        if (lambertian > 0.0) {
            specular = countBlinnPhongPart(L, E, N);
        }

        //LightColor += (lambertian * materialInput[materilIndex].diffuseColor + specular * materialInput[materilIndex].specularColor) * directionalLights[i].color * directionalLights[i].power;
        LightColor += (lambertian + specular) * directionalLights[i].color * directionalLights[i].power * ShadowCalculation(directionalLights[i].lightSpaceMatrix * vec4(position , 1.0), N, i);
    }
	
    FragColor *= vec4(LightColor + AmbientLight + materialInput[materilIndex].ambientColor.xyz, 1.0); //
	FragColor = vec4(pow(FragColor.rgb, vec3(2.2)), 1.0);
}
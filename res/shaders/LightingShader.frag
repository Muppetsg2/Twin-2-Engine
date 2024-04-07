#version 430

in vec3 position;
in vec3 normal;
in vec2 texCoords;

in vec4 color1;
in vec4 color2;

flat in uint materialIndex;

out vec4 FragColor;

//shadow maps
layout (location = 0) uniform sampler2D DirLightShadowMaps[4];

uniform vec4 uColor;
uniform bool uNoTexture = true;
uniform sampler2D texture_diffuse1;

struct MaterialInput
{
   vec4 color1;
   vec4 color2;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[3];
};

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
	vec3 color;         // Color of the spot light
	float power;		  // Light source power
	mat4 lightSpaceMatrix;
};

layout (std430, binding = 3) buffer Lights {
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
	float gamma;
};


//LIGHTING END

float ShadowCalculation(vec4 fragPosLightSpace, uint shadowMapId)
{
    // Perform shadow calculation, using shadowMap and fragPosLightSpace
    // This is a basic example, adjust for PCF or other techniques as needed
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(DirLightShadowMaps[shadowMapId], projCoords.xy).r; 
    float currentDepth = projCoords.z;
    return (currentDepth - 0.005) > closestDepth  ? 1.0 : 0.0;
    
	//float currentDepth = 0.0;
	//float shadow = 0.0;
	//vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	//for(int x = -1; x <= 1; ++x)
	//{
	//    for(int y = -1; y <= 1; ++y)
	//    {
	//        float pcfDepth = texture(DirLightShadowMaps[shadowMapId], projCoords.xy + vec2(x, y) * texelSize).r; 
	//        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	//    }    
	//}
	//shadow /= 9.0;
}

float countLambertianPart(vec3 L, vec3 N) {
    return max(dot(N, L), 0.0f);
}

float countBlinnPhongPart(vec3 L, vec3 E, vec3 N) {
    vec3 H = normalize(L + E);
    float specAngle = max(dot(H, N), 0.0);
    return pow(specAngle, 2); //<---------
}

void main()
{
	//FragColor = uColor;
	//FragColor = color1 + color2;
	FragColor = materialInput[materialIndex].color1 + materialInput[materialIndex].color2;

	//vec3 result = vec3(0.0);
	//vec3 lightDir;
	//vec3 diffuse;
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
		
		specular = 0.0;
        if (lambertian > 0.0) {
            specular = countBlinnPhongPart(L, E, N);
        }

        LightColor += (lambertian + specular) * directionalLights[i].color * directionalLights[i].power;
        //LightColor += (lambertian + specular) * directionalLights[i].color * directionalLights[i].power * ShadowCalculation(directionalLights[i].lightSpaceMatrix * vec4(position , 1.0), i);
    }
	
    FragColor *= vec4(LightColor + AmbientLight, 1.0f); //
	FragColor = vec4(pow(FragColor.rgb, vec3(1.0 / gamma)), FragColor.a);
}
#version 430

in vec3 position;
in vec3 normal;
in vec2 texCoords;

in vec4 color1;
in vec4 color2;

flat in uint materialIndex;

out vec4 FragColor;

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
	float cutOff;       // Inner cutoff angle (in radians)
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
};

layout (std430, binding = 3) buffer Lights {
	uint numberOfPointLights;
	uint numberOfSpotLights;
	uint numberOfDirLights;
    PointLight pointLights[8];
    SpotLight spotLights[8];
    DirectionalLight directionalLights[4];
} lights;

layout(std140, binding = 4) uniform LightingData {
    vec3 AmbientLight;
	vec3 ViewerPosition;
	float gamma;
} lightingData;

//LIGHTING END

float countLambertianPart(vec3 L, vec3 N) {
    return max(dot(N, L), 0.0f);
}

float countBlinnPhongPart(vec3 L, vec3 E, vec3 N) {
    vec3 H = normalize(L + E);
    float specAngle = max(dot(H, N), 0.0);
    return pow(specAngle, 4); //<---------
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
    vec3 E = normalize(lightingData.ViewerPosition - position);

	float attenuation = 0.0;
	float lambertian = 0.0;
	float specular = 0.0;
	float distance = 0.0;

    for (int i = 0; i < lights.numberOfPointLights; ++i) {
        L = lights.pointLights[i].position - position;
        distance = length(L);
        L = normalize(L);

        attenuation = 1.0 / (lights.pointLights[i].constant + lights.pointLights[i].linear * distance + lights.pointLights[i].quadratic * distance * distance);

        lambertian = countLambertianPart(L, N);

        if (lambertian > 0.0) {
            specular = countBlinnPhongPart(L, E, N);
        }

        LightColor += (lambertian + specular) * lights.pointLights[i].color * lights.pointLights[i].power * attenuation;
    }

	{
		float theta = 0;
		float epsilon = 0;
		float intensity = 0;

		for (int i = 0; i < lights.numberOfSpotLights; ++i) {
			L = lights.spotLights[i].position - position;
			distance = length(L);
			L = normalize(L);

			attenuation = 1.0 / (lights.spotLights[i].constant + lights.spotLights[i].linear * distance + lights.spotLights[i].quadratic * distance * distance);

			lambertian = countLambertianPart(L, N);

			if (lambertian > 0.0) {
			    specular = countBlinnPhongPart(L, E, N);
			}

		    theta = dot(L, normalize(-lights.spotLights[i].direction));
		    epsilon = lights.spotLights[i].cutOff - lights.spotLights[i].outerCutOff;
		    intensity = smoothstep(lights.spotLights[i].outerCutOff, lights.spotLights[i].cutOff, theta);
			
			LightColor += (lambertian + specular) * lights.spotLights[i].color * lights.spotLights[i].power * attenuation * intensity;
		}
	}

	for (int i = 0; i < lights.numberOfDirLights; ++i) {
        L = -lights.directionalLights[i].direction;

        lambertian = countLambertianPart(L, N);

        if (lambertian > 0.0) {
            specular = countBlinnPhongPart(L, E, N);
        }

        LightColor += (lambertian + specular) * lights.directionalLights[i].color * lights.directionalLights[i].power;
    }
	
    FragColor *= vec4(vec3(LightColor + lightingData.AmbientLight), 1.0f);
	FragColor = vec4(pow(FragColor.rgb, vec3(1.0 / lightingData.gamma)), FragColor.a);
}
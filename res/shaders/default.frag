#version 450 core
// IN
layout (std140, binding = 1) uniform WindowData {
    vec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

in VS_OUT {
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
    flat uint materialIndex; // flat = nie interpolowane
} fs_in;

struct MaterialInput {
    vec3 color;
    float shininess;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInputs[8];
};

struct TextureInput {
    sampler2D diffuse_texture;
    sampler2D specular_texture;
};

layout(location = 0) uniform TextureInput texturesInput[8];

// LIGHTS
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 4

struct PointLight {
    vec3 position;      // Position of the point light in world space
    vec3 color;         // Color of the point light
    float power;        // Light source power
    float constant;     // Constant attenuation
    float linear;       // Linear attenuation
    float quadratic;    // Quadratic attenuation
};

struct SpotLight {
    vec3 position;      // Position of the spot light in world space
    vec3 direction;     // Direction of the spot light
    vec3 color;         // Color of the spot light
    float power;        // Light source power
    float innerCutOff;  // Inner cutoff angle (in radians)
    float outerCutOff;  // Outer cutoff angle (in radians)
    float constant;     // Constant attenuation
    float linear;       // Linear attenuation
    float quadratic;    // Quadratic attenuation
};

struct DirectionalLight {
    vec3 position;          // Position of the spot light in world space
	vec3 direction;         // Direction of the spot light
    mat4 lightSpaceMatrix;  // For Shadow Mapping
	vec3 color;             // Color of the spot light
	float power;		    // Light source power
};

uniform sampler2D DirLightShadowMaps[MAX_DIRECTIONAL_LIGHTS]; // SHADOW MAP

layout(std430, binding = 3) buffer Lights {
    uint numberOfPointLights;
    uint numberOfSpotLights;
    uint numberOfDirLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

layout(std140, binding = 4) uniform LightingData {
    vec3 ambientLight;
    vec3 viewerPosition;
    int shadingType;
};

// OUT
out vec4 Color;

// CONSTS
const float ambientPowerPercent = 0.2;
const float diffusePowerPercent = 0.75;
const float specularPowerPercent = 1.0;

const uint diffuseToonBorders = 3;
const float invDiffuseToonBorders = 1.0 / diffuseToonBorders;
const uint specularToonBorders = 1;
const float invSpecularToonBorders = 1.0 / specularToonBorders;

// SHADOW MAP CALCULATIONS
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, uint shadowMapId) {

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform from [-1, 1] to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(DirLightShadowMaps[shadowMapId], projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 lightDir = normalize(directionalLights[shadowMapId].position - fs_in.fragPos);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 0.5 * 1.0 / textureSize(DirLightShadowMaps[shadowMapId], 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(DirLightShadowMaps[shadowMapId], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth  < pcfDepth  ? 1.0 : 0.0;
        }    
    }
    shadow /= 25.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 1.0;
        
    return shadow;
}

// LIGHT VARIABLES
float CalculateAttenuation(float constant, float linear, float quadratic, float dist) {
    return 1.0 / (constant + linear * dist + quadratic * dist * dist);
}

float CalculateLambertian(vec3 lightDir, vec3 normal) {
    return max(dot(normal, lightDir), 0.0);
}

float CalculateBlinnPhong(vec3 lightDir, vec3 viewDir, vec3 normal, float shininess) {
    return pow(max(dot(normal, normalize(lightDir + viewDir)), 0.0), shininess);
}

float CalculatePhong(vec3 lightDir, vec3 viewDir, vec3 normal, float shininess) {
    return pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), shininess);
}

float CalculateDiffuseToon(float value) {
    return invDiffuseToonBorders * floor(value * (diffuseToonBorders + 1.0));
}

float CalculateSpecularToon(float value) {
    return invSpecularToonBorders * floor(value * (specularToonBorders + 1.0));
}

// LIGHT TYPES
vec4 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDir, vec4 mat_diffuse, vec4 mat_specular, float mat_shininess) {
    vec3 lightDir = light.position - fs_in.fragPos;
    float attenuation = CalculateAttenuation(light.constant, light.linear, light.quadratic, length(lightDir));
    lightDir = normalize(lightDir);

    float diff = CalculateLambertian(lightDir, normal);
    float spec = CalculateBlinnPhong(lightDir, viewDir, normal, mat_shininess);

    // TOON SHADING
    if (shadingType == 1) {
        diff = CalculateDiffuseToon(diff);
        spec = CalculateSpecularToon(spec);
    }

    vec4 lightColor = vec4(light.color, 1.0);
    vec4 ambient = ambientPowerPercent * attenuation * light.power * lightColor * mat_diffuse;
    vec4 diffuse = diffusePowerPercent * attenuation * diff * light.power * lightColor * mat_diffuse;
    vec4 specular = specularPowerPercent * attenuation * spec * light.power * lightColor * mat_specular;

    return ambient + diffuse + specular;
}

vec4 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec4 mat_diffuse, vec4 mat_specular, float mat_shininess) {    
    vec3 lightDir = light.position - fs_in.fragPos;
    float attenuation = CalculateAttenuation(light.constant, light.linear, light.quadratic, length(lightDir));
    lightDir = normalize(lightDir);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;

    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    float diff = CalculateLambertian(lightDir, normal);
    float spec = CalculateBlinnPhong(lightDir, viewDir, normal, mat_shininess);

    // TOON SHADING
    if (shadingType == 1) {
        diff = CalculateDiffuseToon(diff);
        spec = CalculateSpecularToon(spec);
    }

    vec4 lightColor = vec4(light.color, 1.0);
    vec4 ambient = ambientPowerPercent * attenuation * intensity * light.power * lightColor * mat_diffuse;
    vec4 diffuse = diffusePowerPercent * attenuation * intensity * diff * light.power * lightColor * mat_diffuse;
    vec4 specular = specularPowerPercent * attenuation * intensity * spec * light.power * lightColor * mat_specular;

    return ambient + diffuse + specular;
}

vec4 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, uint shadowMapId, vec4 mat_diffuse, vec4 mat_specular, float mat_shininess) {
    vec3 lightDir = normalize(-light.direction);

    float intensity = 1.0 /*CalculateShadow(light.lightSpaceMatrix * vec4(position, 1.0), normal, shadowMapId)*/;
    float diff = CalculateLambertian(lightDir, normal);
    float spec = CalculateBlinnPhong(lightDir, viewDir, normal, mat_shininess);

    // TOON SHADING
    if (shadingType == 1) {
        diff = CalculateDiffuseToon(diff);
        spec = CalculateSpecularToon(spec);
    }

    vec4 lightColor = vec4(light.color, 1.0);
    vec4 ambient = ambientPowerPercent * intensity * light.power * lightColor * mat_diffuse;
    vec4 diffuse = diffusePowerPercent * intensity * diff * light.power * lightColor * mat_diffuse;
    vec4 specular = specularPowerPercent * intensity * spec * light.power * lightColor * mat_specular;

    return ambient + diffuse + specular;
}

void main()
{
    MaterialInput mat = materialInputs[fs_in.materialIndex];
    
    vec4 mat_diffuse = /*vec4(mat.color, 1.0) **/ texture(texturesInput[fs_in.materialIndex].diffuse_texture, fs_in.texCoord);
    vec4 mat_specular = /*vec4(mat.color, 1.0) **/ texture(texturesInput[fs_in.materialIndex].specular_texture, fs_in.texCoord);

    vec4 result = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 normal = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewerPosition - fs_in.fragPos);

    // POINT LIGHTS
    for (uint i = 0; i < numberOfPointLights && i < MAX_POINT_LIGHTS; ++i) {
        result += CalculatePointLight(pointLights[i], normal, viewDir, mat_diffuse, mat_specular, mat.shininess);
    }

    // SPOT LIGHTS
    for (uint i = 0; i < numberOfSpotLights && i < MAX_SPOT_LIGHTS; ++i) {
        result += CalculateSpotLight(spotLights[i], normal, viewDir, mat_diffuse, mat_specular, mat.shininess);
    }

    // DIRECTIONAL LIGHTS
    for (uint i = 0; i < numberOfDirLights && i < MAX_DIRECTIONAL_LIGHTS; ++i) {
        result += CalculateDirectionalLight(directionalLights[i], normal, viewDir, i, mat_diffuse, mat_specular, mat.shininess);
    }

    // AMBIENT LIGHT
    result += vec4(ambientLight, 0.0);

    Color = vec4(pow(result.rgb, vec3(gamma)), result.a);
}
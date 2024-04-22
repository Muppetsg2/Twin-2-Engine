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
    bool has_diffuse_texture;
    bool has_specular_texture;
    vec3 color;
    float shininess;

    // TOON SHADING PARAMETERS
    uint diffuseToonBorders;
    uint specularToonBorders;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInputs[8];
};

struct TextureInput {
    sampler2D diffuse_texture;
    sampler2D specular_texture;
};

layout(location = 0) uniform TextureInput texturesInput[8];

// STRUCTS
struct FragmentData {
    // VECTORS
    vec3 normal;
    vec3 viewDir;

    // MATERIAL
    vec4 mat_diffuse;
    vec4 mat_specular;
    float mat_shininess;

    // TOON BORDERS
    uint diffuseToonBorders;
    float invDiffuseToonBorders;
    uint specularToonBorders;
    float invSpecularToonBorders;
} data;

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

// GOOCH CONSTS
const vec3 coolColor = vec3(0.0, 0.0, 0.4);
const float coolFactor = 0.25;
const vec3 warmColor = vec3(0.4, 0.4, 0.0);
const float warmFactor = 0.25;

// GAMMA CALCULATION
vec4 CalculateGamma(vec4 color) {
    return vec4(pow(color.rgb, vec3(gamma)), color.a);
}

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

float CalculateToon(float value, float toonBorders, float invToonBorders) {
    return invToonBorders * floor(value * (toonBorders + 1.0));
}

// LIGHT TYPES
vec4 CalculatePointLight(PointLight light) {
    vec3 lightDir = light.position - fs_in.fragPos;
    float attenuation = CalculateAttenuation(light.constant, light.linear, light.quadratic, length(lightDir));
    lightDir = normalize(lightDir);

    float diff = CalculateLambertian(lightDir, data.normal);
    float spec = CalculateBlinnPhong(lightDir, data.viewDir, data.normal, data.mat_shininess);

    // TOON SHADING
    if (shadingType == 1) {
        diff = CalculateToon(diff, data.diffuseToonBorders, data.invDiffuseToonBorders);
        spec = CalculateToon(spec, data.specularToonBorders, data.invSpecularToonBorders);
    }

    vec4 lightColor = CalculateGamma(vec4(light.color, 1.0));
    vec4 ambient = ambientPowerPercent * attenuation * light.power * lightColor * data.mat_diffuse;
    vec4 diffuse = diffusePowerPercent * attenuation * diff * light.power * lightColor * data.mat_diffuse;
    vec4 specular = specularPowerPercent * attenuation * spec * light.power * lightColor * data.mat_specular;

    return ambient + diffuse + specular;
}

vec4 CalculateSpotLight(SpotLight light) {    
    vec3 lightDir = light.position - fs_in.fragPos;
    float attenuation = CalculateAttenuation(light.constant, light.linear, light.quadratic, length(lightDir));
    lightDir = normalize(lightDir);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;

    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    float diff = CalculateLambertian(lightDir, data.normal);
    float spec = CalculateBlinnPhong(lightDir, data.viewDir, data.normal, data.mat_shininess);

    // TOON SHADING
    if (shadingType == 1) {
        diff = CalculateToon(diff, data.diffuseToonBorders, data.invDiffuseToonBorders);
        spec = CalculateToon(spec, data.specularToonBorders, data.invSpecularToonBorders);
    }

    vec4 lightColor = CalculateGamma(vec4(light.color, 1.0));
    vec4 ambient = ambientPowerPercent * attenuation * intensity * light.power * lightColor * data.mat_diffuse;
    vec4 diffuse = diffusePowerPercent * attenuation * intensity * diff * light.power * lightColor * data.mat_diffuse;
    vec4 specular = specularPowerPercent * attenuation * intensity * spec * light.power * lightColor * data.mat_specular;

    return ambient + diffuse + specular;
}

vec4 CalculateDirectionalLight(DirectionalLight light, uint shadowMapId) {
    vec3 lightDir = normalize(-light.direction);

    float intensity = 1.0 /*CalculateShadow(light.lightSpaceMatrix * vec4(position, 1.0), normal, shadowMapId)*/;
    float diff = CalculateLambertian(lightDir, data.normal);
    float spec = CalculateBlinnPhong(lightDir, data.viewDir, data.normal, data.mat_shininess);

    // TOON SHADING
    if (shadingType == 1) {
        diff = CalculateToon(diff, data.diffuseToonBorders, data.invDiffuseToonBorders);
        spec = CalculateToon(spec, data.specularToonBorders, data.invSpecularToonBorders);
    }
    // GOOCH SHADING
    else if (shadingType == 2) {
        diff = (diff + 1.0) * 0.5;

        vec4 kCool = vec4(coolColor, 0.0) + coolFactor * data.mat_diffuse;
        vec4 kWarm = vec4(warmColor, 0.0) + warmFactor * data.mat_diffuse;

        vec4 gooch = mix(kWarm, kCool, diff);
        vec4 lightColor = CalculateGamma(vec4(light.color, 1.0));
        vec4 specular = specularPowerPercent * intensity * spec * light.power * lightColor * data.mat_specular;
        return gooch + specular;
    }

    vec4 lightColor = CalculateGamma(vec4(light.color, 1.0));
    vec4 ambient = ambientPowerPercent * intensity * light.power * lightColor * data.mat_diffuse;
    vec4 diffuse = diffusePowerPercent * intensity * diff * light.power * lightColor * data.mat_diffuse;
    vec4 specular = specularPowerPercent * intensity * spec * light.power * lightColor * data.mat_specular;

    return ambient + diffuse + specular;
}

void main()
{
    // VECTORS
    data.normal = normalize(fs_in.normal);
    data.viewDir = normalize(viewerPosition - fs_in.fragPos);

    // CURRENT MATERIAL
    MaterialInput mat = materialInputs[fs_in.materialIndex];
    
    // DIFFUSE
    data.mat_diffuse = CalculateGamma(vec4(mat.color, 1.0));
    if (mat.has_diffuse_texture) data.mat_diffuse *= texture(texturesInput[fs_in.materialIndex].diffuse_texture, fs_in.texCoord);

    // SPECULAR
    data.mat_specular = CalculateGamma(vec4(1.0));
    if (mat.has_specular_texture) data.mat_specular *= texture(texturesInput[fs_in.materialIndex].specular_texture, fs_in.texCoord);

    // SHININESS
    data.mat_shininess = mat.shininess;

    // TOON BORDERS
    if (shadingType == 1) {
        data.diffuseToonBorders = mat.diffuseToonBorders;
        data.invDiffuseToonBorders = 1.0 / data.diffuseToonBorders;

        data.specularToonBorders = mat.specularToonBorders;
        data.invSpecularToonBorders = 1.0 / data.specularToonBorders;
    }

    Color = vec4(0.0);

    // POINT LIGHTS
    for (uint i = 0; i < numberOfPointLights && i < MAX_POINT_LIGHTS; ++i) {
        Color += CalculatePointLight(pointLights[i]);
    }

    // SPOT LIGHTS
    for (uint i = 0; i < numberOfSpotLights && i < MAX_SPOT_LIGHTS; ++i) {
        Color += CalculateSpotLight(spotLights[i]);
    }

    // DIRECTIONAL LIGHTS
    for (uint i = 0; i < numberOfDirLights && i < MAX_DIRECTIONAL_LIGHTS; ++i) {
        Color += CalculateDirectionalLight(directionalLights[i], i);
    }

    // AMBIENT LIGHT
    Color += CalculateGamma(vec4(ambientLight, 1.0)) * data.mat_diffuse;
}
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
    vec4 color;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInput[8];
};

struct TextureInput {
    sampler2D tex;
};

layout(location = 0) uniform TextureInput texturesInput[8];

// SHADOW MAP
uniform sampler2D DirLightShadowMaps[4];

// LIGHTS
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
    //float innerCutOff;  // Inner cutoff angle (in radians)
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

layout(std430, binding = 3) buffer Lights {
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
};

// OUT
out vec4 Color;

// UNIFORMS
uniform bool uNoTexture = true;
uniform bool toon = true;

// CONSTS
const uint toonBorders = 3;
const float invToonBorders = 1.0 / toonBorders;

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

float CalculateLambertian(vec3 LightDir, vec3 Normal) {
    return max(dot(Normal, LightDir), 0.0);
}

float CalculateBlinnPhong(vec3 LightDir, vec3 ViewDir, vec3 Normal) {
    vec3 HalfDir = normalize(LightDir + ViewDir);
    return pow(max(dot(Normal, HalfDir), 0.0), highlightParam);
}

float CalculatePhong(vec3 LightDir, vec3 ViewDir, vec3 Normal) {
    vec3 ReflectDir = reflect(-LightDir, Normal);
    return pow(max(dot(ViewDir, ReflectDir), 0.0), highlightParam);
}

float CalculateToon(float value) {
    return invToonBorders * floor(value * (toonBorders + 1.0));
}

// LIGHT TYPES
vec3 CalculatePointLight(PointLight light, vec3 position, vec3 normal, vec3 viewDir) {
    vec3 lightDir = light.position - position;
    float attenuation = CalculateAttenuation(light.constant, light.linear, light.quadratic, length(lightDir));
    lightDir = normalize(lightDir);

    float diffuse = CalculateLambertian(lightDir, normal);
    if (!toon) {
        diffuse += CalculateBlinnPhong(lightDir, viewDir, normal);
    }
    else {
        diffuse = CalculateToon(diffuse);
    }

    return diffuse * light.color * light.power * attenuation;
}

vec3 CalculateSpotLight(SpotLight light, vec3 position, vec3 normal, vec3 viewDir) {
    vec3 lightDir = light.position - position;
    float attenuation = CalculateAttenuation(light.constant, light.linear, light.quadratic, length(lightDir));
    lightDir = normalize(lightDir);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = /*light.innerCutOff*/ - light.outerCutOff;

    float intensity = smoothstep(light.outerCutOff, /*light.innerCutOff*/0.0, theta);
    float diffuse = CalculateLambertian(lightDir, normal);
    if (!toon) {
        diffuse += CalculateBlinnPhong(lightDir, viewDir, normal);
    }
    else {
        diffuse = CalculateToon(diffuse);
    }

    return diffuse * intensity * light.color * light.power * attenuation;
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 position, vec3 normal, vec3 viewDir, uint shadowMapId) {
        vec3 lightDir = -light.direction;

        float intensity = 1.0 /*CalculateShadow(light.lightSpaceMatrix * vec4(position, 1.0), normal, shadowMapId)*/;
        float diffuse = CalculateLambertian(lightDir, normal);
        if (!toon) {
            diffuse += CalculateBlinnPhong(lightDir, viewDir, normal);
        }
        else {
            diffuse = CalculateToon(diffuse);
        }

        return diffuse * intensity * light.color * light.power;
}

void main() 
{
    Color = materialInput[fs_in.materialIndex].color * texture(texturesInput[fs_in.materialIndex].tex, fs_in.texCoord);

    vec3 LightColor = vec3(0.0);

    vec3 normal = normalize(fs_in.normal);
    vec3 viewDir = normalize(ViewerPosition - fs_in.fragPos);

    // POINT LIGHTS
    for (uint i = 0; i < numberOfPointLights; ++i) {
        LightColor += CalculatePointLight(pointLights[i], fs_in.fragPos, normal, viewDir);
    }

    // SPOT LIGHTS
    for (uint i = 0; i < numberOfSpotLights; ++i) {
        LightColor += CalculateSpotLight(spotLights[i], fs_in.fragPos, normal, viewDir);
    }

    // DIRECTIONAL LIGHTS
    for (uint i = 0; i < numberOfDirLights; ++i) {
        LightColor += CalculateDirectionalLight(directionalLights[i], fs_in.fragPos, normal, viewDir, i);
    }

    // AMBIENT LIGHT
    LightColor += AmbientLight;

    Color *= vec4(LightColor, 1.0);
    Color = vec4(pow(Color.rgb, vec3(gamma)), 1.0);
}
#version 450 core
// IN
layout (std140, binding = 1) uniform WindowData
{
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
    float innerCutOff;  // Inner cutoff angle (in radians)
    float outerCutOff;  // Outer cutoff angle (in radians)
    float constant;     // Constant attenuation
    float linear;       // Linear attenuation
    float quadratic;    // Quadratic attenuation
};

struct DirectionalLight {
    vec3 position;          // Position of the spot light in world space
	vec3 direction;         // Direction of the spot light
	vec3 color;             // Color of the spot light
	float power;		    // Light source power

    mat4 lightSpaceMatrix;  // For Shadow Mapping
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
uniform vec4 uColor;
uniform bool uNoTexture = true;
uniform sampler2D texture_diffuse1;

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

// ADDITIONAL LIGHT FUNCTIONS
float CalculateAttenuation(float constant, float linear, float quadratic, float dist) {
    return 1.0 / (constant + linear * dist + quadratic * dist * dist);
}

float CalculateLambertian(vec3 LightDir, vec3 Normal) {
    return max(dot(Normal, LightDir), 0.0);
}

float CalculateBlinnPhong(vec3 LightDir, vec3 ViewerDir, vec3 Normal) {
    vec3 halfDir = normalize(LightDir + ViewerDir);
    return pow(max(dot(halfDir, Normal), 0.0), highlightParam);
}

void main() 
{
    MaterialInput material = materialInput[fs_in.materialIndex];
    Color = texture(texturesInput[fs_in.materialIndex].tex, fs_in.texCoord) * material.color;

    vec3 LightColor = vec3(0.0);

    vec3 LightDir = vec3(0.0);
    vec3 Normal = normalize(fs_in.normal);
    vec3 ViewerDir = normalize(ViewerPosition - fs_in.fragPos);

    float atten = 0.0;  // atenuation
    float lamb = 0.0;   // lambertian
    float spec = 0.0;   // specular
    float dist = 0.0;   // distance

    // POINT LIGHTS
    for (uint i = 0; i < numberOfPointLights; ++i) {
        PointLight pointLight = pointLights[i];

        LightDir = pointLight.position - fs_in.fragPos;
        dist = length(LightDir);
        LightDir = normalize(LightDir);

        atten = CalculateAttenuation(pointLight.constant, pointLight.linear, pointLight.quadratic, dist);

        lamb = CalculateLambertian(LightDir, Normal);

        spec = 0.0;
        if (lamb > 0.0) {
            spec = CalculateBlinnPhong(LightDir, ViewerDir, Normal);
        }

        LightColor += (lamb + spec) * pointLight.color * pointLight.power * atten;
    }

    // SPOT LIGHTS
    float theta = 0.0;
    float epsilon = 0.0;
    float intensity = 0.0;

    for (uint i = 0; i < numberOfSpotLights; ++i) {
        SpotLight spotLight = spotLights[i];

        LightDir = spotLight.position - fs_in.fragPos;
        dist = length(LightDir);
        LightDir = normalize(LightDir);

        atten = CalculateAttenuation(spotLight.constant, spotLight.linear, spotLight.quadratic, dist);

        lamb = CalculateLambertian(LightDir, Normal);

        spec = 0.0;
        if (lamb > 0.0) {
            spec = CalculateBlinnPhong(LightDir, ViewerDir, Normal);
        }

        theta = dot(LightDir, normalize(-spotLight.direction));
        epsilon = spotLight.innerCutOff - spotLight.outerCutOff;
        intensity = smoothstep(spotLight.outerCutOff, spotLight.innerCutOff, theta);

        LightColor += (lamb + spec) * spotLight.color * spotLight.power * atten * intensity;
    }

    for (uint i = 0; i < numberOfDirLights; ++i) {
        DirectionalLight directionalLight = directionalLights[i];
        LightDir = -directionalLight.direction;

        lamb = CalculateLambertian(LightDir, Normal);

        spec = 0.0;
        if (lamb > 0.0) {
            spec = CalculateBlinnPhong(LightDir, ViewerDir, Normal);
        }

        LightColor += (lamb + spec) * directionalLight.color * directionalLight.power /** CalculateShadow(directionalLight.lightSpaceMatrix * vec4(fs_in.fragPos, 1.0), Normal, i)*/;
    }

    Color *= vec4(LightColor + AmbientLight, 1.0);
    Color = vec4(pow(Color.rgb, vec3(gamma)), 1.0);
}
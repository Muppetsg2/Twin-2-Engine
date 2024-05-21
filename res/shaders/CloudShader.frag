#version 430 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler3D noiseTexture;
//uniform vec3 lightDir;
//uniform vec3 cameraPos;
//uniform float stepSize;
//uniform float densityFactor;
float stepSize = 0.1;
float densityFactor = 0.7;


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

//LIGHTING END





// Helper functions for Perlin noise

vec3 permute(vec3 x) {
    return mod(((x * 34.0) + 1.0) * x, 289.0);
}

float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

vec4 fade(vec4 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float grad(int hash, vec3 p) {
    int h = hash & 15;
    float u = h < 8 ? p.x : p.y;
    float v = h < 4 ? p.y : (h == 12 || h == 14 ? p.x : p.z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Perlin noise function for 3D
float perlinNoise(vec3 p) {
    vec3 Pi0 = floor(p); // Integer part for indexing
    vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
    Pi0 = mod(Pi0, 289.0);
    Pi1 = mod(Pi1, 289.0);
    vec3 Pf0 = fract(p); // Fractional part for interpolation
    vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
    vec3 fade_pf0 = fade(Pf0);

    // Hash coordinates of the 8 cube corners
    int ix0 = int(Pi0.x);
    int iy0 = int(Pi0.y);
    int iz0 = int(Pi0.z);
    int ix1 = int(Pi1.x);
    int iy1 = int(Pi1.y);
    int iz1 = int(Pi1.z);

    float n000 = grad(hash(ix0, iy0, iz0), Pf0);
    float n100 = grad(hash(ix1, iy0, iz0), vec3(Pf1.x, Pf0.y, Pf0.z));
    float n010 = grad(hash(ix0, iy1, iz0), vec3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = grad(hash(ix1, iy1, iz0), vec3(Pf1.x, Pf1.y, Pf0.z));
    float n001 = grad(hash(ix0, iy0, iz1), vec3(Pf0.x, Pf0.y, Pf1.z));
    float n101 = grad(hash(ix1, iy0, iz1), vec3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = grad(hash(ix0, iy1, iz1), vec3(Pf0.x, Pf1.y, Pf1.z));
    float n111 = grad(hash(ix1, iy1, iz1), vec3(Pf1.x, Pf1.y, Pf1.z));

    // Blend contributions from 8 corners of the cube
    vec4 n_xyz = mix(mix(mix(vec4(n000), vec4(n100), fade_pf0.x),
                         mix(vec4(n010), vec4(n110), fade_pf0.x), fade_pf0.y),
                     mix(mix(vec4(n001), vec4(n101), fade_pf0.x),
                         mix(vec4(n011), vec4(n111), fade_pf0.x), fade_pf0.y), fade_pf0.z);

    return 2.2 * n_xyz.x; // Adjusted to [0, 1] range
}

// Example hash function for gradient lookup
int hash(int x, int y, int z) {
    return int(mod(float(x) + float(y) * 57.0 + float(z) * 113.0, 256.0));
}





float noise(vec3 p)
{
    return texture(noiseTexture, p).r;
}

vec3 rayDirection(vec2 uv)
{
    uv = uv * 2.0 - 1.0;
    return normalize(vec3(uv, -1.0));
}

vec4 integrateCloud(vec3 ro, vec3 rd, float stepSize, float densityFactor)
{
    vec4 accumulatedColor = vec4(0.0);
    vec3 p = ro;

    for (int i = 0; i < 128; i++)
    {
        float density = perlinNoise(p) * densityFactor;
        vec4 sampleColor = vec4(1.0, 1.0, 1.0, density);
        accumulatedColor = accumulatedColor + sampleColor * (1.0 - accumulatedColor.a);
        if (accumulatedColor.a >= 1.0)
            break;
        p += rd * stepSize;
    }

    return accumulatedColor;
}

void main()
{
    vec3 ro = ViewerPosition;
    vec3 rd = rayDirection(TexCoords);

    vec4 color = integrateCloud(ro, rd, stepSize, densityFactor);
    FragColor = color;
}
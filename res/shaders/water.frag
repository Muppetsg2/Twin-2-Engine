#version 450 core

#define MAGIC 43758.5453123
#define AMBIENT_POWER 0.2

layout (std140, binding = 1) uniform WindowData {
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
    float time;
    float deltaTime;
};

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[1024];
};

in VS_OUT {
    vec3 normal;
    vec3 fragPos;
    vec3 objPos;
    vec2 texCoord;
    float wValue;
    flat uint instance_id;
} fs_in;

struct MaterialInput {
    // Colors
    vec4 shallowColor;
    vec4 deepColor;
    vec4 foamColor;
    
    // Foam Values
    float foamSpeed;
    float foamScale;
    float foamDepth;
    
    // Depth Values
    float depthFade;

    // Vertex Modification Data
    float waterScale;
	float waterHeight;
	float waterSpeed;
};

layout(std140, binding = 2) uniform MaterialInputBuffer {
    MaterialInput materialInputs[8];
};

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
	float power;		  // Light source power
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

uniform sampler2D depthMap;

out vec4 Color;

vec2 random2(vec2 st){
    vec2 s = vec2(
      dot(st, vec2(127.1,311.7)),
      dot(st, vec2(269.5,183.3))
    );
    return -1. + 2. * fract(sin(s) * MAGIC);
}

vec2 scale(vec2 p, float s) {
    return p * s;
}

float interpolate(float t) {
    // return t;
    // return t * t * (3. - 2. * t); // smoothstep
    return t * t * t * (10. + t * (6. * t - 15.)); // smootherstep
}

vec4 gradientNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float f11 = dot(random2(i + vec2(0., 0.)), f - vec2(0., 0.));
    float f12 = dot(random2(i + vec2(0., 1.)), f - vec2(0., 1.));
    float f21 = dot(random2(i + vec2(1., 0.)), f - vec2(1., 0.));
    float f22 = dot(random2(i + vec2(1., 1.)), f - vec2(1., 1.));
    
    return vec4(f11, f12, f21, f22);
}

float noise(vec2 p) {
    vec4 v = gradientNoise(p);
    
    vec2 f = fract(p);
    float t = interpolate(f.x);
    float u = interpolate(f.y);
    
    // linear interpolation on t and u,
    // the returned value belongs to [0, 1]
    return mix(
        mix(v.x, v.z, t),
        mix(v.y, v.w, t), 
        u
    ) * .5 + .5;
}

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 color) {
    vec3 lightDir = normalize(-light.direction);
    // swiatlo rozproszone
    float diff = max(dot(normal, lightDir), 0.0);
    // swiatlo lustrzane
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(viewDir, halfwayDir), 0.0), shininess);
    // polacz wyniki
    vec3 ambient = light.color * AMBIENT_POWER * light.power * color;
    vec3 diffuse = light.color * diff * light.power * color;
    //vec3 specular = light.specular * spec * color;
    //return (ambient + diffuse + specular);

    //vec3 res = clamp(diffuse + ambient, ambient, color * light.diffuse);
    //return res;
    return diffuse + ambient;
}

float LinearEyeDepth(float depth, float near, float far) {
    return 1.0 / (((1.0 - (far / near)) / far) * depth + ((far / near) / far));
}

void main() {

    mat4 model = transform[fs_in.instance_id];
    vec3 N = normalize(vec3(mat3(transpose(inverse(model))) * cross(dFdy(fs_in.objPos), dFdx(fs_in.objPos)))) * -1.0;

    vec3 viewDir  = normalize(viewPos - fs_in.fragPos);

    vec4 screenPos = vec4(gl_FragCoord.xy / windowSize.xy, gl_FragCoord.z * fs_in.wValue, fs_in.wValue);
    float depthRaw = texture(depthMap, screenPos.xy).x;
    float depthEye = LinearEyeDepth(depthRaw, nearPlane, farPlane); // Eye
    vec2 uv = fs_in.fragPos.xz + (materialInputs[0].foamSpeed * time);

    vec4 result = vec4(vec3(0.0), 1.0);

    result += vec4(vec3(noise(scale(uv, materialInputs[0].foamScale))), 1.0);

    float depth = depthEye - screenPos.w;

    result += vec4(vec3(depth), 1.0);

    result /= materialInputs[0].foamDepth;

    result = step(result, materialInputs[0].foamColor);

    for (int i = 0; i < numberOfDirLights; ++i) {
        result += vec4(CalcDirLight(directionalLights[i], N, viewDir, vec3(mix(materialInputs[0].deepColor, materialInputs[0].shallowColor, clamp(depth / materialInputs[0].depthFade, 0.0, 1.0)))), 1.0);
    }

    Color = vec4(vec3(pow(result.rgb, vec3(gamma))), 1.0);
}
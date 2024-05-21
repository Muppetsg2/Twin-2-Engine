#version 450 core

#define MAGIC 43758.5453123
#define AMBIENT_POWER 0.2

layout (std140, binding = 1) uniform WindowData {
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

in VS_OUT {
    vec2 texCoord;
    vec3 normal;
    vec3 fragPos;
    float wValue;
    vec3 objPos;
} fs_in;

struct DirectionalLight {
    mat4 lightSpaceMatrix;  // For Shadow Mapping
    vec3 color;             // Color of the dir light
	vec3 direction;         // Direction of the dir light
	float power;		    // Light source power
    uint shadowMapFBO;
    uint shadowMap;
};

struct FragWater {
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
};

uniform float time;
uniform mat4 model;
uniform FragWater fWater;

uniform DirectionalLight dirLight;
uniform float shininess;
uniform mat4 normalModel;

uniform sampler2D depthMap;

out vec4 Color;

vec2 random2(vec2 st){
    vec2 s = vec2(
      dot(st, vec2(127.1,311.7)),
      dot(st, vec2(269.5,183.3))
    );
    return -1. + 2. * fract(sin(s) * MAGIC);
}

vec2 scale (vec2 p, float s) {
    return p * s;
}

float interpolate (float t) {
    // return t;
    // return t * t * (3. - 2. * t); // smoothstep
    return t * t * t * (10. + t * (6. * t - 15.)); // smootherstep
}

vec4 gradientNoise (vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float f11 = dot(random2(i + vec2(0., 0.)), f - vec2(0., 0.));
    float f12 = dot(random2(i + vec2(0., 1.)), f - vec2(0., 1.));
    float f21 = dot(random2(i + vec2(1., 0.)), f - vec2(1., 0.));
    float f22 = dot(random2(i + vec2(1., 1.)), f - vec2(1., 1.));
    
    return vec4(f11, f12, f21, f22);
}

float noise (vec2 p) {
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

void main()
{
    vec3 N = normalize(vec3(mat3(normalModel) * cross(dFdy(fs_in.objPos), dFdx(fs_in.objPos)))) * -1.0;

    vec3 viewDir  = normalize(viewPos - fs_in.fragPos);

    vec4 screenPos = vec4(gl_FragCoord.xy / windowSize.xy, gl_FragCoord.z * fs_in.wValue, fs_in.wValue);
    float depthRaw = texture(depthMap, screenPos.xy).x;
    float depthEye = LinearEyeDepth(depthRaw, nearPlane, farPlane); // Eye
    vec2 uv = fs_in.fragPos.xz + (fWater.foamSpeed * time);

    vec4 result = vec4(0.f, 0.f, 0.f, 1.f);

    result += vec4(vec3(noise(scale(uv, fWater.foamScale))), 1.0);

    float depth = depthEye - screenPos.w;

    result += vec4(vec3(depth), 1.0);

    result /= fWater.foamDepth;

    result = step(result, fWater.foamColor);

    result += vec4(CalcDirLight(dirLight, N, viewDir, vec3(mix(fWater.deepColor, fWater.shallowColor, clamp(depth / fWater.depthFade, 0, 1)))), 1.0);

    Color = vec4(vec3(pow(result.rgb, vec3(gamma))), 1.0);
}
#version 450 core

#define MAGIC 43758.5453123

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

out VS_OUT {
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
	float wValue;
    vec3 objPos;
} vs_out;

struct VertWater {
	float waterScale;
	float waterHeight;
	float waterSpeed;
};

uniform VertWater vWater;
uniform float time;
uniform mat4 model;
uniform mat4 normalModel;

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

void main()  
{
	vec4 worldPos = model * vec4(position, 1.f);

    vec2 uv = worldPos.xz + (vWater.waterSpeed * time);

    vec3 pos = vec3(position.x, noise(scale(uv, vWater.waterScale)) * vWater.waterHeight, position.z);

	gl_Position = projection * view * model * vec4(pos, 1.f);
	vs_out.wValue = gl_Position.w;
	vs_out.texCoord = texCoord;
	vs_out.normal = mat3(normalModel) * normal;
	vs_out.fragPos = vec3(model * vec4(pos, 1.f));
    vs_out.objPos = pos;
}
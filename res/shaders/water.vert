#version 450 core

#define MAGIC 43758.5453123

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[1024];
};

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

out VS_OUT {
	vec3 normal;
	vec3 fragPos;
    vec3 objPos;
	vec2 texCoord;
	float wValue;
    flat uint instance_id;
} vs_out;

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

void main()  
{
    mat4 model = transform[gl_InstanceID];

	vec4 worldPos = model * vec4(position, 1.0);
    
    vec2 uv = worldPos.xz + (materialInputs[0].waterSpeed * time);

    vec3 pos = vec3(position.x, position.y + map(noise(scale(uv, materialInputs[0].waterScale)), 0.0, 1.0, -1.0, 1.0) * (materialInputs[0].waterHeight / 2.0), position.z);

	gl_Position = projection * view * model * vec4(pos, 1.0);
	vs_out.normal = mat3(transpose(inverse(model))) * normal;
	vs_out.fragPos = vec3(model * vec4(pos, 1.0));
	vs_out.texCoord = texCoord;
	vs_out.wValue = gl_Position.w;
    vs_out.objPos = pos;
    vs_out.instance_id = gl_InstanceID;
}
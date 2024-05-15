#version 450 core 
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[1024];
};

layout (std430, binding = 1) buffer MaterialIndexes {
    int materialIndexes[1024];
};

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
	vec4 clipSpacePos;
    flat uint materialIndex; // flat = nie interpolowane
} vs_out;

void main()  
{
    mat4 model = transform[gl_InstanceID];

    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.clipSpacePos = gl_Position;

    vs_out.texCoord = texCoord;
    vs_out.normal = mat3(transpose(inverse(model))) * normal;
    vs_out.fragPos = vec3(model * vec4(position, 1.0));
    vs_out.materialIndex = materialIndexes[gl_InstanceID];
}

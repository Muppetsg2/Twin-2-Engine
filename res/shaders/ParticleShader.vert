#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

layout (std430, binding = 5) buffer ParticlePositions {
	vec4 particlePos[1];
};

out VS_OUT {
    flat uint instanceID;
    vec2 texCoords;
    flat vec3 pos;
    flat bool ui;
} vs_out;

void main()
{
    uint instanceId = gl_InstanceID;

    vs_out.instanceID = instanceId;
    vs_out.texCoords = aTexCoords;
    vs_out.pos = vec3(particlePos[instanceId]);

    vs_out.ui = false;

    gl_Position = projection * view * (particlePos[instanceId] + vec4(aPos, 0.0));
}
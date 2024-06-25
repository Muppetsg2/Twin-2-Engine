#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (std430, binding = 5) buffer ParticlePositions {
	vec2 particlePos[1];
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
    vs_out.pos = vec3(particlePos[instanceId], 0.0);
    vs_out.ui = true;

    gl_Position = vec4(particlePos[instanceId] + aPos, 0.0, 1.0);
}
#version 450 core
layout (location = 0) in vec4 aPos;
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

out vec2 texCoords;

void main()
{
    uint instanceId = gl_InstanceID;

    texCoords = aTexCoords;

    gl_Position = projection * view * vec4((particlePos[instanceId] + aPos).xyz, 1.0);
}
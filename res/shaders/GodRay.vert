#version 430 core
//GodRay.vert

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

layout (location = 0) out float FragModelPosY;
layout (location = 1) out vec3 fNormal;
layout (location = 2) out vec3 fPos;

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

layout (std430, binding = 0) buffer InstanceBuffer {
    mat4 transform[1];
} instanceData;


void main() {
    uint instanceId = gl_InstanceID;

    fNormal = vec3(mat3(transpose(inverse(instanceData.transform[instanceId]))) * aNormal);
    //TexCoords = aTexCoords;
    
    FragModelPosY = aPos.y;
    fPos = vec3(instanceData.transform[instanceId] * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(fPos, 1.0);
}
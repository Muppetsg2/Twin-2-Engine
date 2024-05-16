#version 450
layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
};

layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

in vec2 TexCoord;
   
uniform sampler2D depthTexture;
uniform sampler2D noiseTexture;

uniform float sampleRadius; // 0.5
uniform float bias; // 0.025
uniform vec3 kernel[64];

out vec4 Color;

vec3 calcFragPosition(vec2 coords) {
        float fragmentDepth = texture(depthTexture, coords).r;
      
        vec4 ndc = vec4(coords.x * 2.0 - 1.0, coords.y * 2.0 - 1.0, fragmentDepth * 2.0 - 1.0, 1.0);
        
        // Transform to view space using inverse camera projection matrix.
        vec4 vs_pos = inverse(projection) * ndc;
      
        // since we used a projection transformation (even if it was in inverse)
        // we need to convert our homogeneous coordinates using the perspective divide.
        vs_pos.xyz = vs_pos.xyz / vs_pos.w;
          
        return vs_pos.xyz;
}

void main() {
    vec3 fragPos = calcFragPosition(TexCoord);
    vec3 fragNormal = cross(dFdy(fragPos.xyz), dFdx(fragPos.xyz));
    fragNormal = normalize(fragNormal * -1.0);


    vec2 noiseScale = vec2(float(windowSize.x) / 4.0, float(windowSize.y) / 4.0) / 2.0;
    vec3 randomVec = texture(noiseTexture, TexCoord * noiseScale).xyz;

    vec3 tangent   = normalize(randomVec - fragNormal * dot(randomVec, fragNormal));
    vec3 bitangent = cross(fragNormal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, fragNormal);

    float occlusion = 0.0;
    for(int i = 0; i < 64; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * kernel[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * sampleRadius;

        vec4 offset = vec4(samplePos, 1.0);
        offset      = projection * offset;    // from view to clip-space
        offset.xyz /= offset.w;               // perspective divide
        offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        float sampleDepth = calcFragPosition(offset.xy).z;//texture(depthTexture, offset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, sampleRadius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck; 
    }

    occlusion = 1.0 - (occlusion / 64);
    Color = vec4(occlusion, 0.0, 0.0, 1.0); 
}
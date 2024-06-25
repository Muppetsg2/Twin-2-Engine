#version 450 core
in VS_OUT {
    flat uint instanceID;
    vec2 texCoords;
    flat vec3 pos;
    flat bool ui;
} fs_in;

out vec4 FragColor;

uniform sampler2D particleTexture;
uniform vec3 particleEmmiterPos;
uniform float maxHeight;

uniform vec4 startColor;
uniform vec4 endColor;

float map(float x, float currStart, float currEnd, float expectedStart, float expectedEnd) 
{
    return expectedStart + ((expectedEnd - expectedStart) / (currEnd - currStart)) * (x - currStart);
}

void main()
{
    vec4 color = texture(particleTexture, fs_in.texCoords);
    if (color.w == 0.0) {
        discard;
    }

    if (fs_in.ui) {
        FragColor = color * mix(startColor, endColor, map(fs_in.pos.y, 0.0, maxHeight, 0.0, 1.0));
    }
    else {
        FragColor = color * mix(startColor, endColor, map(fs_in.pos.y, particleEmmiterPos.y, particleEmmiterPos.y + maxHeight, 0.0, 1.0));   
    }
}
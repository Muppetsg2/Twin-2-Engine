#version 450 core
layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

in VS_OUT {
    vec2 texCoord;
    vec3 fragPos;
} fs_in;

struct SPRITE {
    vec4 color;
    float width;
    float height;
    float x;
    float y;
    float texWidth;
    float texHeight;
    sampler2D img;
};

uniform SPRITE sprite;
uniform bool isText;

out vec4 Color;

float map(float value, float min1, float max1, float min2, float max2) {
    return ((value - min1) / (max1 - min1)) * (max2 - min2) + min2;
}

vec2 map(vec2 value, vec2 min1, vec2 max1, vec2 min2, vec2 max2) {
    return vec2(map(value.x, min1.x, max1.x, min2.x, max2.x), map(value.y, min1.y, max1.y, min2.y, max2.y));
}

void main() 
{
    float InvTexWidth = 1.0 / sprite.texWidth;
    float InvTexHeight = 1.0 / sprite.texHeight;
    vec2 uvMin = vec2(sprite.x * InvTexWidth, sprite.y * InvTexHeight);
    vec2 uvMax = vec2((sprite.x + sprite.width) * InvTexWidth, (sprite.y + sprite.height) * InvTexHeight);
    vec2 uv = map(fs_in.texCoord, vec2(0.0, 0.0), vec2(1.0, 1.0), uvMin, uvMax);
    if (!isText) {
        Color = texture(sprite.img, uv) * vec4(pow(sprite.color.rgb, vec3(gamma)), sprite.color.a);
    }
    else {
        Color = vec4(1.0, 1.0, 1.0, texture(sprite.img, uv).r) * vec4(pow(sprite.color.rgb, vec3(gamma)), sprite.color.a);
        //Color = vec4(pow(sprite.color.rgb, vec3(gamma)), sprite.color.a);
    }
}
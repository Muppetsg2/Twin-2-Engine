#version 450 core
layout (std140, binding = 1) uniform WindowData
{
    vec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

in vec2 TexCoord;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

out vec4 Color;  

uniform bool hasBlur;
uniform bool hasVignette;
uniform bool hasNegative;
uniform bool hasGrayscale;
uniform bool displayDepth;
uniform bool hasOutline;

vec3 applyVignette(vec3 color) {
    vec2 position = TexCoord.xy - vec2(0.5);  
    float dist = length(position * 2);
    vec3 tintColor = vec3(0.0, 0.0, 0.0);
    //float dist = length(position * vec2(windowSize.x / windowSize.y, 1.0));

    float radius = 0.7;
    float softness = 0.5;
    float vignette = 1.0 - smoothstep(radius, radius + softness, dist);

    vec3 displayColor = color * vignette;
    vec3 vignetteColor = (1.0 - color) * tintColor * (1.0 - vignette);

    return displayColor + vignetteColor;
}

vec3 applyNegative(vec3 color) {
    return 1.0 - color;
}

vec3 applyGrayscale(vec3 color) {
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return vec3(average);
}

float Linear01Depth(float depth, float near, float far) {
	return 1.0 / ((1.0 - (far / near)) * depth + (far / near));
}

float LinearEyeDepth(float depth, float near, float far) {
    return 1.0 / (((1.0 - (far / near)) / far) * depth + ((far / near) / far));
}

vec3 getDepthValue(vec2 coord) {
    return vec3(Linear01Depth(texture(depthTexture, coord).r, nearPlane, farPlane));
}

vec3 applyBlur() {
    vec2 direction = normalize(vec2(1.0));
    vec2 resolution = displayDepth ? textureSize(depthTexture, 0) : textureSize(screenTexture, 0);
    vec2 uv = TexCoord;

    vec3 color = vec3(0.0);
    vec2 off1 = vec2(1.411764705882353) * direction;
    vec2 off2 = vec2(3.2941176470588234) * direction;
    vec2 off3 = vec2(5.176470588235294) * direction;

    color += (displayDepth ? getDepthValue(uv) : texture(screenTexture, uv).rgb) * 0.1964825501511404;
    color += (displayDepth ? getDepthValue(uv + (off1 / resolution)) : texture(screenTexture, uv + (off1 / resolution)).rgb) * 0.2969069646728344;
    color += (displayDepth ? getDepthValue(uv - (off1 / resolution)) : texture(screenTexture, uv - (off1 / resolution)).rgb) * 0.2969069646728344;

    color += (displayDepth ? getDepthValue(uv + (off2 / resolution)) : texture(screenTexture, uv + (off2 / resolution)).rgb) * 0.09447039785044732;
    color += (displayDepth ? getDepthValue(uv - (off2 / resolution)) : texture(screenTexture, uv - (off2 / resolution)).rgb) * 0.09447039785044732;

    color += (displayDepth ? getDepthValue(uv + (off3 / resolution)) : texture(screenTexture, uv + (off3 / resolution)).rgb) * 0.010381362401148057;
    color += (displayDepth ? getDepthValue(uv - (off3 / resolution)) : texture(screenTexture, uv - (off3 / resolution)).rgb) * 0.010381362401148057;
    return color;
}

int getDepthOffset(int i, int offset) {
    return min((1 + i + offset) % 4, 1) * (1 - 2 * (int((i + offset) / 4) % 2));
}

bool IsEdge(vec2 coord) {
    float depth = getDepthValue(coord).r;
    vec2 offset = 1.0 / textureSize(depthTexture, 0);

    for (int i = 0; i < 8; ++i) {
        float nearDepth = getDepthValue(coord.xy + (vec2(getDepthOffset(i, 1), getDepthOffset(i, -1)) * offset)).r;
        if (abs(depth - nearDepth) > 0.1) {
            return true;
        }
    }

    return false;
}

void main() {

    vec3 res = displayDepth ? getDepthValue(TexCoord) : texture(screenTexture, TexCoord).rgb;

    if (hasOutline) {
        res = IsEdge(TexCoord) ? (displayDepth ? vec3(0.5) : vec3(0.0)) : res;
    }

    if (hasBlur) {
        res = applyBlur();
    }

    if (hasGrayscale) {
        res = applyGrayscale(res);
    }

    if (hasVignette) {
        res = applyVignette(res);
    }
    
    if (hasNegative) {
        res = applyNegative(res);
    }

    Color = vec4(pow(res, vec3(1.0/gamma)), 1.0);
}
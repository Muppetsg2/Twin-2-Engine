#version 450 core
layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
    float time;
    float deltaTime;
};

in vec2 TexCoord;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform sampler2D ssaoTexture;

out vec4 Color;  

uniform bool hasBlur;
uniform bool hasVignette;
uniform bool hasNegative;
uniform bool hasGrayscale;
uniform bool hasOutline;
uniform bool hasDepthOfField;

uniform bool depthOfField2;

uniform bool displayDepth;
uniform bool displaySSAO;

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

int getDepthOffset(int i, int offset) {
    return min((1 + i + offset) % 4, 1) * (1 - 2 * (int((i + offset) / 4) % 2));
}

bool IsEdge(vec2 coord) {
    float depth = getDepthValue(coord).r;
    vec2 offset = 1.0 / textureSize(depthTexture, 0);

    for (int i = 0; i < 8; i+=2) {
        float nearDepth = getDepthValue(coord.xy + (vec2(getDepthOffset(i, 1), getDepthOffset(i, -1)) * offset)).r;
        if (abs(depth - nearDepth) > 0.0007) {
            return true;
        }
    }

    return false;
}

vec3 getColor(vec2 coord) {
    vec3 outlineColor = vec3(1.0);
    vec3 res = displayDepth ? getDepthValue(coord) : (displaySSAO ? texture(ssaoTexture, coord).rrr : texture(screenTexture, coord).rgb);

    if (hasOutline) {
        res = IsEdge(coord) ? ((displayDepth || displaySSAO) ? outlineColor / 2.0 : outlineColor) : res;
    }

    return res;
}

vec3 applyBlur() {
    vec2 direction = normalize(vec2(1.0));
    vec2 resolution = displayDepth ? textureSize(depthTexture, 0) : textureSize(screenTexture, 0);
    vec2 uv = TexCoord;

    vec3 color = vec3(0.0);
    vec2 off1 = vec2(1.411764705882353) * direction;
    vec2 off2 = vec2(3.2941176470588234) * direction;
    vec2 off3 = vec2(5.176470588235294) * direction;

    color += getColor(uv) * 0.1964825501511404;
    color += getColor(uv + (off1 / resolution)) * 0.2969069646728344;
    color += getColor(uv - (off1 / resolution)) * 0.2969069646728344;

    color += getColor(uv + (off2 / resolution)) * 0.09447039785044732;
    color += getColor(uv - (off2 / resolution)) * 0.09447039785044732;

    color += getColor(uv + (off3 / resolution)) * 0.010381362401148057;
    color += getColor(uv - (off3 / resolution)) * 0.010381362401148057;
    return color;
}

vec3 applyDepthOfField(vec3 currentOutput) {
    float currentDepth = getDepthValue(TexCoord).r;
    float centerDepth = getDepthValue(vec2(0.5, 0.5)).r;

    //vec3 blurred = vec3(0.0, 0.0, 0.0);
    vec3 blurred = applyBlur();
    
    vec2 resolution = textureSize(screenTexture, 0);

    //float distance = currentDepth - centerDepth;
    float distance = length(currentDepth - centerDepth) * 150;
    distance = (distance < 0) ? -distance : distance;

    distance = clamp(distance, 0.0, 1.0);

    //return distance * blurred + (1.0 - distance) * currentOutput;
    return distance * blurred + (1.0 - distance) * currentOutput;
}

uniform float quadraticDepthOfField;
uniform float linearDepthOfField;
uniform float constantDepthOfField;

vec3 applyDepthOfField2(vec3 color) {

    vec3 focusColor = color;
    vec3 outOfFocusColor = applyBlur();

    float focusDist = texture(depthTexture, vec2(0.5)).r;

    float currDist = texture(depthTexture, TexCoord).r;

    float dist = abs(focusDist - currDist);

    float blur = clamp(quadraticDepthOfField * dist * dist + linearDepthOfField * dist + constantDepthOfField, 0.0, 1.0);

    return mix(focusColor, outOfFocusColor, blur);
}

void main() {

    vec3 res = getColor(TexCoord);

    if (hasBlur) {
        res = applyBlur();
    }

    if (hasDepthOfField)
    {
        if (depthOfField2) {
            res = applyDepthOfField2(res);
        }
        else {
            res = applyDepthOfField(res);
        }        
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
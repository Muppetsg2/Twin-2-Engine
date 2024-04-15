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

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
vec3 applyBlur2() {
    vec2 tex_offset = displayDepth ? 1.0 / textureSize(depthTexture, 0) : 1.0 / textureSize(screenTexture, 0);  // gets size of single texel
    vec3 result = displayDepth ? getDepthValue(TexCoord) * weight[0] : texture(screenTexture, TexCoord).rgb * weight[0];    // current fragment's contribution

    // HORIZONTAL
    for(int i = 1; i < 5; ++i)
    {
        result += displayDepth ? getDepthValue(TexCoord + vec2(tex_offset.x * i, 0.0)) : texture(screenTexture, TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += displayDepth ? getDepthValue(TexCoord - vec2(tex_offset.x * i, 0.0)) : texture(screenTexture, TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }

    // VERTICAL
    for(int i = 1; i < 5; ++i)
    {
        result += displayDepth ? getDepthValue(TexCoord + vec2(0.0, tex_offset.y * i)) : texture(screenTexture, TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        result += displayDepth ? getDepthValue(TexCoord - vec2(0.0, tex_offset.y * i)) : texture(screenTexture, TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }

    return result / 2;
}

float normpdf(float x, float sigma) {
	return 0.39894 * exp(-0.5 * x * x / (sigma * sigma)) / sigma;
}

vec3 applyBlur() {
    //declare stuff
	const int mSize = 11;
	const int kSize = (mSize - 1) / 2;
	float kernel[mSize];
	vec3 result = vec3(0.0);
    vec2 tex_offset = displayDepth ? 1.0 / textureSize(depthTexture, 0) : 1.0 / textureSize(screenTexture, 0);
		
	//create the 1-D kernel
	float sigma = 7.0;
	float Z = 0.0;
	for (int j = 0; j <= kSize; ++j)
	{
		kernel[kSize + j] = kernel[kSize - j] = normpdf(float(j), sigma);
	}
		
	//get the normalization factor (as the gaussian has been clamped)
	for (int j = 0; j < mSize; ++j)
	{
		Z += kernel[j];
	}
		
	//read out the texels
	for (int i =- kSize; i <= kSize; ++i)
	{
		for (int j =- kSize; j <= kSize; ++j)
		{
            vec3 c = displayDepth ? getDepthValue(TexCoord.xy + vec2(tex_offset.x * i, tex_offset.y * j)) : texture(screenTexture, TexCoord.xy + vec2(tex_offset.x * i, tex_offset.y * j)).rgb;
			result += kernel[kSize + j] * kernel[kSize + i] * c;
		}
	}

	return result / (Z * Z);
}

void main() {

    vec3 res = vec3(0);

    if (displayDepth) {
        res = getDepthValue(TexCoord);
    }
    else {
        res = texture(screenTexture, TexCoord).rgb;
    }

    if (hasBlur) {
        //res = applyBlur2();
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
    //Color = vec4(res, 1.0);
}
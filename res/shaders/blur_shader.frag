#version 450 core
in vec2 TexCoord;

uniform sampler2D Texture;
uniform bool isDepth;
//uniform float nearPlane;
//uniform float farPlane;

float nearPlane = 0.1;
float farPlane = 1000.0;

// Gaussian Blur
uniform int blurMSize;
uniform float blurKernel[40];

out vec4 Color;

float Linear01Depth(float depth, float near, float far) {
	return 1.0 / ((1.0 - (far / near)) * depth + (far / near));
}

float LinearEyeDepth(float depth, float near, float far) {
    return 1.0 / (((1.0 - (far / near)) / far) * depth + ((far / near) / far));
}

vec3 getDepthValue(vec2 coord) {
    return vec3(Linear01Depth(texture(Texture, coord).r, nearPlane, farPlane));
}

vec3 getColor(vec2 coord) {
    return isDepth ? getDepthValue(coord) : texture(Texture, coord).rgb;
}

vec3 applyBlurGaussian() {
    if (blurMSize != 0) {
        //declare stuff
        vec3 result = vec3(0.0);
        const int kSize = (blurMSize - 1) / 2;
        vec2 tex_offset = 1.0 / textureSize(Texture, 0);

        float Z = 0.0;

        //get the normalization factor (as the gaussian has been clamped)
        for (int j = 0; j < blurMSize; ++j)
        {
            Z += blurKernel[j];
        }

        //read out the texels
        for (int i =- kSize; i <= kSize; ++i)
        {
            for (int j =- kSize; j <= kSize; ++j)
            {
                vec3 c = getColor(TexCoord.xy + vec2(tex_offset.x * i, tex_offset.y * j));
                result += blurKernel[kSize + j] * blurKernel[kSize + i] * c;
            }
        }
        return result / (Z * Z);
    }
    else {
        return getColor(TexCoord.xy);
    }
}

void main() {

    Color = vec4(applyBlurGaussian(), 1.0);
}
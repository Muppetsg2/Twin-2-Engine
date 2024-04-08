#version 450 core
layout (std140, binding = 1) uniform WindowData
{
    vec2 windowSize;
    float nearPlane;
    float farPlane;
};

in vec2 TexCoord;
uniform sampler2D screenTexture;

out vec4 Color;  

uniform bool hasBlur;
uniform bool hasVignette;
uniform bool hasNegative;
uniform bool hasGrayscale;

vec3 applyVignette(vec3 color)
{
    vec2 position = (gl_FragCoord.xy / windowSize.xy) - vec2(0.5);  
    //float dist = length(position);
    float dist = length(position * vec2(windowSize.x / windowSize.y, 1.0));

    float radius = 0.5;
    float softness = 0.2;
    float vignette = smoothstep(radius, radius - softness, dist);

    color = color - (1.0 - vignette);

    return color;
}

vec3 applyNegative(vec3 color) {
    return 1.0 - color;
}

vec3 applyGrayscale(vec3 color) {
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return vec3(average);
}

float normpdf(float x, float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

vec3 applyBlur(vec3 color) {
    //declare stuff
	const int mSize = 11;
	const int kSize = (mSize - 1) / 2;
	float kernel[mSize];
	vec3 final_colour = vec3(0.0);
		
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
	for (int i = -kSize; i <= kSize; ++i)
	{
		for (int j = -kSize; j <= kSize; ++j)
		{
			final_colour += kernel[kSize + j] * kernel[kSize + i] * texture(screenTexture, TexCoord.xy + vec2(float(i), float(j))).rgb;
		}
	}

	return final_colour / (Z * Z);
}

void main()
{ 
    vec3 res = texture(screenTexture, TexCoord).rgb;

    if (hasGrayscale) {
        res = applyGrayscale(res);
    }
    
    if (hasNegative) {
        res = applyNegative(res);
    }

    // Popracowac
    if (hasBlur) {
        res = applyBlur(res);
    }

    // Popracowac
    if (hasVignette) {
        res = applyVignette(res);
    }

    Color = vec4(res, 1.0);
}
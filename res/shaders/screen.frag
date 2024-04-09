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
    vec2 position = TexCoord.xy - vec2(0.5);  
    float dist = length(position);
    //float dist = length(position * vec2(windowSize.x / windowSize.y, 1.0));

    float radius = 0.7;
    float softness = 0.25;
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

vec3 applyBlur() {

    float Pi2 = 6.28318530718; // Pi*2
    
    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 4.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 6.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}
   
    vec3 color = texture(screenTexture, TexCoord).rgb;

    vec2 Radius = Size/windowSize;
    
    // Blur calculations
    for(float d = 0.0; d < Pi2; d += Pi2 / Directions)
    {
		for(float i = 1.0 / Quality; i <= 1.0; i += 1.0 / Quality)
        {
			color += texture(screenTexture, TexCoord + vec2(cos(d), sin(d)) * Radius * i).rgb;
        }
    }
    
    // Output to screen
    color /= Quality * Directions - 15.0;
    return color;
}

void main()
{ 
    vec3 res = texture(screenTexture, TexCoord).rgb;

    // Popracowac
    if (hasBlur) {
        res = applyBlur();
    }

    if (hasGrayscale) {
        res = applyGrayscale(res);
    }
    
    if (hasNegative) {
        res = applyNegative(res);
    }

    if (hasVignette) {
        res = applyVignette(res);
    }

    Color = vec4(res, 1.0);
}
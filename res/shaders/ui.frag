#version 450 core
layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

layout (std140, binding = 4) uniform CanvasData {
	ivec2 canvasSize;
};

layout (std140, binding = 5) uniform UIElementData {
	mat4 transform;
	vec4 color;
	vec2 elemSize;
	ivec2 spriteOffset;
	ivec2 spriteSize;
	ivec2 texSize;
	bool isText;
};

in GS_OUT {
    vec2 texCoord;
} fs_in;

uniform sampler2D image;

out vec4 Color;

void main() 
{
    vec2 invTexSize = 1.0 / texSize;
    vec2 uv = (fs_in.texCoord * spriteSize + spriteOffset) * invTexSize;
    vec4 gammaColor = vec4(pow(color.rgb, vec3(gamma)), color.a);
    if (!isText) {
        Color = texture(image, uv) * gammaColor;
    }
    else {
        Color = vec4(1.0, 1.0, 1.0, texture(image, uv).r) * gammaColor;
    }
}
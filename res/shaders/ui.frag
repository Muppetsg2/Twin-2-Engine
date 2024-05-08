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
    bool hasTexture;
};

in GS_OUT {
    vec2 texCoord;
} fs_in;

uniform sampler2D image;

out vec4 Color;

void main() 
{
    vec4 gammaColor = vec4(pow(color.rgb, vec3(gamma)), color.a);
    vec4 elemColor = vec4(1.0);
    if (hasTexture) {
        vec2 invTexSize = 1.0 / texSize;
        vec2 uv = (fs_in.texCoord * spriteSize + spriteOffset) * invTexSize;
        elemColor = texture(image, uv);
    }
    if (!isText) {
        Color = elemColor * gammaColor;
    }
    else {
        Color = vec4(1.0, 1.0, 1.0, elemColor.r) * gammaColor;
    }
}
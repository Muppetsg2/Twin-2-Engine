#version 450 core
layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

layout (std140, binding = 4) uniform CanvasData {
    mat4 canvasTransform;
	vec2 canvasSize;
    bool worldSpaceCanvas;
};

layout (std140, binding = 5) uniform UIElementData {
	mat4 elemTransform;
	mat4 maskTransform;
	vec4 color;
	vec2 elemSize;
	vec2 maskSize;
	ivec2 spriteOffset;
	ivec2 spriteSize;
	ivec2 texSize;
	bool isText;
	bool hasTexture;
	bool hasMaskTexture;
	bool useMask;
};

in GS_OUT {
	vec2 texCoord;
	vec2 screenPos;
	vec2 canvasPos;
	vec4 worldPos;
} fs_in;

uniform sampler2D image;
uniform sampler2D maskImage;

out vec4 Color;

void main() 
{
    // Is Frag In Screen
    if (fs_in.screenPos.x > 1.0 || fs_in.screenPos.x < -1.0 || fs_in.screenPos.y > 1.0 || fs_in.screenPos.y < -1.0)
        discard;

    // Is Frag In Canvas
    if (fs_in.canvasPos.x > canvasSize.x * 0.5 || fs_in.canvasPos.x < -canvasSize.x * 0.5 || fs_in.canvasPos.y > canvasSize.y * 0.5 || fs_in.canvasPos.y < -canvasSize.y * 0.5)
        discard;

    float maskPower = 1.0;
    if (useMask) {
        // Is Frag In Mask
        vec2 inMaskPos = vec2(inverse(maskTransform) * vec4(fs_in.canvasPos, 0.0, 1.0));
        if (inMaskPos.x > maskSize.x * 0.5 || inMaskPos.x < -maskSize.x * 0.5 || inMaskPos.y > maskSize.y * 0.5 || inMaskPos.y < -maskSize.y * 0.5)
            discard;
    
        if (hasMaskTexture) {
            vec2 invMaskSize = 1.0 / maskSize;
            vec2 maskTexCoord = vec2(0.5);
            maskTexCoord.x += invMaskSize.x * inMaskPos.x;
            maskTexCoord.y += -invMaskSize.y * inMaskPos.y;
            vec3 maskColor = texture(maskImage, maskTexCoord).rgb;
            maskPower = (maskColor.r + maskColor.g + maskColor.b) / 3.0;
        }
    }

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

    Color.a *= maskPower;
}
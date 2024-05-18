#version 450 core
layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
};

struct RectTransform {
    mat4 transform;
    vec2 size;
};

struct Sprite {
    uvec2 offset;
    uvec2 size;
    uvec2 texSize;
    bool isActive;
};

layout (std140, binding = 4) uniform CanvasData {
    RectTransform canvasRect;
	bool canvasIsInWorldSpace;
    bool canvasIsActive;
};

layout (std140, binding = 5) uniform MaskData {
    RectTransform maskRect;
    Sprite maskSprite;
    bool maskIsActive;
};

struct UIElement {
    RectTransform rect;
    Sprite sprite;
    vec4 color;
    bool isText;
};

const uint maxUIElements = 8;
layout (std140, binding = 3) buffer UIElementsBuffer {
	UIElement uiElements[maxUIElements];
};

in GS_OUT {
    flat uint instanceID;
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

    float maskPower = 1.0;
    if (maskIsActive) {
        // Is Frag In Mask
        vec2 inMaskPos = vec2(inverse(maskRect.transform) * vec4(fs_in.canvasPos, 0.0, 1.0));
        if (inMaskPos.x > maskRect.size.x * 0.5 || inMaskPos.x < -maskRect.size.x * 0.5 || inMaskPos.y > maskRect.size.y * 0.5 || inMaskPos.y < -maskRect.size.y * 0.5)
            discard;
    
        if (maskSprite.isActive) {
            vec2 invMaskSize = 1.0 / maskRect.size;
            vec2 maskTexCoord = vec2(0.5);
            maskTexCoord.x += invMaskSize.x * inMaskPos.x;
            maskTexCoord.y += -invMaskSize.y * inMaskPos.y;
            vec3 maskColor = texture(maskImage, maskTexCoord).rgb;
            maskPower = (maskColor.r + maskColor.g + maskColor.b) / 3.0;
        }
    }

    UIElement element = uiElements[fs_in.instanceID];

    vec4 gammaColor = vec4(pow(element.color.rgb, vec3(gamma)), element.color.a);
    vec4 elemColor = vec4(1.0);
    if (element.sprite.isActive) {
        vec2 invTexSize = 1.0 / element.sprite.texSize;
        vec2 uv = (fs_in.texCoord * element.sprite.texSize + element.sprite.offset) * invTexSize;
        elemColor = texture(image, uv);
    }

    if (!element.isText) {
        Color = elemColor * gammaColor;
    }
    else {
        Color = vec4(1.0, 1.0, 1.0, elemColor.r) * gammaColor;
    }

    Color.a *= maskPower;
}
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

struct RectTransform {
    mat4 transform;
    vec2 size;
};

struct Sprite {
    uvec2 offset;
    uvec2 size;
    bool isActive;
};

struct Texture {
    uvec2 size;
    bool isActive;
};

// FILL TYPES
const uint HORIZONTAL_FILL = 0;
const uint VERTICAL_FILL = 1;
const uint CIRCLE_FILL = 2;

// HORIZONTAL FILL SUBTYPES
const uint LEFT_FILL = 0;
const uint CENTER_FILL = 1;
const uint RIGHT_FILL = 2;

// VERTICAL FILL SUBTYPES
const uint TOP_FILL = 0;
const uint MIDDLE_FILL = 1;
const uint BOTTOM_FILL = 2;

// CIRCLE FILL SUBTYPES
const uint CW_FILL = 0;
const uint CCW_FILL = 1;

struct FillData {
    uint type;
    uint subType;
    float progress;
    float rotation;
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
    FillData maskFill;
    uvec2 maskTextureSize;
    bool maskIsActive;
};

struct UIElement {
    RectTransform rect;
    Sprite sprite;
    FillData fill;
    vec4 color;
    bool isText;
};

const uint maxUIElements = 8;
layout (std140, binding = 3) buffer UIElementsBuffer {
	UIElement uiElements[maxUIElements];
    Texture elementTexture;
};

in GS_OUT {
    flat uint instanceID;
    vec2 pointPos;
	vec2 texCoord;
	vec2 canvasPos;
	vec4 worldPos;
} fs_in;

uniform sampler2D image;
uniform sampler2D maskImage;

out vec4 Color;

bool OutOfFill(vec2 pos, vec2 center, vec2 size, FillData fill) {
    if (fill.isActive) {
        float p = 0.0;
        if (fill.type == HORIZONTAL_FILL) {
            if (fill.subType == LEFT_FILL) {
                p = (0.99 / size.x) * (pos.x - center.x) + 0.495;
            }
            else if (fill.subType == CENTER_FILL) {
                p = (1.98 / size.x) * abs(pos.x - center.x);
            }
            else if (fill.subType == RIGHT_FILL) {
                p = (-0.99 / size.x) * (pos.x - center.x) + 0.495;
            }
        }
        else if (fill.type == VERTICAL_FILL) {
            if (fill.subType == TOP_FILL) {
                p = (-0.99 / size.y) * (pos.y - center.y) + 0.495;
            }
            else if (fill.subType == MIDDLE_FILL) {
                p = (1.98 / size.y) * abs(pos.y - center.y);
            }
            else if (fill.subType == BOTTOM_FILL) {
                p = (0.99 / size.y) * (pos.y - center.y) + 0.495;
            }
        }
        else if (fill.type == CIRCLE_FILL) {
            const vec2 diff = pos - center;
            const float r = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
            if (r == 0) return false;

            const float invR = 1.0 / r;
            const float alphaX = degrees(asin(diff.x * invR));
            const float alphaY = degrees(acos(diff.y * invR));

            float alpha = 0.0;
            if (alphaX >= 0.0) alpha = alphaY;
            else if (alphaX < 0.0) alpha = 360.0 - alphaY;

            float rotation = -fill.rotation;

            while (rotation >= 360.0)
                rotation -= 360.0;
            while (rotation <= -360.0)
                rotation += 360.0;

            if (rotation < 0.0 && alpha >= 360.0 + rotation)
                alpha -= 360.0 + rotation;
            else if (rotation > 0.0 && alpha < rotation)
                alpha += 360.0 - rotation;
            else
                alpha -= rotation;

            if (fill.subType == CW_FILL) {
                p = 0.00275 * alpha;
            }
            else if (fill.subType == CCW_FILL) {
                p = 0.00275 * (360.0 - alpha);
            }
        }
        return p >= fill.progress;
    }
    return false;
}

void main()
{
    if (!canvasIsInWorldSpace || !canvasIsActive) {
        // Is Frag In Screen
        if (fs_in.worldPos.x > 1.0 || fs_in.worldPos.x < -1.0 || fs_in.worldPos.y > 1.0 || fs_in.worldPos.y < -1.0)
            discard;
    }

    float maskPower = 1.0;
    if (maskIsActive) {
        // Is Frag In Mask
        vec2 inMaskPos = vec2(inverse(maskRect.transform) * vec4(fs_in.canvasPos, 0.0, 1.0));
        if (inMaskPos.x > maskRect.size.x * 0.5 || inMaskPos.x < -maskRect.size.x * 0.5 || inMaskPos.y > maskRect.size.y * 0.5 || inMaskPos.y < -maskRect.size.y * 0.5)
            discard;

        // Is Frag Out Of Mask Fill
        if (OutOfFill(inMaskPos, vec2(0.0, 0.0), maskRect.size, maskFill))
            discard;
    
        if (maskSprite.isActive) {
            vec2 invMaskSize = 1.0 / maskRect.size;
            vec2 maskTexCoord = vec2(0.5);
            maskTexCoord.x += invMaskSize.x * inMaskPos.x;
            maskTexCoord.y += -invMaskSize.y * inMaskPos.y;
            vec3 maskColor = texture(maskImage, maskTexCoord).rgb;
            maskPower = (maskColor.r + maskColor.g + maskColor.b) / 3.0;

            if (maskPower == 0.0)
                discard;
        }
    }

    UIElement element = uiElements[fs_in.instanceID];

    // Check if even visible
    if (element.color.a == 0.0)
        discard;

    // Is Frag Out Of Image Fill
    if (OutOfFill(fs_in.canvasPos, fs_in.pointPos, element.rect.size, element.fill))
        discard;

    vec4 gammaColor = vec4(pow(element.color.rgb, vec3(gamma)), element.color.a);
    vec4 elemColor = vec4(1.0);
    if (elementTexture.isActive) {
        if (element.sprite.isActive) {
            vec2 invTexSize = 1.0 / elementTexture.size;
            vec2 uv = (fs_in.texCoord * element.sprite.size + element.sprite.offset) * invTexSize;
            elemColor = texture(image, uv);
        }
        else {
            elemColor = texture(image, fs_in.texCoord);
        }
    }

    if (!element.isText) {
        Color = elemColor * gammaColor;
    }
    else {
        Color = vec4(1.0, 1.0, 1.0, elemColor.r) * gammaColor;
    }

    if (!canvasIsInWorldSpace || !canvasIsActive) {
        Color = vec4(pow(Color.rgb, vec3(1.0 / gamma)), Color.a * maskPower);
    }
    else {
        Color = vec4(Color.rgb, Color.a * maskPower);
    }
}
#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
	float time;
    float deltaTime;
};

layout (std140, binding = 0) uniform CameraData
{
    mat4 projection;
    mat4 view;
	vec3 viewPos;
    bool isSSAO;
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

in VS_OUT {
	flat uint instanceID;
} gs_in[1];

out GS_OUT {
	flat uint instanceID;
	vec2 pointPos;
	vec2 texCoord;
	vec2 canvasPos;
	vec4 worldPos;
} gs_out;

void main() {
	const vec2 pointPos = vec2(gl_in[0].gl_Position);
	mat4 canvasTransform;
	vec2 invCanvasSize;
	if (canvasIsActive) {
		canvasTransform = canvasRect.transform;
		invCanvasSize = 1.0 / canvasRect.size;
	}
	else {
		canvasTransform = mat4(1.0);
		invCanvasSize = 1.0 / windowSize;
	}

	UIElement element = uiElements[gs_in[0].instanceID];
	gs_out.pointPos = vec2(element.rect.transform * vec4(pointPos, 0.0, 1.0));

	vec2 elemPos = (pointPos + vec2(-0.5, -0.5)) * element.rect.size;
	gs_out.canvasPos = vec2(element.rect.transform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (canvasIsInWorldSpace && canvasIsActive) {
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gl_Position = vec4(gs_out.worldPos.xy, 0.0, 1.0);
	}
	gs_out.texCoord = vec2(0.0, 1.0);
	gs_out.instanceID = gs_in[0].instanceID;
	EmitVertex();

	elemPos = (pointPos + vec2(0.5, -0.5)) * element.rect.size;
	gs_out.canvasPos = vec2(element.rect.transform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (canvasIsInWorldSpace && canvasIsActive) {
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gl_Position = vec4(gs_out.worldPos.xy, 0.0, 1.0);
	}
	gs_out.texCoord = vec2(1.0, 1.0);
	gs_out.instanceID = gs_in[0].instanceID;
	EmitVertex();

	elemPos = (pointPos + vec2(-0.5, 0.5)) * element.rect.size;
	gs_out.canvasPos = vec2(element.rect.transform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (canvasIsInWorldSpace && canvasIsActive) {
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gl_Position = vec4(gs_out.worldPos.xy, 0.0, 1.0);
	}
	gs_out.texCoord = vec2(0.0, 0.0);
	gs_out.instanceID = gs_in[0].instanceID;
	EmitVertex();

	elemPos = (pointPos + vec2(0.5, 0.5)) * element.rect.size;
	gs_out.canvasPos = vec2(element.rect.transform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (canvasIsInWorldSpace && canvasIsActive) {
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gs_out.texCoord = vec2(1.0, 0.0);
	gs_out.instanceID = gs_in[0].instanceID;
	EmitVertex();

	EndPrimitive();
}
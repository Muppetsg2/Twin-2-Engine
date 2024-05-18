#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140, binding = 1) uniform WindowData
{
    ivec2 windowSize;
    float nearPlane;
    float farPlane;
    float gamma;
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

in VS_OUT {
	flat uint instanceID;
} gs_in[1];

out GS_OUT {
	flat uint instanceID;
	vec2 texCoord;
	vec2 screenPos;
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

	vec2 elemPos = (pointPos + vec2(-0.5, -0.5)) * element.rect.size;
	gs_out.canvasPos = vec2(element.rect.transform * vec4(elemPos, 1.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (canvasIsInWorldSpace) {
		gs_out.screenPos = vec2(projection * view * gs_out.worldPos);
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gs_out.texCoord = vec2(0.0, 1.0);
	gs_out.instanceID = gs_in[0].instanceID;
	EmitVertex();

	elemPos = (pointPos + vec2(0.5, -0.5)) * element.rect.size;
	gs_out.canvasPos = vec2(element.rect.transform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (canvasIsInWorldSpace) {
		gs_out.screenPos = vec2(projection * view * gs_out.worldPos);
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	gs_out.texCoord = vec2(1.0, 1.0);
	gs_out.instanceID = gs_in[0].instanceID;
	EmitVertex();

	elemPos = (pointPos + vec2(-0.5, 0.5)) * element.rect.size;
	gs_out.canvasPos = vec2(element.rect.transform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (canvasIsInWorldSpace) {
		gs_out.screenPos = vec2(projection * view * gs_out.worldPos);
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	gs_out.texCoord = vec2(0.0, 0.0);
	gs_out.instanceID = gs_in[0].instanceID;
	EmitVertex();

	elemPos = (pointPos + vec2(0.5, 0.5)) * element.rect.size;
	gs_out.canvasPos = vec2(element.rect.transform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (canvasIsInWorldSpace) {
		gs_out.screenPos = vec2(projection * view * gs_out.worldPos);
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	gs_out.texCoord = vec2(1.0, 0.0);
	gs_out.instanceID = gs_in[0].instanceID;
	EmitVertex();

	EndPrimitive();
}
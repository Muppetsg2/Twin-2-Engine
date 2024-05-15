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

out GS_OUT {
	vec2 texCoord;
	vec2 screenPos;
	vec2 canvasPos;
	vec4 worldPos;
} gs_out;

void main() {
	vec2 pointPos = vec2(gl_in[0].gl_Position);
	vec2 invCanvasSize = 1.0 / canvasSize;

	vec2 elemPos = (pointPos + vec2(-0.5, -0.5)) * elemSize;
	gs_out.canvasPos = vec2(elemTransform * vec4(elemPos, 1.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (worldSpaceCanvas) {
		gs_out.screenPos = vec2(projection * view * gs_out.worldPos);
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gs_out.texCoord = vec2(0.0, 1.0);
	EmitVertex();

	elemPos = (pointPos + vec2(0.5, -0.5)) * elemSize;
	gs_out.canvasPos = vec2(elemTransform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (worldSpaceCanvas) {
		gs_out.screenPos = vec2(projection * view * gs_out.worldPos);
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	gs_out.texCoord = vec2(1.0, 1.0);
	EmitVertex();

	elemPos = (pointPos + vec2(-0.5, 0.5)) * elemSize;
	gs_out.canvasPos = vec2(elemTransform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (worldSpaceCanvas) {
		gs_out.screenPos = vec2(projection * view * gs_out.worldPos);
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	gs_out.texCoord = vec2(0.0, 0.0);
	EmitVertex();

	elemPos = (pointPos + vec2(0.5, 0.5)) * elemSize;
	gs_out.canvasPos = vec2(elemTransform * vec4(elemPos, 0.0, 1.0));
	gs_out.worldPos = canvasTransform * vec4(2.0 * invCanvasSize * gs_out.canvasPos, 0.0, 1.0);
	if (worldSpaceCanvas) {
		gs_out.screenPos = vec2(projection * view * gs_out.worldPos);
		gl_Position = projection * view * gs_out.worldPos;
	}
	else {
		gs_out.screenPos = vec2(gs_out.worldPos);
		gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	}
	gl_Position = vec4(gs_out.screenPos, 0.0, 1.0);
	gs_out.texCoord = vec2(1.0, 0.0);
	EmitVertex();

	EndPrimitive();
}
#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

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

out GS_OUT {
	vec2 texCoord;
} gs_out;

void main() {
	vec2 pointPos = vec2(gl_in[0].gl_Position);
	vec2 invCanvasSize = 1.0 / canvasSize;

	vec2 elemPos = (pointPos + vec2(-0.5, -0.5)) * elemSize;
	vec2 canvasPos = vec2(transform * vec4(elemPos, 1.0, 1.0));
	gl_Position = vec4(2.0 * invCanvasSize * canvasPos, 0.0, 1.0);
	gs_out.texCoord = vec2(0.0, 1.0);
	EmitVertex();

	elemPos = (pointPos + vec2(0.5, -0.5)) * elemSize;
	canvasPos = vec2(transform * vec4(elemPos, 1.0, 1.0));
	gl_Position = vec4(2.0 * invCanvasSize * canvasPos, 0.0, 1.0);
	gs_out.texCoord = vec2(1.0, 1.0);
	EmitVertex();

	elemPos = (pointPos + vec2(-0.5, 0.5)) * elemSize;
	canvasPos = vec2(transform * vec4(elemPos, 1.0, 1.0));
	gl_Position = vec4(2.0 * invCanvasSize * canvasPos, 0.0, 1.0);
	gs_out.texCoord = vec2(0.0, 0.0);
	EmitVertex();

	elemPos = (pointPos + vec2(0.5, 0.5)) * elemSize;
	canvasPos = vec2(transform * vec4(elemPos, 1.0, 1.0));
	gl_Position = vec4(2.0 * invCanvasSize * canvasPos, 0.0, 1.0);
	gs_out.texCoord = vec2(1.0, 0.0);
	EmitVertex();

	EndPrimitive();
}
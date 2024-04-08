#version 450 core  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

struct CANVAS {
	float width;
	float height;
	float elemWidth;
	float elemHeight;
};

layout (location = 0) out VS_OUT {
	vec2 texCoord;
	vec3 fragPos;
} vs_out;

//uniform mat4 model;
//uniform mat4 normalModel;
//uniform CANVAS canvas;
mat4 model;
mat4 normalModel;
CANVAS canvas;

float map(float value, float min1, float max1, float min2, float max2) {
    return ((value - min1) / (max1 - min1)) * (max2 - min2) + min2;
}

vec2 map(vec2 value, vec2 min1, vec2 max1, vec2 min2, vec2 max2) {
    return vec2(map(value.x, min1.x, max1.x, min2.x, max2.x), map(value.y, min1.y, max1.y, min2.y, max2.y));
}

void main()
{
	vec2 canvasPos = vec2(position.x * canvas.elemWidth, position.y * canvas.elemHeight);
	vec2 tpos = vec2(model * vec4(canvasPos, 1.0, 1.0));

	gl_Position = vec4(map(tpos, vec2(-canvas.width / 2.0, -canvas.height / 2.0), vec2(canvas.width / 2.0, canvas.height / 2.0), vec2(-1, -1), vec2(1, 1)), 0.0, 1.0);
	vs_out.texCoord = texCoord;
	vs_out.fragPos = vec3(tpos, 0.0);
}
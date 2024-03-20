#version 450 core  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out VS_OUT {
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
} vs_out;

void main()
{  
  gl_Position = vec4(position, 1.f);
  vs_out.normal = normal;
  vs_out.texCoord = texCoord;
  vs_out.fragPos = position;
}
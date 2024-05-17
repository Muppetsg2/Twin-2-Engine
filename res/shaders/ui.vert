#version 450 core  
layout (location = 0) in vec2 position;

out VS_OUT {
	flat uint instanceID;
} vs_out;

void main()
{
	gl_Position = vec4(position, 1.0, 1.0);
	vs_out.instanceID = gl_InstanceID;
}
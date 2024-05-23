#version 430 core
//CloudDepthShader.frag

layout(early_fragment_tests) in;


in vec3 FragPos;

layout (location = 0) out vec4 BackFragPos;

void main()
{             
    BackFragPos = vec4(FragPos, 1.0);
}
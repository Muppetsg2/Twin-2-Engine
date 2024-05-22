#version 430 core

layout(early_fragment_tests) in;


in vec3 FragPos;

out vec3 BackFragPos;

void main()
{             
    BackFragPos = FragPos;
}
#version 450 core
out vec4 color;

in VS_OUT {
	vec2 texCoord;
    vec3 fragPos;
} fs_in;

struct TEXT {
    sampler2D glyph;
    vec4 color;
};

uniform TEXT text;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text.glyph, fs_in.texCoord).r);
    color = text.color * sampled;
}  
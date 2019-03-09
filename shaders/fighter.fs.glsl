#version 330
precision highp float;
precision highp int;

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform bool is_hurt;
uniform bool is_blocking;
uniform float time;

//uniform vec3 fcolor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
    if (is_hurt) {
	    color = vec4(1.0, 0.7, 0.7, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
	if (is_blocking) {
		color = vec4(0.5, 0.5, 1.0, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
	else { color = vec4(1.0, 1.0, 1.0, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y)); }
}

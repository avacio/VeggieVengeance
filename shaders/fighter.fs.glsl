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
uniform float blocking_tank;
uniform float heal_animation;

//uniform vec3 fcolor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{	
	float b = blocking_tank/4000;
	
	if (is_blocking) {
		if (blocking_tank < 1800) 
			color = vec4(0.7, 0.7, 1.0, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
		else
			color = vec4(1-b/1.5, 1-b/1.5, 1.0, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
    else if (is_hurt) {
	    color = vec4(1.0, 0.7, 0.7, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
	else if (heal_animation > 0.0) {
	    color = vec4(0.7, 1.0, 0.7, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}
	else { color = vec4(1.0, 1.0, 1.0, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y)); }	
}

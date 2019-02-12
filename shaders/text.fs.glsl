#version 330
// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D text;
//uniform vec3 textColor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
//    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texcoord).r);
////	color = vec4(textColor, 1.0) * sampled;
//	color = vec4(0.0,0.0,0.0, 1.0) * sampled;
	color = vec4(0.0,0.0,0.0, 1.0) * texture(text, vec2(texcoord.x, texcoord.y));


}

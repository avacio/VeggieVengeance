#version 330
uniform sampler2D screen_texture;
uniform float time;
uniform float dead_timer;
uniform bool is_wavy;

in vec2 uv;

layout(location = 0) out vec4 color;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// HANDLE THE WATER WAVE DISTORTION HERE (you may want to try sin/cos)
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
vec2 distort(vec2 uv) {
    uv.y += cos(uv.x*25.0) * 0.01 * cos(time);
    return uv;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// HANDLE THE COLOR SHIFTING HERE (you may want to make the image slightly blue-ish)
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
vec4 color_shift(vec4 in_color) {
	vec4 color = in_color;
	color +=  vec4(0.1, 0.1, 0.1, 0);
	return color;
}

vec4 fade_color(vec4 in_color) {
	vec4 color = in_color;
	if (dead_timer > 0)
		color -= 0.1 * dead_timer * vec4(0.1, 0.1, 0.1, 0);

	return color;
}

void main()
{
//    if (is_wavy) {
//        vec2 coord = distort(uv);
//        vec4 in_color = texture(screen_texture, coord);
//        color = color_shift(in_color);
//    }
    vec2 coord;
     if (is_wavy) { coord = distort(uv); } else { coord = uv; }
    vec4 in_color = texture(screen_texture, coord);
    color = color_shift(in_color);
    color = fade_color(color);
//     color = vec4(0.0, 1.0, 0.0, 1.0);
}
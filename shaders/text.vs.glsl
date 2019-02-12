#version 330 
// Input attributes
//in vec3 in_position;
layout (location = 0) in vec4 vertex;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat4 projection;

void main()
{
	texcoord = vertex.zw;
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
}
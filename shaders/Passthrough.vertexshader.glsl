#version 330

layout(location = 3) in vec4 in_position;
out vec2 UV;

void main()
{
    gl_Position = in_position;
	UV = (in_position.xy + vec2(1, 1)) / 2.0;
}
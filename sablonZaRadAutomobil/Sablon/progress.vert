#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;
out vec4 channelCol;
uniform vec2 uP;

void main()
{
	if(inPos.x != -1.0)
	{
		gl_Position = vec4(inPos.x + uP.x, inPos.y, 0.0, 1.0);
		channelCol = inCol;
	}else{
		gl_Position = vec4(inPos, 0.0, 1.0);
		channelCol = inCol;
	}
}
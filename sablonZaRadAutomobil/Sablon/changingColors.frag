#version 330 core

in vec4 channelCol; //Kanal iz Verteks sejdera - mora biti ISTOG IMENA I TIPA kao u vertex sejderu
out vec4 outCol;

uniform vec3 uA;

void main()
{
	outCol = vec4(channelCol.x + uA.x, channelCol.y +  uA.y,channelCol.z +  uA.z, 1.0);
}
#version 330 core

uniform mat4 camMatrix;
uniform mat4 projMatrix;

layout(location=0)in vec3 vertexPosition;
layout(location=1)in vec3 instancePosition;
layout(location=2)in vec3 instanceColor;
layout(location=3)in float instanceSize;

out vec3 mColor;
out float size;
void main(){
	vec3 pos=(instanceSize*vertexPosition)+instancePosition;
	mat4 matrix = projMatrix*camMatrix;
	gl_Position=matrix*vec4(pos,1.);
	//gl_Position=vec4(pos,1.);
	mColor=instanceColor;
	size=instanceSize;
}
#version 330 core

uniform mat4 camMatrix;
uniform mat4 projMatrix;

layout(location=0)in vec3 vertexPosition;
layout(location=1)in vec3 vertexColor;

out vec3 mColor;
void main(){
	gl_Position=projMatrix*camMatrix*vec4(vertexPosition,1.);
	mColor=vertexColor;
}
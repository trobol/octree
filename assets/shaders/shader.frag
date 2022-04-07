#version 330 core

out vec3 color;
in vec3 mColor;
in float size;

void main(){
	color=mColor;
	gl_FragDepth=gl_FragCoord.z + 0.00001*size; // reduce z-fighting on branches by making larger ones slightly further back
}
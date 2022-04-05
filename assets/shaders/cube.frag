#version 330 core

out vec4 color;
in vec3 mColor;
in float size;

void main(){
	color=vec4(mColor, 1);
	gl_FragDepth=gl_FragCoord.z + 0.001*size; // reduce z-fighting on branches by making larger ones slightly further back
	
}
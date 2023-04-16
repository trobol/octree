#version 330 core
#extension GL_EXT_gpu_shader4 : enable
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
layout(location=0)in vec3 vertexPosition;

out vec3 vertWorldPos;
out vec3 vertWorldNormal;
out vec3 mColor;

void main(){
	vec3 pos=vertexPosition;
	vertWorldPos = (viewMatrix * vec4(vertexPosition,1.0)).xyz;
	gl_Position=projMatrix*viewMatrix*vec4(pos,1.);
	//gl_Position=vec4(pos,1.);
	mColor=vec3( 1.0, 1.0, 1.0 );
}
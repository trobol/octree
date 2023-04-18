#version 330
layout(location=0)in vec2 vp;
layout(location=1)in vec2 vt;
out vec2 uv;
out vec2 screen_uv;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

void main(){
    uv=vt;
	screen_uv=vp;
	//screen_uv=(viewMatrix*vec4(vp, 0, 1.)).xy;
	//gl_Position=projMatrix*viewMatrix*vec4(vp, 0.,1.);
    gl_Position=vec4(vp,0.,1.);
};
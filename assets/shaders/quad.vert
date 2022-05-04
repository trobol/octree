#version 330
layout(location=0)in vec2 vp;
layout(location=1)in vec2 vt;
out vec2 uv;
out vec2 screen_uv;

void main(){
    uv=vt;
	screen_uv=vp;
    gl_Position=vec4(vp,0.,1.);
};
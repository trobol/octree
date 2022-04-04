#version 330
layout(location=0)in vec2 vp;
layout(location=1)in vec2 vt;
out vec2 uv;

void main(){
    uv=vp;
    gl_Position=vec4(vp,0.,1.);
};
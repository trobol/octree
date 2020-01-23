#version 330 core
#define PI 3.1415926535897932384626433832795

mat4 BuildPerspProjMat(float fov,float aspect,
float znear,float zfar)
{
	
	float xymax=znear*tan(fov*PI/360);
	float ymin=-xymax;
	float xmin=-xymax;
	
	float width=xymax-xmin;
	float height=xymax-ymin;
	
	float depth=zfar-znear;
	float q=-(zfar+znear)/depth;
	float qn=-2*(zfar*znear)/depth;
	
	float w=2*znear/width;
	w=w/aspect;
	float h=2*znear/height;
	
	return mat4(
		w,
		0.,
		0.,
		0.,
		
		0.,
		h,
		0.,
		0.,
		
		0.,
		0.,
		q,
		-1.,
		
		0.,
		0.,
		qn,
	0.);
}

mat4 camMatrix=BuildPerspProjMat(90.,1.,1.,1000.);

layout(location=0)in vec3 vertexPosition;
layout(location=1)in vec3 vertexColor;

out vec3 mColor;
void main(){
	gl_Position=vec4(vertexPosition-vec3(0.,0.,2.),1.)*camMatrix;
	mColor=vertexColor;
}
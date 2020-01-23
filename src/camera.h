#ifndef _CAMERA_H
#define _CAMERA_H
#include "vec3.h"
#include "mat4.h"

#include "transform.h"
class Camera
{
public:
	Camera(){};
	Camera(vec3 pos, Quaternion rot, float fov) : mTransform{pos, rot}, mFov{fov} {};

	float mFov;

	Transform mTransform;
	mat4 getProjMatrix()
	{
		return mTransform.ToMatrix();
	}
	mat4 getTransformMatrix()
	{
		return mat4::PerspectiveProj(mFov, aspectRatio, nearClip, farClip);
	}

private:
};

#endif
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

	float mFov = 90;

	Transform mTransform;

	float mAspectRatio = 1;
	float mNearClip = 1;
	float mFarClip = 1000;

	mat4 getProjMatrix()
	{
		return mat4::PerspectiveProj(mFov, mAspectRatio, mNearClip, mFarClip);
	}
	mat4 getTransformMatrix()
	{
		return mTransform.ToMatrix();
	}

private:
};

#endif
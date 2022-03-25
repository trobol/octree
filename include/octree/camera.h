#ifndef _CAMERA_H
#define _CAMERA_H
#include "math/vec3.h"
#include "math/mat4.h"

#include "math/transform.h"
#include <math.h>

class Camera
{
public:
	Camera() {};
	Camera(vec3 pos, Quaternion rot, float fov) : mTransform{ pos, rot }, mFov{ fov } {};

	float mFov = 90;

	void rotateLook(float x, float y) {
		lookX += x;
		lookY += y;

		if (lookX > 2.0) lookX -= 2.0;
		if (lookX < -2.0) lookX += 2.0;
		if (lookY > 0.49999) lookY = 0.49999;
		if (lookY < -0.49999) lookY = -0.49999;
		
	

		mTransform.rotation = Quaternion::AxisAngle(vec3::up, lookX * PI) * Quaternion::AxisAngle(vec3::right, lookY * PI);
	}

	vec3 getRight() {
		return Quaternion::AxisAngle(vec3::up, lookX * PI) * vec3::right;
	}

	float lookX = 0;
	float lookY = 0;

	Transform mTransform;

	float mNearClip = 1;
	float mFarClip = 1000;

	bool bCanCaptureMouse;

	float sensitivity = 1.0f;
	float speed = 1.0f;

	mat4 getProjMatrix()
	{
		return mat4::PerspectiveProj(mFov, Window::getInstance().getAspectRatio(), mNearClip, mFarClip);
	}
	mat4 getViewMatrix()
	{
		vec3 view_dir = mTransform.rotation * vec3::forwards; //Quaternion::AxisAngle(vec3::up, lookX * PI) * Quaternion::AxisAngle(vec3::right, lookY * PI) * vec3::forwards;
		return mat4::lookAt(mTransform.position, view_dir, vec3::up);
	}

	void Update(float dt) {
		if (!bCanCaptureMouse) return;
		Window& window = Window::getInstance();
		GLFWwindow* winptr = window.mWindow;

		
		if (glfwGetMouseButton(window.mWindow, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) return;

		float realSens = sensitivity * -0.001f;
		double xpos, ypos;
		glfwGetCursorPos(winptr, &xpos, &ypos);
		glfwSetCursorPos(winptr, 0, 0);
		rotateLook((float)xpos * realSens, (float)ypos * realSens);

		
		vec3 right = getRight();
		vec3 forward = vec3::cross(vec3::up, right);
		// MOVEMENT
		if (glfwGetKey(winptr, GLFW_KEY_W))
			mTransform.position += forward * speed;

		if (glfwGetKey(winptr, GLFW_KEY_S))
			mTransform.position -= forward * speed;

		if (glfwGetKey(winptr, GLFW_KEY_SPACE))
			mTransform.position.y += speed;
		if (glfwGetKey(winptr, GLFW_KEY_LEFT_SHIFT))
			mTransform.position.y -= speed;

		if (glfwGetKey(winptr, GLFW_KEY_D))
			mTransform.position += right * speed;
		if (glfwGetKey(winptr, GLFW_KEY_A))
			mTransform.position -= right * speed;

		
	}

private:
};

#endif
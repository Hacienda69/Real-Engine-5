#pragma once
#include "Component.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"


class Component;
class GameObject;

class ComponentCamera : public Component 
{
public:
	ComponentCamera();
	ComponentCamera(GameObject* owner);
	~ComponentCamera();

	void SetCam();
	void SetBuffers();

	void UpdateCam();

public:

	Frustum frustum;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float fov;

private:
	uint camBuffer, frameBuffer, objBuffer;
};
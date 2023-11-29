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

	void Update();

	float* GetViewMatrix();
	float* GetProjetionMatrix();

	void Look(const float3& pos, const float3& ref);
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);

public:

	Frustum frustum;
	float3 reference;

	int fov = 60;
	int farDist = 500;

	bool mainCam = false;

private:

	//uint camBuffer, frameBuffer, objBuffer;

	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
};
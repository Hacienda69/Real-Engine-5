#pragma once
#include "Component.h"
#include "GameObject.h"
#include "ModuleAssimpMeshes.h"
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

	uint* GetFrameBuffer();
	uint* GetCamBuffer();
	uint* GetObjBuffer();

	void Look(const float3& pos, const float3& ref);
	void LookAt(const float3& Spot);
	
	//void Move(const float3& Movement);

	void PrintInspector();

	void RenderFrustum();

	int ContainsAABB(AABB& refBox);

public:

	Frustum frustum;
	float3 reference;

	int fov = 60;
	int farDist = 500;
	bool perspective = true;
	bool mainCam = false;

	int cameraType; // 0 = Perspective, 1 = Orthographic

private:

	uint frameBuffer; // Frame buffer
	uint camBuffer;	  // Color buffer
	uint objBuffer;	  // Depth buffer

	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;

	bool renderFrustum = true;
};
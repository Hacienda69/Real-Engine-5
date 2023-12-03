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
<<<<<<< Updated upstream
	void Move(const float3& Movement);
=======
	
	//void Move(const float3& Movement);

	void RenderFrustum();

	void PrintInspector();
>>>>>>> Stashed changes

	void StartDraw();
	void EndDraw();

	void ReGenerateBuffer(int w, int h);

public:

	Frustum frustum;
	float3 reference;

	int fov = 60;
	int farDist = 500;

	bool mainCam = false;

private:

<<<<<<< Updated upstream
	//uint camBuffer, frameBuffer, objBuffer;
=======
	unsigned int frameBuffer; // Frame buffer
	unsigned int colorBuffer; // Color buffer
	unsigned int objBuffer;	  // Depth buffer
>>>>>>> Stashed changes

	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
};
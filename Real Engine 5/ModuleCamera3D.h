#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"

#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3& Position, const float3& Reference, bool RotateAroundReference = false);
	void LookAt(const float3& Spot);

	float* GetViewMatrix();
	void FocusCameraToSelectedObject();

private:

	//Simulates First_Person or Around_Reference rotations
	void ChangeReference(const bool& firstperson);

	void CalculateViewMatrix();

public:

	//You won't need this after using Frustum
	float3 X, Y, Z, Position, Reference, distanceToReference, defaultDistToRef;
	float dt;

private:

	//True: first person camera rotation / False: rotation around reference
	bool First_Person = true;

	//True: calls ChangeReference()
	bool Rotate_Camera = false;

	//Frustum mMainCamera; Some help here :)
	mat4x4 ViewMatrix;
};
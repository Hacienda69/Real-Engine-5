#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "ComponentCamera.h"

#include "glmath.h"

class ComponentCamera;

enum class camStates {
	NORMAL,
	FIRST_PERSON,
	FOCUSED,
};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	float3 GetSelectedObjPos();

	Ray GenerateRayFromMouse(int mouse_x, int mouse_y);

private:
	void MouseRotation(float dx, float dy, float sensitivity);

public:
	ComponentCamera* sceneCamera;
	camStates state;
};
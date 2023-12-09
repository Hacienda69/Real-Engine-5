#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"

#include "ComponentCamera.h"

#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl2.h"

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

	LineSegment GenerateRayFromMouse();
	ImVec2 NormalizeMousePos(float x, float y, float w, float h, ImVec2 p);

private:
	void MouseRotation(float dx, float dy, float sensitivity);

public:
	ComponentCamera* sceneCamera;
	camStates state;
};
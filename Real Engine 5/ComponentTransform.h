#pragma once

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "ImGui/imgui.h"
#include "PhysBody3D.h"
#include <vector>
#include <string>
#include "glmath.h"

class ComponentTransform : public Component
{
public:
	struct AffectedCollider 
	{
		PhysBody3D* collider;
		mat4x4 offset;
	};

	ComponentTransform(GameObject* owner);
	~ComponentTransform();

	float4x4 getGlobalMatrix();
	float4x4 getLocalMatrix();

	void resetMatrix();

	float3 getGlobalRotation();
	
	float3 getPosition(bool globalPosition = false);
	void setPosition(float3 pos);
	float3 getRotation();
	void setRotation(float3 rot);
	float3 getScale();
	void setScale(float3 sca);

	void PrintInspector();

	void calculateMatrix();
	void setOffset();

public: 
	std::vector<AffectedCollider*> affectedCollidersList;

private:
	float4x4 matrix;
	float3 position;
	float3 rotation;
	float3 scale;
};
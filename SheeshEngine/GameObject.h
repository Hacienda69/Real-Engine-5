#pragma once
#include "Module.h"
#include <vector>
#include <string>


enum class ComponentType;
class Component;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;
class GameObject
{
public:

	GameObject();
	GameObject(GameObject* parent);
	~GameObject();

	void AddComponent(ComponentType type);
	void Update();
	GameObject* GetParent();
	bool SetNewParent(GameObject* newParent);
	void PrintInspector();
	bool CheckChildOf(GameObject* parent);
	void DeleteChild(GameObject* child);


	ComponentMesh* GetMeshComponent();

	ComponentMaterial* GetComponentTexture();


	std::string name;

	GameObject* mParent;
	std::vector<GameObject*> mChildren;
	std::vector<Component*> mComponents;

	ComponentTransform* transform;

};
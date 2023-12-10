#pragma once
#include "Module.h"
#include <vector>
#include <string>

#include "MathGeoLib/include/MathGeoLib.h"

enum class ComponentType;
enum class GeometryType;
class Component;
class ComponentTransform;
class ComponentMesh;
class ComponentMaterial;
class ComponentCamera;

class GameObject
{
public:

	GameObject();
	GameObject(GameObject* parent);
	~GameObject();

	void AddComponent(Component* component);
	void Update();
	GameObject* GetParent();
	bool SetNewParent(GameObject* newParent);
	bool GameObject::SetAsChildOf(GameObject* gameObject);
	void PrintInspector();
	bool CheckChildOf(GameObject* parent);
	void DeleteChild(GameObject* child);


	ComponentMesh* GetMeshComponent();

	ComponentTransform* GetTransformComponent();

	ComponentMaterial* GetTextureComponent();

	ComponentCamera* GetCameraComponent();

	const std::vector<GameObject*>& GetChildren() const;

	bool isTimetoDelete;
	std::string name;

	GameObject* mParent;
	std::vector<Component*> mComponents;

	ComponentTransform* transform;
	GeometryType Stype;

	int newComponent = 0; // 1 = Mesh, 2 = Texture, 3 = Camera

	std::vector<GameObject*> mChildren;
};
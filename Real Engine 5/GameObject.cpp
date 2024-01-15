#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "ComponentCollider.h"
#include "OurPrimitive.h"
#include "ModuleInput.h"

// CONSTRUCTORS & DESTRUCTOR -----------------------------------------------------
GameObject::GameObject()
{
	name = "GameObject";
	mParent = nullptr;
	transform = new ComponentTransform(this);
	isTimetoDelete = false;
	Stype = GeometryType::NONE;
	mComponents.push_back(transform);

	App->scene->gameObjects.push_back(this);
}

GameObject::GameObject(GameObject* parent)
{
	name = "GameObject";
	this->mParent = parent;
	isTimetoDelete = false;
	Stype = GeometryType::NONE;

	if (parent != nullptr)
	{
		parent->mChildren.push_back(this);
	}

	transform = new ComponentTransform(this);
	

	mComponents.push_back(transform);
}

GameObject::~GameObject()
{
	for (int i = 0; i < App->scene->gameObjects.size(); i++)
	{
		if (App->scene->gameObjects[i] == this) App->scene->gameObjects.erase(App->scene->gameObjects.begin() + i);
	}

	name = "";
	transform = nullptr;

	if (isTimetoDelete && mParent != nullptr) {
		mParent->DeleteChild(this);
	}


	for (size_t i = 0; i < mComponents.size(); ++i)
	{
		delete mComponents[i];
		mComponents[i] = nullptr;
	}

	for (size_t i = 0; i < mChildren.size(); ++i)
	{
		delete mChildren[i];
		mChildren[i] = nullptr;
	}

	mComponents.clear();
}

// UPDATE ---------------------------------------------------------------------------
void GameObject::Update()
{
	// No tots els children o components tenen un update
	for (size_t i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->Update();
	}

	for (size_t i = 0; i < mComponents.size(); ++i)
	{
		mComponents[i]->Update();
	}
}

// ADDCOMPONENT ---------------------------------------------------------------------
void GameObject::AddComponent(Component* component)
{
	component->mOwner = this;
	mComponents.push_back(component);
}

// CHILDS / PARENTS -----------------------------------------------------------------
void GameObject::DeleteChild(GameObject* child)
{
	for (int i = 0; i < mChildren.size(); i++) {
		if (mChildren[i] == child) {
			mChildren.erase(mChildren.begin() + i);
			child->mParent = nullptr;
		}
	}
}

bool GameObject::CheckChildOf(GameObject* parent)
{
	if (parent == this)
	{
		return true;
	}
	if (parent->mChildren.empty())
	{
		return false;
	}
	for (size_t i = 0; i < parent->mChildren.size(); i++)
	{
		if (CheckChildOf(parent->mChildren[i])) return true;
	}
	return false;
}

bool GameObject::SetNewParent(GameObject* newParent)
{
	if (mParent != nullptr) {
		if (newParent->CheckChildOf(this)) return false;

		mParent->DeleteChild(this);
	}

	mParent = newParent;
	newParent->mChildren.push_back(this);

	return true;
}

bool GameObject::SetAsChildOf(GameObject* gameObject)
{
	if (CheckChildOf(gameObject))
	{
		return false;
	}
	gameObject->mParent = this;
	mChildren.push_back(gameObject);
	return true;
}

// PRINT INSPECTOR -------------------------------------------------------------------------------
void GameObject::PrintInspector()
{
	char* listComponents[] { "Add Component", "Mesh Component", "Material Component", "Camera Component", "Collider Component"};
	char aux[255] = { ' ' }; 

	if (mParent != nullptr)
	{
		strcpy(aux, this->name.c_str());


		ImGui::BulletText("Name:");
		ImGui::SameLine();

		
		ImGui::InputText("##Name", aux, 255, ImGuiInputTextFlags_EnterReturnsTrue);

		if (ImGui::IsKeyDown(ImGuiKey_Enter))
			name = aux;


		if (ImGui::Button("Delete")) {
			
			isTimetoDelete = true;
			delete App->hierarchy->objSelected;
			App->hierarchy->objSelected = nullptr;
		}

		for (size_t i = 0; i < mComponents.size(); i++)
		{
			ImGui::Separator();

			mComponents[i]->PrintInspector();
		}

		ImGui::Separator();
		ImGui::Text("");
		ImGui::Text("");
		ImGui::Text("");

		ImGui::Text("");
		ImGui::SameLine(ImGui::GetWindowWidth() / 6);

		if (ImGui::Combo("##Add Component", &newComponent, listComponents, IM_ARRAYSIZE(listComponents)))
		{
			switch (newComponent) {
			case 1: // Mesh
			{
				if (GetMeshComponent() == nullptr) {
					ComponentMesh* cm = new ComponentMesh();
					AddComponent(cm);
				}
				else LOG("Mesh Component already added")
			}
			break;
			case 2: // Material
			{
				//if (GetTextureComponent() == nullptr) {
				//	ComponentMaterial* ct = new ComponentMaterial();
				//	AddComponent(ct);
				//}
				//else LOG("Texture Component already added")
			}
			break;
			case 3: // Camera
			{
				if (GetCameraComponent() == nullptr) {
					ComponentCamera* cc = new ComponentCamera();
					AddComponent(cc);
					App->scene->sceneCameras.push_back(cc);
				}
				else LOG("Camera Component already added")
			}
			break;
			case 4: // Collider
			{
				ComponentCollider* cc = new ComponentCollider();
				AddComponent(cc);
			}
			break;

			newComponent = 0;
			}
		}

	}
	//ImGui::End();
}

// GETTERS ----------------------------------------------------------------
GameObject* GameObject::GetParent()
{
	return mParent;
}

ComponentMesh* GameObject::GetMeshComponent()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i]->type == ComponentType::MESH)
		{
			return (ComponentMesh*)mComponents[i];
		}
	}
	return nullptr;
}

ComponentTransform* GameObject::GetTransformComponent()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i]->type == ComponentType::TRANSFORM)
		{
			return (ComponentTransform*)mComponents[i];
		}
	}
	return nullptr;
}

ComponentMaterial* GameObject::GetTextureComponent()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i]->type == ComponentType::MATERIAL) {

			return (ComponentMaterial*)mComponents[i];
		}
	}

	return nullptr;
}

ComponentCamera* GameObject::GetCameraComponent()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i]->type == ComponentType::CAMERA) {

			return (ComponentCamera*)mComponents[i];
		}
	}

	return nullptr;
}

const std::vector<GameObject*>& GameObject::GetChildren() const {
	return mChildren;
}
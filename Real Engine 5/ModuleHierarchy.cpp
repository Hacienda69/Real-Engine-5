#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleAssimpMeshes.h"
#include "ModuleHierarchy.h"
#include "ModuleRenderer3D.h"
#include"ModuleScene.h"

#include "Primitive.h"

ModuleHierarchy::ModuleHierarchy(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	TargetDropped = nullptr;
	objSelected = nullptr;
	
}

ModuleHierarchy::~ModuleHierarchy()
{}

//-----------------------------------------------------------------
bool ModuleHierarchy::Start()
{
	
	bool ret = true;


	return ret;
}

// -----------------------------------------------------------------
bool ModuleHierarchy::CleanUp()
{
	LOG("Cleaning Hierarchy");
	
	return true;
}

// -----------------------------------------------------------------
update_status ModuleHierarchy::PreUpdate(float dt)
{


	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModuleHierarchy::Update(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		if (objSelected != nullptr) {
			objSelected->isTimetoDelete = true;
		}
		delete objSelected;
		objSelected = nullptr;
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModuleHierarchy::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}



void ModuleHierarchy::GameObjectTree(GameObject* obj, int index)
{
	ImGuiTreeNodeFlags flag_TNode = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow;

	bool isSelected;

	if (obj->GetParent() == nullptr) {
		flag_TNode |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	else {
		flag_TNode |= ImGuiTreeNodeFlags_OpenOnArrow;
	}

	if (obj == objSelected) {
		flag_TNode |= ImGuiTreeNodeFlags_Selected;
	}

	if (obj->mChildren.size() != 0)
		isSelected = ImGui::TreeNodeEx((void*)(intptr_t)index, flag_TNode, obj->name.c_str(), index);

	else {
		flag_TNode |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)index, flag_TNode, obj->name.c_str(), index);
		isSelected = false;
	}
	if (obj->GetParent() != nullptr) {

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("GameObject", obj, sizeof(GameObject*));

			TargetDropped = obj;
			ImGui::Text("Changing...");
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			objHovered = obj;
			if (ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				SetGameObject(obj);
			}
		}

	}

	if (ImGui::IsWindowHovered())
	{
		if (!ImGui::IsAnyItemHovered())
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				objSelected = nullptr;
			}
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject")) {

			
				TargetDropped->SetNewParent(objHovered);
			
		}
		ImGui::EndDragDropTarget();
	}

	if (isSelected)
	{
		if (!obj->mChildren.empty()) {
			for (unsigned int i = 0; i < obj->mChildren.size(); i++)
			{
				GameObjectTree(obj->mChildren[i], i);
			}
		}
		ImGui::TreePop();
	}

	if (obj == objSelected) {
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
	}

	// Rest of your existing code for tree node rendering

	// Restore style if not selected
	if (obj == objSelected) {
		ImGui::PopStyleColor(2);
	}

}

void ModuleHierarchy::SetGameObject(GameObject* obj)
{
	objSelected = obj;
}

// Find by name & root
GameObject* ModuleHierarchy::findByName(GameObject* currentNode, const std::string& name)
{
	if (currentNode == nullptr) {
		return nullptr;  // No se encontr� el GameObject
	}

	if (currentNode->name == name) {
		return currentNode;  // Se encontr� el GameObject
	}

	// Explora los nodos hijos de manera recursiva
	for (GameObject* child : currentNode->GetChildren()) {
		GameObject* result = findByName(child, name);
		if (result != nullptr) {
			return result;  // Si se encuentra en alg�n nodo hijo, devuelve el resultado
		}
	}

	return nullptr;  // No se encontr� en este sub�rbol
}

// Find by name
GameObject* ModuleHierarchy::findByName(const std::string& name)
{
	// Inicia la b�squeda desde el nodo ra�z del �rbol
	return findByName(App->scene->root, name);
}
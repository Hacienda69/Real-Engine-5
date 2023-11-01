
#pragma once
#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial() : Component(nullptr)
{

}
ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(owner)
{
	type = ComponentType::MATERIAL;
	mOwner = owner;
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::SetTexture(const char* path)
{
	/*ResetTexture();*/
	if (textureID != 0)
	{
		App->textures->CleanTexture(textureID);
		textureID = 0;
	}
	//if (mOwner->GetParent()->mChildren[0]->GetComponentTexture() != nullptr) {
	//	int a = mOwner->GetParent()->mChildren[0]->GetComponentTexture()->textureID;
	//	int b = mOwner->GetParent()->mChildren[1]->GetComponentTexture()->textureID;
	//}

	textureID = App->textures->LoadTexture(path);

	//int c = mOwner->GetParent()->mChildren[0]->GetComponentTexture()->textureID;
	//int x = mOwner->GetParent()->mChildren[1]->GetComponentTexture()->textureID;
	UpdateMeshTexture();
}

void ComponentMaterial::ResetTexture()
{
	if (textureID !=0)
	{
		App->textures->CleanTexture(textureID);
		textureID = 0;
	}

	UpdateMeshTexture();
}

void ComponentMaterial::UpdateMeshTexture()
{
	ComponentMesh* componentMesh = mOwner->GetMeshComponent();

	if (componentMesh == nullptr)
		return;
	 
	if (textureSelected == 0)
	{
		componentMesh->mesh->id_texture = textureID;
		
		return;
	}

	if (textureSelected == 2)
	{
		componentMesh->mesh->id_texture = App->textures->checkersID;
		return;
	}
	
	componentMesh->mesh->id_texture = -1;

}

void ComponentMaterial::PrintInspector()
{
	const char* listTextures[]{ "Texture", "None", "Checkes" };

	//Texture component inspector
	if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGui::Text("Texture: ");
		ImGui::SameLine();
		ImGui::Combo("##ChoseTexture", &textureSelected, listTextures, IM_ARRAYSIZE(listTextures));

	}

	UpdateMeshTexture();

}

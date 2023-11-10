
#pragma once
#include "ComponentMaterial.h"
#include <string>

ComponentMaterial::ComponentMaterial() : Component(nullptr)
{
	type = ComponentType::MATERIAL;
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
	if (textureID != 0)
	{
		App->textures->CleanTexture(textureID);
		textureID = 0;
	}

	pathTexture = path;
	textureID = App->textures->LoadTexture(path);


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

	
	if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGui::Text("Path: "); ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, pathTexture.c_str());

		int width = mOwner->GetMeshComponent()->mesh->texture_width;
		int height = mOwner->GetMeshComponent()->mesh->texture_height;

		ImGui::Text("Texture Width: ");  ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, std::to_string(width).c_str());
		ImGui::Text("Texture Height: "); ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, std::to_string(height).c_str());

		ImGui::Image((ImTextureID)textureID, ImVec2(128, 128));

		ImGui::Text("Texture: ");
		ImGui::SameLine();
		ImGui::Combo("##ChoseTexture", &textureSelected, listTextures, IM_ARRAYSIZE(listTextures));
	}

	UpdateMeshTexture();

}

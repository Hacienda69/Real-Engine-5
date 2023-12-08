#include "ComponentMesh.h"

ComponentMesh::ComponentMesh() : Component(nullptr)
{
	type = ComponentType::MESH;
	mesh = nullptr;

	faceNormals = false;
}
ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	type = ComponentType::MESH;
	mOwner = owner;
	mesh = nullptr;

	faceNormals = false;
}

ComponentMesh::ComponentMesh::~ComponentMesh()
{
	App->assimpMeshes->DeleteMesh(mesh);
	mesh = nullptr;
}

void ComponentMesh::PrintInspector()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		if (mesh == nullptr) return;
		ImGui::LabelText("##%f", "Number of vertex:");
		ImGui::SameLine();
		ImGui::TextColored(IMGUICOL_CYAN, "%d", mesh->vertexCount);
		ImGui::Checkbox("Face Normals", &faceNormals); ImGui::SameLine(); ImGui::Checkbox("AABB", &mesh->showAABB); ImGui::SameLine(); ImGui::Checkbox("OBB", &mesh->showOBB);
	}
}

Mesh* ComponentMesh::getMesh() {
	return mesh;
}
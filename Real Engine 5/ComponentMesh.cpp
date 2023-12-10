#include "ComponentMesh.h"

ComponentMesh::ComponentMesh() : Component(nullptr)
{
    type = ComponentType::MESH;

    faceNormals = false;
}
ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
    type = ComponentType::MESH;
    mOwner = owner;

    faceNormals = false;
}

ComponentMesh::ComponentMesh::~ComponentMesh()
{
    for (int i = 0; i < meshes.size(); i++)
    {
        App->assimpMeshes->DeleteMesh(meshes[i]);
    }
    meshes.clear();
}

void ComponentMesh::PrintInspector()
{
    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        if (meshes.empty()) return;
        for (int i = 0; i < meshes.size(); i++)
        {
            ImGui::LabelText("##%f", "Number of vertex:");
            ImGui::SameLine();
            ImGui::TextColored(IMGUICOL_CYAN, "%d", meshes[i]->vertexCount);
            ImGui::Checkbox("Face Normals", &faceNormals); ImGui::SameLine(); ImGui::Checkbox("AABB", &meshes[i]->showAABB); ImGui::SameLine(); ImGui::Checkbox("OBB", &meshes[i]->showOBB);
        }
    }
}

std::vector<Mesh*> ComponentMesh::getMeshes()
{
    return meshes;
}
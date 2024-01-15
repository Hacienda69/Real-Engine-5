#include "Application.h"
#include "Globals.h"
#include "ComponentCollider.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Component;
class GameObject;

ComponentCollider::ComponentCollider() : Component(nullptr)
{
	type = C_BOX;
}

ComponentCollider::ComponentCollider(GameObject* owner) : Component(owner)
{
	type = C_BOX;
}

ComponentCollider::~ComponentCollider() 
{

}

void ComponentCollider::Update() 
{

}

void ComponentCollider::PrintInspector() 
{
    const char* colType[] = { "Box", "Sphere", "Cylinder"};

    if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        if (type = C_BOX)       { ImGui::SameLine(); ImGui::Text(" ( Box Collider ) "); }
        if (type = C_SPHERE)    { ImGui::SameLine(); ImGui::Text(" ( Sphere Collider ) "); }
        if (type = C_CYLINDER)  { ImGui::SameLine(); ImGui::Text(" ( Cylinder Collider ) "); }

        if (ImGui::Combo("Collider Type", &colTypeIdentifier, colType, IM_ARRAYSIZE(colType)))
        {
            if (colTypeIdentifier == 0) type = CollType::C_BOX;
            if (colTypeIdentifier == 1) type = CollType::C_SPHERE;
            if (colTypeIdentifier == 3) type = CollType::C_CYLINDER;
        }
    }
}
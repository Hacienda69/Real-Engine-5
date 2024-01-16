#include "Globals.h"
#include "ComponentCollider.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "PhysBody3D.h"


ComponentCollider::ComponentCollider() : Component(nullptr)
{
    type = NONE;
    isTrigger = false;
    radius = 1.5f;
    boxSize = float3(3, 3, 3);
    cylinderShape = float2(radius, boxSize.y);

    collider = nullptr;
}

ComponentCollider::ComponentCollider(GameObject* owner) : Component(owner)
{
    type = NONE;
    isTrigger = false;
    radius = 1.5f;
    boxSize = float3(3, 3, 3);
    cylinderShape = float2(radius, boxSize.y);

    collider = nullptr;
}

ComponentCollider::~ComponentCollider()
{

}

void ComponentCollider::Update()
{
    // Lógica de actualización del collider si es necesario
}

void ComponentCollider::PrintInspector()
{
    const char* colType[] = { "Box", "Sphere", "Cylinder", "None" };

    if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        if (type == CollType::C_BOX)      { ImGui::SameLine(); ImGui::Text(" ( Box Collider ) "); }
        if (type == CollType::C_SPHERE)   { ImGui::SameLine(); ImGui::Text(" ( Sphere Collider ) "); }
        if (type == CollType::C_CYLINDER) { ImGui::SameLine(); ImGui::Text(" ( Cylinder Collider ) "); }
        if (type == CollType::NONE)       { ImGui::SameLine(); ImGui::Text(" ( No Collider ) "); }

        if (ImGui::Checkbox("isTrigger", &isTrigger)) 
        {
            // Configurar el RigidBody como un sensor si es necesario
        }
         
        if (ImGui::Combo("Collider type", reinterpret_cast<int*>(&type), colType, IM_ARRAYSIZE(colType)));

        switch (type) 
        {
        case CollType::C_BOX:
            if (collider == nullptr) {
                SetBoxCollider();
            }
            else {
                App->physics->RemoveBody(collider);
                SetBoxCollider();
            }
            break;
        case CollType::C_SPHERE:
            if (collider == nullptr) {
                SetSphereCollider();
            }
            else {
                App->physics->RemoveBody(collider);
                SetSphereCollider();
            }
            break;
        case CollType::C_CYLINDER:
            if (collider == nullptr) {
                SetCylinderCollider();
            }
            else {
                App->physics->RemoveBody(collider);
                SetCylinderCollider();
            }
            break;
        case CollType::NONE:
            if (collider != nullptr) App->physics->RemoveBody(collider);
        }

        switch (type)
        {
        case ComponentCollider::C_BOX: 
            ImGui::Text("Box size");
            if (ImGui::DragFloat3("X / Y / Z", boxSize.ptr()))
            {
                boxCol.size.x = boxSize.x;  // Ancho (width)
                boxCol.size.y = boxSize.y;  // Altura (height)
                boxCol.size.z = boxSize.z;  // Profundidad (depth)

                cylinderShape = float2(radius, boxSize.y);
                cylinderCol.height = boxSize.y;
            }
            break;
        case ComponentCollider::C_SPHERE: 
            ImGui::Text("Sphere radius");
            if (ImGui::DragFloat("Radius", &radius)) 
            {
                sphereCol.radius = radius;

                cylinderShape = float2(radius, boxSize.y);
                cylinderCol.radius = radius;
            }
            break;
        case ComponentCollider::C_CYLINDER: 
            ImGui::Text("Cylinder shape");
            if (ImGui::DragFloat2("Radius, Height", cylinderShape.ptr()))
            {
                cylinderCol.radius = cylinderShape.x;
                cylinderCol.height = cylinderShape.y;

                sphereCol.radius = cylinderShape.x;
                boxCol.size.y = cylinderShape.y;
            }
            break;  
        case ComponentCollider::NONE:
            break; 
        default:
            break;
        }
    }
}

void ComponentCollider::SetBoxCollider() 
{   
    boxCol.size.x = boxSize.x;  // Ancho (width)
    boxCol.size.y = boxSize.y;  // Altura (height)
    boxCol.size.z = boxSize.z;  // Profundidad (depth)

    ComponentTransform* transform = App->hierarchy->objSelected->GetTransformComponent();
    if (transform) {
        float3 pos = transform->getPosition();
        boxCol.SetPos(pos.x / 2, pos.y / 2 + 1, pos.z / 2);
    }
    collider = App->physics->AddBody(boxCol, 0);
}

void ComponentCollider::SetSphereCollider() 
{
    sphereCol.radius = radius; 
    ComponentTransform* transform = App->hierarchy->objSelected->GetTransformComponent(); 
    if (transform) {
        float3 pos = transform->getPosition();
        sphereCol.SetPos(pos.x / 2, pos.y / 2 + 1, pos.z / 2);
    }
    collider = App->physics->AddBody(sphereCol, 0);
}

void ComponentCollider::SetCylinderCollider() 
{
    cylinderCol.radius = cylinderShape.x;
    cylinderCol.height = cylinderShape.y;

    ComponentTransform* transform = App->hierarchy->objSelected->GetTransformComponent(); 
    if (transform) { 
        float3 pos = transform->getPosition(); 
        cylinderCol.SetPos(pos.x / 2, pos.y / 2 + 1, pos.z / 2); 
    }
    collider = App->physics->AddBody(cylinderCol, 0);
}
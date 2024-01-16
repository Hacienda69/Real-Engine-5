#include "Globals.h"
#include "GameObject.h"
#include "ComponentPhysics.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "PhysBody3D.h"

ComponentPhysics::ComponentPhysics() : Component(nullptr)
{
    type = NONE;
    isTrigger = false;

    ComponentTransform* GoTransform = App->hierarchy->objSelected->GetTransformComponent();  
    boxSize.x = GoTransform->getScale().x;  
    boxSize.y = GoTransform->getScale().y;  
    boxSize.z = GoTransform->getScale().z;  
    radius = 1.5f;
    cylinderShape = float2(radius, boxSize.y); 

    colPos = { 0, 0, 0 };
    colRot = { 0, 0, 0 };
    colScl = { 1, 1, 1 };

    mass = 0.f;

    collider = nullptr;
}

ComponentPhysics::ComponentPhysics(GameObject* owner) : Component(owner)
{
    type = NONE; 
    isTrigger = false;

    ComponentTransform* GoTransform = App->hierarchy->objSelected->GetTransformComponent(); 
    boxSize.x = GoTransform->getScale().x; 
    boxSize.y = GoTransform->getScale().y; 
    boxSize.z = GoTransform->getScale().z; 
    radius = 1.5f;
    cylinderShape = float2(radius, boxSize.y); 

    colPos = { 0, 0, 0 };
    colRot = { 0, 0, 0 };
    colScl = { 1, 1, 1 };

    mass = 0.f;

    collider = nullptr;
}

ComponentPhysics::~ComponentPhysics()
{

}

void ComponentPhysics::Update()
{
    // Lógica de actualización del collider si es necesario

    if (mOwner == nullptr) return;

    if (isStatic) mass = 0.0f;
    else mass = 1.0f;
}

void ComponentPhysics::PrintInspector()
{
    const char* colType[] = { "Box", "Sphere", "Cylinder", "None" };

    if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
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

        // This is inefficient, colldiers are removed and re-added on every frame but it's the only way to update its position and rotation. Sorry :(
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
        case ComponentPhysics::C_BOX: 
            ImGui::Text("Box size");
            if (ImGui::DragFloat3("X / Y / Z", boxSize.ptr()))
            {

            }
            break;
        case ComponentPhysics::C_SPHERE: 
            ImGui::Text("Sphere radius");
            if (ImGui::DragFloat("Radius", &radius)) 
            {

            }
            break;
        case ComponentPhysics::C_CYLINDER: 
            ImGui::Text("Cylinder shape");
            if (ImGui::DragFloat2("Radius, Height", cylinderShape.ptr()))
            {

            }
            break;  
        case ComponentPhysics::NONE:
            break; 
        default:
            break;
        }
    }
}

void ComponentPhysics::SetBoxCollider() 
{   
    PrimCube box;
    toIdentity(box.transform);

    ComponentTransform* GoTransform = App->hierarchy->objSelected->GetTransformComponent();

    box.SetPos(colPos.x, colPos.y, colPos.z);
    boxSize.x = GoTransform->getScale().x;
    boxSize.y = GoTransform->getScale().y;
    boxSize.z = GoTransform->getScale().z;
    
    collider = App->physics->AddBody(box, mass);
    LinkToTransform();
}

void ComponentPhysics::SetSphereCollider() 
{ 
    ComponentTransform* GoTransform = App->hierarchy->objSelected->GetTransformComponent(); 
    float3 gRot = GoTransform->getGlobalRotation();

    float rx = gRot.x * DEGTORAD;
    float ry = gRot.y * DEGTORAD;
    float rz = gRot.z * DEGTORAD;

    Quat rotQuat = Quat::FromEulerXYZ(rx, ry, rz);
    vec3 rotVec(rotQuat.Axis().x, rotQuat.Axis().y, rotQuat.Axis().z);
    float rotAngle = rotQuat.Angle();

    PrimSphere sphere;

    sphere.SetPos(colPos.x, colPos.y, colPos.z);
    sphere.SetRotation(rotAngle, rotVec);

    sphere.radius = radius;

    collider = App->physics->AddBody(sphere, mass);
    LinkToTransform();
}

void ComponentPhysics::SetCylinderCollider() 
{
    ComponentTransform* GoTransform = App->hierarchy->objSelected->GetTransformComponent();
    float3 gRot = GoTransform->getGlobalRotation();

    float rx = gRot.x * DEGTORAD;
    float ry = gRot.y * DEGTORAD;
    float rz = gRot.z * DEGTORAD;

    Quat rotQuat = Quat::FromEulerXYZ(rx, ry, rz);
    vec3 rotVec(rotQuat.Axis().x, rotQuat.Axis().y, rotQuat.Axis().z);
    float rotAngle = rotQuat.Angle();

    PrimCylinder cylinder;

    cylinder.SetPos(colPos.x, colPos.y, colPos.z);
    cylinder.SetRotation(rotAngle, rotVec);
    cylinder.radius = 1.0f;
    cylinder.height = 1.0f;
    cylinder.color = Green;

    collider = App->physics->AddBody(cylinder, mass);
    LinkToTransform();
}

void ComponentPhysics::LinkToTransform()
{
    ComponentTransform* GoTransform = App->hierarchy->objSelected->GetTransformComponent();

    ComponentTransform::AffectedCollider* newRelation = new ComponentTransform::AffectedCollider();
    newRelation->collider = collider;
    mat4x4 newMat;
    newRelation->offset = newMat;

    GoTransform->affectedCollidersList.push_back(newRelation);
    GoTransform->setOffset();

    std::vector<GameObject*> mOwner_children = mOwner->GetChildren();

    for (int i = 0; i < mOwner_children.size(); i++) {
        ComponentTransform::AffectedCollider* newRel = new ComponentTransform::AffectedCollider();
        newRel->collider = collider;
        mat4x4 newMt;
        newRelation->offset = newMt;
        mOwner_children[i]->GetTransformComponent()->affectedCollidersList.push_back(newRel);
        mOwner_children[i]->GetTransformComponent()->setOffset();
    }
}

void ComponentPhysics::toIdentity(mat4x4 mat) 
{
    for (int i = 0; i < 15; ++i)
    {
        if (i == 0 || i == 5 || i == 10 || i == 15) mat[i] = 1;
        else mat[i] = 0;
    }
}
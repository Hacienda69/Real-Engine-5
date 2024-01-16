#include "Globals.h"
#include "GameObject.h"
#include "ComponentPhysics.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "PhysBody3D.h"
#include "Primitive.h"
#include "ModulePhysics.h"
#include "Module.h"

ComponentPhysics::ComponentPhysics() : Component(nullptr)
{
    type = ComponentType::PHYSICS;
    colliderType = CollType::NONE;
    isTrigger = false;

    radius = 1.5f;
    cylinderShape = float2(radius, 1);
    boxSize = float3(0, 0, 0);
    colPos = { 0, 0, 0 };
    colRot = { 0, 0, 0 };
    colScl = { 3, 3, 3 };

    mass = 0.f;

    phys = nullptr;
    p2pConstraint = nullptr;
    hingeConstraint = nullptr;
    isStatic = false;
    collider = nullptr;
}

ComponentPhysics::ComponentPhysics(GameObject* owner) : Component(owner)
{
    type = ComponentType::PHYSICS;
    colliderType = CollType::NONE;
    isTrigger = false;

    radius = 1.5f;
    cylinderShape = float2(radius, 1); 
    boxSize = float3(3, 3, 3);
    ComponentTransform* auxTransform = owner->GetTransformComponent();

    colPos = { 
        auxTransform->getPosition().x,
        auxTransform->getPosition().y,
        auxTransform->getPosition().z
    };
    colRot = { 0, 0, 0 };
    colScl = { 3, 3, 3 };

    mass = 0.f;

    phys = nullptr;
    p2pConstraint = nullptr;
    hingeConstraint = nullptr;
    isStatic = false;
    collider = nullptr;
}

ComponentPhysics::~ComponentPhysics()
{
    RemoveCollider();
    delete hingeConstraint;
    delete p2pConstraint;
    delete collider;
}

void ComponentPhysics::Update()
{
    if (isStatic) //sequeda
        App->physics->SetBodyMass(collider, 0);
    else
        App->physics->SetBodyMass(collider, mass);

    if (collider != nullptr) {
        float matrix[16];
        collider->GetTransform(matrix);
        mOwner->GetTransformComponent()->SetTransformFromMatrix(matrix);
    }
    if (mOwner == nullptr) return;
}

void ComponentPhysics::PrintInspector()
{
    const char* colType[] = { "Box", "Sphere", "Cylinder", "None" };

    if (ImGui::CollapsingHeader("Physics"))
    {
        if (colliderType == CollType::C_BOX) { ImGui::SameLine(); ImGui::Text(" ( Box Collider ) "); }
        if (colliderType == CollType::C_SPHERE) { ImGui::SameLine(); ImGui::Text(" ( Sphere Collider ) "); }
        if (colliderType == CollType::C_CYLINDER) { ImGui::SameLine(); ImGui::Text(" ( Cylinder Collider ) "); }
        if (colliderType == CollType::NONE) { ImGui::SameLine(); ImGui::Text(" ( No Collider ) "); }

        ImGui::Checkbox("Static\t", &isStatic);

        ImGui::SameLine();
        if (ImGui::Checkbox("Static", &isStatic))
        {
            if (isStatic) mass = 0;
            if (!isStatic) mass = 1;
            UpdateShape();
        }
        if (!isStatic)
        {
            if (ImGui::DragFloat("Mass", &mass)) { UpdateShape(); }
        }


        ImGui::Text("\n");

        if (ImGui::Combo("Collider type", reinterpret_cast<int*>(&colliderType), colType, IM_ARRAYSIZE(colType)))
        {
            switch (colliderType)
            {
            case CollType::C_BOX:
                if (collider == nullptr) {
                    SetBoxCollider();
                }
                else {
                    RemoveCollider();
                    SetBoxCollider();
                }
                break;
            case CollType::C_SPHERE:
                if (collider == nullptr) {
                    SetSphereCollider();
                }
                else {
                    RemoveCollider();
                    SetSphereCollider();
                }
                break;
            case CollType::C_CYLINDER:
                if (collider == nullptr) {
                    SetCylinderCollider();
                }
                else {
                    RemoveCollider();
                    SetCylinderCollider();
                }
                break;
            case CollType::NONE:
                if (collider != nullptr) RemoveCollider();
            }
        }

        switch (colliderType)
        {
        case CollType::C_BOX:
            if (ImGui::DragFloat3(" - Position", colPos.ptr())) { UpdateShape(); }
            if (ImGui::DragFloat3(" - Rotation", colRot.ptr())) { UpdateShape(); }
            if (ImGui::DragFloat3(" - Scale", colScl.ptr())) { UpdateShape(); }
            break;
        case CollType::C_SPHERE:
            if (ImGui::DragFloat3(" - Position", colPos.ptr())) { UpdateShape(); }
            if (ImGui::DragFloat3(" - Rotation", colRot.ptr())) { UpdateShape(); }
            if (ImGui::DragFloat(" - Radius", &radius)) { UpdateShape(); }
            break;
        case CollType::C_CYLINDER:
            if (ImGui::DragFloat3(" - Position", colPos.ptr())) { UpdateShape(); }
            if (ImGui::DragFloat3(" - Rotation", colRot.ptr())) { UpdateShape(); }
            if (ImGui::DragFloat2(" - Radius, Height", cylinderShape.ptr())) { UpdateShape(); }
            break;
        case CollType::NONE:
            break;
        default:
            break;
        }

        if (ImGui::Button("Remove Collider"))
        {
            RemoveCollider();
            colliderType = CollType::NONE;
        }

    }
    Update();
}

void ComponentPhysics::RemoveCollider() 
{
    if (collider != nullptr) {
        App->physics->RemoveBody(collider);
        collider = nullptr;
    }
}

void ComponentPhysics::SetBoxCollider() 
{   
    colliderType = CollType::C_BOX;

    PrimCube box;
    toIdentity(box.transform);

    box.SetPos(colPos.x / 2, colPos.y / 2 + 1, colPos.z / 2);
    box.size.x = colScl.x;
    box.size.y = colScl.y;
    box.size.z = colScl.z;

    box.color = Green;
    
    collider = App->physics->AddBody(box, mass);
}

void ComponentPhysics::SetSphereCollider() 
{ 
    colliderType = CollType::C_SPHERE;

    ComponentTransform* GoTransform = App->hierarchy->objSelected->GetTransformComponent(); 
    float3 gRot = GoTransform->getGlobalRotation();

    float rx = gRot.x * DEGTORAD;
    float ry = gRot.y * DEGTORAD;
    float rz = gRot.z * DEGTORAD;

    Quat rotQuat = Quat::FromEulerXYZ(rx, ry, rz);
    vec3 rotVec(rotQuat.Axis().x, rotQuat.Axis().y, rotQuat.Axis().z);
    float rotAngle = rotQuat.Angle();

    PrimSphere sphere;

    sphere.SetPos(colPos.x / 2, colPos.y / 2 + 1, colPos.z / 2);
    sphere.SetRotation(rotAngle, rotVec);

    sphere.radius = radius;

    sphere.color = Green;

    collider = App->physics->AddBody(sphere, mass);
}

void ComponentPhysics::SetCylinderCollider() 
{
    colliderType = CollType::C_CYLINDER;

    ComponentTransform* GoTransform = App->hierarchy->objSelected->GetTransformComponent();
    float3 gRot = GoTransform->getGlobalRotation();

    float rx = gRot.x * DEGTORAD;
    float ry = gRot.y * DEGTORAD;
    float rz = gRot.z * DEGTORAD;

    Quat rotQuat = Quat::FromEulerXYZ(rx, ry, rz);
    vec3 rotVec(rotQuat.Axis().x, rotQuat.Axis().y, rotQuat.Axis().z);
    float rotAngle = rotQuat.Angle();

    PrimCylinder cylinder;

    cylinder.SetPos(colPos.x / 2, colPos.y / 2 + 1, colPos.z / 2);
    cylinder.SetRotation(rotAngle, rotVec);
    cylinder.radius = 1.0f;
    cylinder.height = 1.0f;

    cylinder.color = Green;

    collider = App->physics->AddBody(cylinder, mass);
}

void ComponentPhysics::UpdateShape()
{
    RemoveCollider();

    float3 auxRot = colRot;

    float rx = auxRot.x * DEGTORAD;
    float ry = auxRot.y * DEGTORAD;
    float rz = auxRot.z * DEGTORAD;

    PrimCube box;
    PrimSphere sphere;
    PrimCylinder cylinder;

    switch (colliderType)
    {
    case CollType::C_BOX:

        box.transform[0] = cos(ry) * cos(rz);
        box.transform[1] = sin(ry) * sin(rx) * cos(rz) - (cos(rx) * sin(rz));
        box.transform[2] = sin(rx) * sin(rz) + sin(ry) * cos(rz) * cos(rx);
         
        box.transform[4] = cos(ry) * sin(rz);
        box.transform[5] = cos(rx) * cos(rz) + sin(ry) * sin(rz) * sin(rz);
        box.transform[6] = sin(ry) * sin(rz) * cos(rx) - sin(rx) * cos(rz);

        box.transform[8] = -sin(ry);
        box.transform[9] =  cos(ry) * sin(rx);
        box.transform[10] = cos(rx) * cos(ry);

        box.SetPos(colPos.x / 2, colPos.y / 2 + 1, colPos.z / 2);

        box.size.x = colScl.x; 
        box.size.y = colScl.y; 
        box.size.z = colScl.z; 
        
        collider = App->physics->AddBody(box, mass); 
        break;

    case CollType::C_SPHERE:

        sphere.transform[0] = cos(ry) * cos(rz);
        sphere.transform[1] = sin(ry) * sin(rx) * cos(rz) - (cos(rx) * sin(rz));
        sphere.transform[2] = sin(rx) * sin(rz) + sin(ry) * cos(rz) * cos(rx);

        sphere.transform[4] = cos(ry) * sin(rz);
        sphere.transform[5] = cos(rx) * cos(rz) + sin(ry) * sin(rz) * sin(rz);
        sphere.transform[6] = sin(ry) * sin(rz) * cos(rx) - sin(rx) * cos(rz);

        sphere.transform[8] = -sin(ry);
        sphere.transform[9] = cos(ry) * sin(rx);
        sphere.transform[10] = cos(rx) * cos(ry);

        sphere.SetPos(colPos.x / 2, colPos.y / 2 + 1, colPos.z / 2);
        sphere.radius = radius;

        collider = App->physics->AddBody(sphere, mass);
        break;

    case CollType::C_CYLINDER:

        cylinder.transform[0] = cos(ry) * cos(rz);
        cylinder.transform[1] = sin(ry) * sin(rx) * cos(rz) - (cos(rx) * sin(rz));
        cylinder.transform[2] = sin(rx) * sin(rz) + sin(ry) * cos(rz) * cos(rx);

        cylinder.transform[4] = cos(ry) * sin(rz);
        cylinder.transform[5] = cos(rx) * cos(rz) + sin(ry) * sin(rz) * sin(rz);
        cylinder.transform[6] = sin(ry) * sin(rz) * cos(rx) - sin(rx) * cos(rz);

        cylinder.transform[8] = -sin(ry);
        cylinder.transform[9] = cos(ry) * sin(rx);
        cylinder.transform[10] = cos(rx) * cos(ry);

        cylinder.SetPos(colPos.x / 2, colPos.y / 2 + 1, colPos.z / 2);
        cylinder.radius = cylinderShape.x;
        cylinder.height = cylinderShape.y;

        collider = App->physics->AddBody(cylinder, mass);

        break;
    default:
        break;
    }
}

void ComponentPhysics::CubeColliderCreate()
{
    cube.size = (3, 3, 3);
    transformComponent = App->hierarchy->objSelected->GetTransformComponent();
    if (transformComponent) {
        float3 pos = transformComponent->getPosition();
        cube.SetPos(pos.x / 2, pos.y / 2 / 4, pos.z / 2);
    }
    collider = App->physics->AddBody(cube, 1);
    mass = 1;
}

void ComponentPhysics::SphereColliderCreate()
{
    sphere.radius = 2;
    transformComponent = App->hierarchy->objSelected->GetTransformComponent();
    if (transformComponent) {
        float3 pos = transformComponent->getPosition();
        sphere.SetPos(pos.x / 2, pos.y / 2 / 2, pos.z / 2);
    }
    collider = App->physics->AddBody(sphere, 1);
    mass = 1;
}

void ComponentPhysics::toIdentity(mat4x4 mat) 
{
    for (int i = 0; i < 15; ++i)
    {
        if (i == 0 || i == 5 || i == 10 || i == 15) mat[i] = 1;
        else mat[i] = 0;
    }
}
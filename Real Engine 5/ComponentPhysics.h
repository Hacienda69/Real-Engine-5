#include "Globals.h"
#include "Application.h"
#include "Component.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "ModulePhysics.h"
#include "PhysBody3D.h"

#ifdef _DEBUG
#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

class ComponentPhysics : public Component
{
public:

    enum class CollType
    {
        C_BOX,
        C_SPHERE,
        C_CYLINDER,
        NONE,
    };

    enum class ConstraintType
    {
        P2P,
        HINGE,
        NONE,
    };

    ComponentPhysics();
    ComponentPhysics(GameObject* owner);
    ~ComponentPhysics();

    void Update();
    void PrintInspector();

    void Enable() { isActive = true; }
    void Disable() { isActive = false; }
    bool IsEnable() { return isActive; }

    void SetBoxCollider();
    void SetSphereCollider();
    void SetCylinderCollider();

    void RemoveCollider();
    void UpdateShape();

    void CubeColliderCreate();
    void SphereColliderCreate();

    void setMass(float m) { mass = m; }
    void setStatic(bool x) { isStatic = x; }

public:
    void toIdentity(mat4x4 mat);

private:
    ModulePhysics* phys;
    CollType type;
    CollType colliderType;
    PhysBody3D* collider;

    mat4x4 colliderMatrix;

    ComponentTransform* transformComponent;
    //int colTypeIdentifier; // Used in PrintInspector() / 0 = box, 1 = sphere, 2 = cylinder
    bool isTrigger;
    bool isStatic = true; // if true: mass = 0

    float2 cylinderShape; // For cylinder shape / [0] = radius, [1] = height
    float radius; // For sphere shape

    float mass;
    float3 boxSize;
    float3 auxColPos;

    PrimCube cube;
    PrimSphere sphere;
    PrimCylinder cylinder;

    float3 colPos;
    float3 colRot;
    float3 colScl;

    GameObject* GoConstraint;
    btTypedConstraint* p2pConstraint;
    btHingeConstraint* hingeConstraint;
};
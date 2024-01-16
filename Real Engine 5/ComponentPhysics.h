#include "Application.h"
#include "Component.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "ModulePhysics.h"

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
    enum CollType
    {
        C_BOX,
        C_SPHERE,
        C_CYLINDER,
        NONE,
    };

    ComponentPhysics();
    ComponentPhysics(GameObject* owner);
    ~ComponentPhysics();

    void Update();
    void PrintInspector();

    void SetBoxCollider();
    void SetSphereCollider();
    void SetCylinderCollider();

    void RemoveCollider();
    void UpdateShape();

    // --------------------------------------------------------------------------------------
    void toIdentity(mat4x4 mat);

private:
    CollType type;
    PhysBody3D* collider;

    mat4x4 colliderMatrix;

    //int colTypeIdentifier; // Used in PrintInspector() / 0 = box, 1 = sphere, 2 = cylinder
    bool isTrigger;
    bool isStatic; // if true: mass = 0

    float2 cylinderShape; // For cylinder shape / [0] = radius, [1] = height
    float radius; // For sphere shape

    float mass;

    float3 auxColPos;

    float3 colPos;
    float3 colRot;
    float3 colScl;
};
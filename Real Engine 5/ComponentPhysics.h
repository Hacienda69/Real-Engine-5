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

    enum class CollType
    {
        C_BOX,
        C_SPHERE,
        NONE,
    };

    enum class ConstraintType
    {
        P2PCONSTRAINT,
        HINGECONSTRAINT,
        NONE,
    };

    ComponentPhysics();
    ComponentPhysics(GameObject* owner);
    ~ComponentPhysics();

public:
    void toIdentity(mat4x4 mat);

private:
    CollType type;
    CollType colliderType;
    ConstraintType constraintType;
    PhysBody3D* collider;

    mat4x4 colliderMatrix;
    float3 colPos;
    float3 colRot;
    float3 colScl;

    GameObject* GoConstraint;
    btTypedConstraint* p2pConstraint;
    btHingeConstraint* hingeConstraint;
};
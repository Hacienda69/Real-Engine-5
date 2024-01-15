#include "Component.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Component;
class GameObject;

enum CollType
{
	C_BOX,
	C_SPHERE,
	C_CYLINDER,
};

class ComponentCollider : public Component
{
public:
	ComponentCollider();
	ComponentCollider(GameObject* owner);
	~ComponentCollider();

	void Update();
	void PrintInspector();

public:

	CollType type;

private:

	int colTypeIdentifier; // Used in PrintInspector() / 0 = box, 1 = sphere, 3 = cylinder
	int width, height, depth; // For collider shape
};

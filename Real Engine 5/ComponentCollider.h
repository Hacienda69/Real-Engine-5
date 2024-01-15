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

public:

	void Update();
	void PrintInspector();

private:

	CollType type;
	int colTypeIdentifier; // Used in PrintInspector() / 0 = box, 1 = sphere, 3 = cylinder

};

#pragma once

class Mesh;
class GameObject;

enum class GeometryType {
	EMPTY,
	CAMERA,
	P_PLANE,
	P_CUBE,
	P_SPHERE,
	P_CYLINDER,
	P_SUZANNE,
	P_ICOSHPERE,
	P_CONE,
	P_TORUS,
	NONE
};

class PrimitivesGeomtriesLibrary
{
public:
	static GameObject* InstanciatePrimitiveGeometry(GeometryType type);

private:

};


//holi : )

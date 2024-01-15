#include "OurPrimitive.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"

GameObject* PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType type)
{
	
	if (type == GeometryType::EMPTY)
	{
        GameObject* gameObject = new GameObject(App->scene->root);
		gameObject->name = "Empty Object";
        gameObject->Stype = GeometryType::EMPTY;
        App->hierarchy->objSelected = gameObject;
		return gameObject;
	}
    else if (type == GeometryType::CAMERA)
    {
        GameObject* gameObject = new GameObject(App->scene->root);
        gameObject->name = "Camera";
        gameObject->Stype = GeometryType::CAMERA;
        App->hierarchy->objSelected = gameObject;

        ComponentCamera* cam = new ComponentCamera(gameObject);

        gameObject->AddComponent(cam);
        App->scene->sceneCameras.push_back(cam);

        return gameObject;
    }
    else
    {
        const char* aux = " ";
        GameObject* gameObject = new GameObject();
        switch (type)
        {
        case GeometryType::P_PLANE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/plane.fbx");
            aux = "Plane";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_PLANE;
            App->hierarchy->objSelected = gameObject;
            return gameObject;
            break;
        case GeometryType::P_CUBE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cube.fbx");
            aux = "Cube";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CUBE;
            App->hierarchy->objSelected = gameObject;
            return gameObject;

            break;
        case GeometryType::P_CYLINDER:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cylinder.fbx");
            aux = "Cylinder";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CYLINDER;
            App->hierarchy->objSelected = gameObject;
            return gameObject;
            break;
        case GeometryType::P_SPHERE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/sphere.fbx");
            aux = "Sphere";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_SPHERE;
            App->hierarchy->objSelected = gameObject;
            return gameObject;
            break;

        case GeometryType::P_ICOSHPERE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/icosphere.fbx");
            aux = "Icosphere";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_ICOSHPERE;
            App->hierarchy->objSelected = gameObject;
            return gameObject;
            break;

        case GeometryType::P_CONE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cone.fbx");
            aux = "Cone";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CONE;
            App->hierarchy->objSelected = gameObject;
            return gameObject;
            break;

        case GeometryType::P_TORUS:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/torus.fbx");
            aux = "Torus";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_TORUS;
            App->hierarchy->objSelected = gameObject;
            return gameObject;
            break;
        case GeometryType::P_SUZANNE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/suzanne.fbx");
            aux = "Suzanne";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_SUZANNE;
            App->hierarchy->objSelected = gameObject;
            return gameObject;
            break;

        default:

            return nullptr;

        }
    }
}
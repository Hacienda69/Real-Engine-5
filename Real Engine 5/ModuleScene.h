#pragma once
#include "Timer.h"
#include "Module.h"
#include "Globals.h"
#include "ComponentCamera.h"
#include "ImGui/imgui.h"
#include "Primitive.h"
#include "p2DynArray.h"

struct PhysBody3D;
struct PhysMotor3D;

class GameObject;

struct Constraints
{
	p2DynArray<PhysBody3D*>		phys_constraints;
	p2DynArray<PrimCube*>		prim_constraints;
};

class ModuleScene : public Module
{
public:

	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void Play();
	void Stop();
	void Pause();

	// What happens when play is pressed
	void PlayEvent();

	bool CleanUp();

	void SceneWindow(bool& active);
	void GameWindow(bool& active);

	void setMainCamera(ComponentCamera* cam);

	GameObject* GetRoot() const { return root; }

	ImVec2 NormalizeMouse(float x, float y, float w, float h, ImVec2 pos);

	void SetSelectedByTriangle(LineSegment ray, std::vector<GameObject*> GoList);

public:

	ImVec2 sizeScreen;

	Timer timerGameScene;
	bool isStop = false;
	bool isPlay = false;
	int gameState = 0;
	float deltaTime = 0;

	float counter = 0.f;

	std::vector<GameObject*> gameObjects;
	std::vector<ComponentCamera*> sceneCameras;
	ComponentCamera* mainCamera;

	std::string filePath;
	std::string texPath;

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* root = nullptr;
	GameObject* streetEnviroment;
	GameObject* bakerHouse;
	GameObject* defaultCamera;

	Constraints constraints;

	PhysBody3D* pb_chassis;
	PrimCube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;
};
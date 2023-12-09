#pragma once
#include "Timer.h"
#include "Module.h"
#include "Globals.h"
#include "ComponentCamera.h"
#include "ImGui/imgui.h"

class GameObject;

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
	float regulator;

	float rotatingScene;

	std::vector<GameObject*> gameObjects;
	std::vector<ComponentCamera*> sceneCameras;
	ComponentCamera* mainCamera;

	GameObject* defaultCamera;

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* root = nullptr;
	GameObject* prova1;
	GameObject* prova2;
	GameObject* prova3;
	GameObject* prova4;
	GameObject* prova5;
	GameObject* prova6;
};
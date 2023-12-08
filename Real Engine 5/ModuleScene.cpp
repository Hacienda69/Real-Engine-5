#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ModuleEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl2.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
}

ModuleScene::~ModuleScene() 
{
    gameObjects.clear();
    sceneCameras.clear();
}

bool ModuleScene::Init()
{
    return false;
}

bool ModuleScene::Start() {
    
    root = new GameObject(nullptr);
    //HOLACARACOLA root->name = ("Scene");
    regulator = 1.0f;
    //Load Baker House
    App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
   
    mainCamera = nullptr;

    return true;
}


update_status ModuleScene::PreUpdate(float dt) {
    
    switch (gameState) {
        case 1: 
            deltaTime = (float)timerGameScene.Read() / 690.0f;
            timerGameScene.Stop();
            timerGameScene.Start();
            //TODO cubo rotando
            break;
        case 2:
            timerGameScene.Stop();
            //TODO hacer que el cubo deje de rotar
            deltaTime = 0;
            isPlay = false;
            break;
        case 3:	
            isPlay = false;
            break;
    }
    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {

    //LOG("GameObjects: %d", gameObjects.size());
    //LOG("SceneCameras: %d", sceneCameras.size());
    rotatingScene = 1;

    regulator += deltaTime;

    for (int i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i]->Update();
    }

    for (int i = 0; i < sceneCameras.size(); i++)
    {
        sceneCameras[i]->Update();
        if (sceneCameras[i]->mainCam) mainCamera = sceneCameras[i];
    }
    
    return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp() {
    
    LOG("Scene Cleanup")
    delete root;
    return true;
}

void ModuleScene::SceneWindow() {
    ImGui::Begin("Scene");

    sizeScreen = ImGui::GetContentRegionAvail();

    float aspectRatio = sizeScreen.x / sizeScreen.y;
    App->camera->sceneCamera->frustum.verticalFov = App->camera->sceneCamera->fov * DEGTORAD;
    App->camera->sceneCamera->frustum.horizontalFov = 2.0f * atanf(tanf(App->camera->sceneCamera->frustum.verticalFov / 2.0f) * aspectRatio);

    ImGui::Image((ImTextureID)App->camera->sceneCamera->GetCamBuffer(), sizeScreen, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

void ModuleScene::GameWindow() {
    ImGui::Begin("Game");
    sizeScreen = ImGui::GetContentRegionAvail();
    float aspectRatio = sizeScreen.x / sizeScreen.y;
    mainCamera->frustum.verticalFov = mainCamera->fov * DEGTORAD;
    mainCamera->frustum.horizontalFov = 2.0f * atanf(tanf(mainCamera->frustum.verticalFov / 2.0f) * aspectRatio);
    ImGui::Image((ImTextureID)mainCamera->GetCamBuffer(), sizeScreen, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

void ModuleScene::Play()
{
    if (isPlay == false) {
        LOG("User is in game");
        isPlay = true;
        gameState = 1;
    }
    else if (isPlay == true) {
        LOG("User is NOT in game");
        gameState = 2;
        deltaTime = 0;
        timerGameScene.Stop();
        isPlay = false;
    }
}

void ModuleScene::Stop() {
    deltaTime = 0;
    ImGui::SetWindowFocus("Editor");
    gameState = 2;
}

void ModuleScene::Pause() {
    if (isPlay == true) {
        gameState = 3;
        isPlay = false;
    }
    else {
        gameState = 1;
        isPlay = true;
    }
}

ImVec2 ModuleScene::NormalizeMouse(float x, float y, float w, float h, ImVec2 pos) {
    ImVec2 normalizedPos;
    normalizedPos.x = -1.0 + 2.0 * ((pos.x - x) / w);
    normalizedPos.y = 1.0 - 2.0 * ((pos.y - y) / h);

    return normalizedPos;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent) {
    GameObject* newGameObject = new GameObject(parent);

    gameObjects.push_back(newGameObject);

    return newGameObject;
}

void ModuleScene::setMainCamera(ComponentCamera* cam) {
    for (int i = 0; i < sceneCameras.size(); i++)
    {
        sceneCameras[i]->mainCam = false;
        if (sceneCameras[i] == cam)
        {
            LOG("Setting main camera");
            sceneCameras[i]->mainCam = true;
            break;
        }
    }
}
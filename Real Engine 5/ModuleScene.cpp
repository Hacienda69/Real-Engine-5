#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ModuleEditor.h"
#include "OurPrimitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "ModulePlayer.h"
#include "GameObject.h"

#include "Bullet/include/btBulletDynamicsCommon.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl2.h"

std::vector<PhysBody3D*> cubeBodies;

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled), isPlay(false), isPaused(false)
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
    
    bakerHouse1 = App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
    bakerHouse1->transform->setPosition(float3(0.f, 0.0f, 10.f));
    bakerHouse1->name = "Baker House 1";

    bakerHouse2 = App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
    bakerHouse2->transform->setPosition(float3(-10.f, 0.0f, 0.f));
    bakerHouse2->transform->setRotation(float3(0, -90, 0));
    bakerHouse2->name = "Baker House 2";

    bakerHouse3 = App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
    bakerHouse3->transform->setPosition(float3(0.f, 0.0f, -10.f));
    bakerHouse3->transform->setRotation(float3(0, 180, 0)); 
    bakerHouse3->name = "Baker House 3";

    bakerHouse4 = App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
    bakerHouse4->transform->setPosition(float3(10.f, 0.0f, 0.f));
    bakerHouse4->transform->setRotation(float3(0, 90, 0));
    bakerHouse4->name = "Baker House 4";

    floor = PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_PLANE);
    floor->transform->setPosition(float3(0, 0, 0));
    floor->transform->setRotation(float3(-90, 0, 0));
    floor->transform->setScale(float3(20, 20, 1));
    floor->name = "Floor";

    PrimCube cube1(1.0f, 1.0f, 1.0f);
    cube1.SetPos(0.0f, 2.0f, 0.0f);
    cube1.color = { 1.0f, 0.0f, 0.0f, 1.0f };  // primer cubo en rojo
    PhysBody3D* body1 = App->physics->AddBody(cube1, 1.0f);
    cubeBodies.push_back(body1);

    PrimCube cube2(1.0f, 1.0f, 1.0f);
    cube2.SetPos(2.0f, 2.0f, 0.0f);
    cube2.color = { 0.0f, 0.0f, 1.0f, 1.0f };  // segundo cubo en azul
    PhysBody3D* body2 = App->physics->AddBody(cube2, 1.0f);
    cubeBodies.push_back(body2);

    App->physics->CreateJointBetweenCubes(cubeBodies[0], cubeBodies[1]);

    defaultCamera = PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::CAMERA);
    defaultCamera->name = "Main Camera";
    defaultCamera->transform->setPosition(float3(0.f, 2.5f, -10.f));
    setMainCamera(defaultCamera->GetCameraComponent());

    return true;
}

update_status ModuleScene::PreUpdate(float dt) {
    switch (gameState) {
    case 1: // PLAY
        deltaTime = (float)timerGameScene.Read() / 1000.0f;
        timerGameScene.Stop();
        timerGameScene.Start();
        isPlay = true;
        isPaused = false;
        App->scene->isPlay = true;
        break;

    case 2: // STOP
        timerGameScene.Stop();
        isPlay = false;
        App->scene->isPlay = false;
        break;

    case 3:	// PAUSE
        isPlay = false;
        isPaused = true;
        App->scene->isPlay = false;
        break;
    }
    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {

    //LOG("GameObjects: %d", gameObjects.size());
    //LOG("SceneCameras: %d", sceneCameras.size());

    if (isPlay) PlayEvent();

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

void ModuleScene::SceneWindow(bool& active) {
    
    ImGui::Begin("Scene");
    sizeScreen = ImGui::GetContentRegionAvail();

    ImVec2 newWinSize = sizeScreen;
    newWinSize.x = (newWinSize.y / 9.0f) * 16.0f;

    float uvOffset = (sizeScreen.x - newWinSize.x) / 2.0f;
    uvOffset /= newWinSize.x;

    ImGui::Image((ImTextureID)App->camera->sceneCamera->GetCamBuffer(), sizeScreen, ImVec2(-uvOffset, 1), ImVec2(1 + uvOffset, 0));

    if (ImGui::IsMouseClicked(0) && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && ImGui::IsWindowHovered()) {
        LineSegment ray = App->camera->GenerateRayFromMouse();

        std::vector<GameObject*> intersectedGO;

        for (int i = 0; i < App->assimpMeshes->meshes.size(); i++)
        {
            if (ray.Intersects(App->assimpMeshes->meshes[i]->OBB))
            {
                if (App->assimpMeshes->meshes[i]->owner != nullptr) 
                    intersectedGO.push_back(App->assimpMeshes->meshes[i]->owner); 
            }
        }

        if (intersectedGO.size() == 0) App->hierarchy->SetGameObject(nullptr);
        else SetSelectedByTriangle(ray, intersectedGO);

        intersectedGO.clear();
    }
    ImGui::End();
}

void ModuleScene::GameWindow(bool& active) 
{
    ImGui::Begin("Game", 0, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus);
    sizeScreen = ImGui::GetContentRegionAvail();

    ImVec2 newWinSize = sizeScreen;
    newWinSize.x = (newWinSize.y / 9.0f) * 16.0f;

    float uvOffset = (sizeScreen.x - newWinSize.x) / 2.0f;
    uvOffset /= newWinSize.x;

    if (App->scene->mainCamera != nullptr)
        ImGui::Image((ImTextureID)App->scene->mainCamera->GetCamBuffer(), sizeScreen, ImVec2(-uvOffset, 1), ImVec2(1 + uvOffset, 0));

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

void ModuleScene::SetSelectedByTriangle(LineSegment ray, std::vector<GameObject*> GoList)
{
    float currentDistance;
    float minDistance = 0;

    for (int i = 0; i < GoList.size(); i++)
    {
        ComponentMesh* auxMesh = GoList[i]->GetMeshComponent();
        if (auxMesh != nullptr) {

            for (int w = 0; w < auxMesh->getMeshes().size(); w++)
            {
                Mesh* mesh = auxMesh->getMeshes()[w];
                float4x4 matrix = GoList[i]->transform->getGlobalMatrix().Transposed();

                if (mesh->indexCount > 9) {
                    for (int j = 0; j < mesh->indexCount; j += 3)
                    {
                        float4 point1, point2, point3;
                        float* vertex1 = &mesh->vertex[mesh->index[j] * VERTEX];
                        float* vertex2 = &mesh->vertex[mesh->index[j + 1] * VERTEX];
                        float* vertex3 = &mesh->vertex[mesh->index[j + 2] * VERTEX];

                        point1 = matrix * float4(*vertex1, *(vertex1 + 1), *(vertex1 + 2), 1);
                        point2 = matrix * float4(*vertex2, *(vertex2 + 1), *(vertex2 + 2), 1);
                        point3 = matrix * float4(*vertex3, *(vertex3 + 1), *(vertex3 + 2), 1);

                        float3 _point1, _point2, _point3;
                        _point1 = float3(point1.x, point1.y, point1.z);
                        _point2 = float3(point2.x, point2.y, point2.z);
                        _point3 = float3(point3.x, point3.y, point3.z);

                        Triangle triangle(_point1, _point2, _point3);

                        if (ray.Intersects(triangle, &currentDistance, nullptr))
                        {
                            if (minDistance == 0) {
                                minDistance = currentDistance;
                                App->hierarchy->SetGameObject(GoList[i]);
                                continue;
                            }

                            if (minDistance > currentDistance) {
                                minDistance = currentDistance;
                                App->hierarchy->SetGameObject(GoList[i]);
                            }
                        }
                    }
                }
            }
        }
    }
}
void ModuleScene::PlayEvent()
{
    counter += deltaTime;
    
    if (counter > 0.03f)
    {
        GameObject* house = App->hierarchy->findByName("Baker House");

        if (house != nullptr) {
            float3 previousRot = house->transform->getRotation();
            house->transform->setRotation(float3(previousRot.x, previousRot.y + 1, previousRot.z));
        }
        counter = 0.f;
    }
    
    //LOG("Counter %f", counter)
}
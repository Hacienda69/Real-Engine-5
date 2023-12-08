#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

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

void ModuleScene::EditorWindow() {
    ImGui::Begin("Editor");
    sizeScreen = ImGui::GetContentRegionAvail();

    float aspectRatio = sizeScreen.x / sizeScreen.y;
    App->camera->sceneCamera->frustum.verticalFov = App->camera->sceneCamera->fov * DEGTORAD;
    App->camera->sceneCamera->frustum.horizontalFov = 2.0f * atanf(tanf(App->camera->sceneCamera->frustum.verticalFov / 2.0f) * aspectRatio);

    ImGui::Image((ImTextureID)App->camera->sceneCamera->GetCamBuffer(), sizeScreen, ImVec2(0, 1), ImVec2(1, 0));

    if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {

        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 normalized = NormalizeMouse(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFrameHeight(), ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), mousePos);

        LineSegment my_ray = App->camera->sceneCamera->frustum.UnProjectLineSegment(normalized.x, normalized.y);
        std::vector<GameObject*> interVec;

        for (size_t i = 0; i < App->assimpMeshes->meshes.size(); i++)
        {
            if (my_ray.Intersects(App->assimpMeshes->meshes[i]->OBB)) {
                if (App->assimpMeshes->meshes[i]->owner != nullptr)
                {
                    if (App->assimpMeshes->meshes[i]->owner != nullptr)
                        interVec.push_back(App->assimpMeshes->meshes[i]->owner);
                }
            }
        };

        float distLength;
        float minDistLenght = 0;

        for (size_t j = 0; j < interVec.size(); j++) {

            ComponentMesh* gObjMesh = interVec[j]->GetMeshComponent();
            if (gObjMesh != nullptr) {

                    Mesh* mesh = gObjMesh->mesh;
                    float4x4 matTrans = interVec[j]->transform->getGlobalMatrix().Transposed();

                    if (mesh->indexCount > 9) {
                        for (size_t b = 0; b < mesh->indexCount; b += 3) {

                            float* t1 = &mesh->vertex[mesh->index[b] * VERTEX];
                            float* t2 = &mesh->vertex[mesh->index[b + 1] * VERTEX];
                            float* t3 = &mesh->vertex[mesh->index[b + 2] * VERTEX];

                            float4 tr1 = matTrans * float4(*t1, *(t1 + 1), *(t1 + 2), 1);
                            float4 tr2 = matTrans * float4(*t2, *(t2 + 1), *(t2 + 2), 1);
                            float4 tr3 = matTrans * float4(*t3, *(t3 + 1), *(t3 + 2), 1);

                            float3 tri1 = float3(tr1.x, tr1.y, tr1.z);
                            float3 tri2 = float3(tr2.x, tr2.y, tr2.z);
                            float3 tri3 = float3(tr3.x, tr3.y, tr3.z);

                            Triangle triangle(tri1, tri2, tri3);

                            if (my_ray.Intersects(triangle, &distLength, nullptr))
                            {
                                if (minDistLenght == 0) {
                                    minDistLenght = distLength;
                                    App->hierarchy->SetGameObject(interVec[j]);
                                    continue;
                                }
                                if (distLength < minDistLenght) {
                                    minDistLenght = distLength;
                                    App->hierarchy->SetGameObject(interVec[j]);
                                }

                            }
                        }
                    }
            }

        }
        interVec.clear();
    }

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
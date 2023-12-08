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

void ModuleScene::SceneWindow(bool& active) {
    ImGui::Begin("Scene");
    sizeScreen = ImGui::GetContentRegionAvail();

    ImVec2 newWinSize = sizeScreen;
    newWinSize.x = (newWinSize.y / 9.0f) * 16.0f;

    float uvOffset = (sizeScreen.x - newWinSize.x) / 2.0f;
    uvOffset /= newWinSize.x;

    ImGui::Image((ImTextureID)App->camera->sceneCamera->GetCamBuffer(), sizeScreen, ImVec2(-uvOffset, 1), ImVec2(1 + uvOffset, 0));

    if (ImGui::IsMouseClicked(0) && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && ImGui::IsWindowHovered())
    {
        std::vector<GameObject*> objectsSelectedByMouse;

        // Get mouse position
        ImVec2 mousePosition = ImGui::GetMousePos();

        // Normalize mouse position
        ImVec2 normalizedMousePos = NormalizeMouse(ImGui::GetWindowPos().x,
            ImGui::GetWindowPos().y + ImGui::GetFrameHeight(),
            ImGui::GetWindowSize().x,
            ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), mousePosition);

        normalizedMousePos.x -= 0.5f;
        normalizedMousePos.y -= 0.5f;

        // Create picking line using the camera frustum
        LineSegment pickingLine = App->camera->sceneCamera->frustum.UnProjectLineSegment(normalizedMousePos.x, normalizedMousePos.y);
        //App->renderer3D->ray = pickingLine;

        // Iterate through meshes for intersection
        for (size_t i = 0; i < App->assimpMeshes->meshes.size(); i++)
        {
            if (pickingLine.Intersects(App->assimpMeshes->meshes[i]->OBB))
            {
                if (App->assimpMeshes->meshes[i]->owner != nullptr)
                    objectsSelectedByMouse.push_back(App->assimpMeshes->meshes[i]->owner);
            }
        }

        float currentDistance;
        float minDistance = 0;

        // Find closest object
        for (int i = 0; i < objectsSelectedByMouse.size(); i++)
        {
            Mesh* mesh = objectsSelectedByMouse[i]->GetMeshComponent()->mesh;
            float4x4 matrix = objectsSelectedByMouse[i]->transform->getGlobalMatrix().Transposed();

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

                if (pickingLine.Intersects(triangle, &currentDistance, nullptr))
                {
                    if (minDistance == 0) {
                        minDistance = currentDistance;
                        App->hierarchy->SetGameObject(objectsSelectedByMouse[i]);
                        continue;
                    }

                    if (minDistance > currentDistance) {
                        minDistance = currentDistance;
                        App->hierarchy->SetGameObject(objectsSelectedByMouse[i]);
                    }
                }
            }
        }

        // If no objects selected, clear selection
        if (objectsSelectedByMouse.size() == 0)
            App->hierarchy->SetGameObject(nullptr);

        objectsSelectedByMouse.clear();
    }

    ImGui::End();
}

void ModuleScene::GameWindow(bool& active) {
    ImGui::Begin("Game");
    
    sizeScreen = ImGui::GetContentRegionAvail();
    float aspectRatio = sizeScreen.x / sizeScreen.y;

    if (mainCamera != nullptr) 
    {
        mainCamera->frustum.verticalFov = mainCamera->fov * DEGTORAD;
        mainCamera->frustum.horizontalFov = 2.0f * atanf(tanf(mainCamera->frustum.verticalFov / 2.0f) * aspectRatio);

        ImGui::Image((ImTextureID)mainCamera->GetCamBuffer(), sizeScreen, ImVec2(0, 1), ImVec2(1, 0));
    }

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
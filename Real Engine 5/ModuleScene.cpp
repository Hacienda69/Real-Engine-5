#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
//#include <filesystem>

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
    root->name = ("Scene");

    CreateLibraryFolder();

    //Load Baker House
    App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
   
    mainCamera = nullptr;

    return true;
}


update_status ModuleScene::PreUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {

    //LOG("GameObjects: %d", gameObjects.size());
    //LOG("SceneCameras: %d", sceneCameras.size());

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

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
    GameObject* newGameObject = new GameObject(parent);

    gameObjects.push_back(newGameObject);

    return newGameObject;

}

#include <iostream>
#include <fstream>
#include <string>
#include <direct.h> 
#include "json.hpp"

using json = nlohmann::json;

void ModuleScene::CreateLibraryFolder() {
    std::string libraryFolderPath = "Game/Library";

#ifdef _WIN32
    if (_mkdir(libraryFolderPath.c_str()) == 0) {
        std::cout << "Library folder created successfully." << std::endl;
    }
    else {
        std::cout << "Failed to create Library folder or it already exists." << std::endl;
    }
#else
    if (mkdir(libraryFolderPath.c_str(), 0777) == 0) {
        std::cout << "Library folder created successfully." << std::endl;
    }
    else {
        std::cout << "Failed to create Library folder or it already exists." << std::endl;
    }
#endif

    json metadata;
    metadata["assets"] = {
        { "name", "asset1" },
        { "type", "model" },
        { "path", "Assets/Models/asset1.fbx" }
    };

    std::string metadataString = metadata.dump();
    std::ofstream outputFile("metadata.json");
    outputFile << metadataString;
    outputFile.close();

    std::string metadataFilePath = libraryFolderPath + "/metadata.json";
    std::ofstream metadataFile(metadataFilePath);
    if (metadataFile.is_open()) {
        metadataFile << metadata.dump(4);
        metadataFile.close();
        std::cout << "Metadata JSON file created in Library folder." << std::endl;
    }
    else {
        std::cout << "Failed to create metadata JSON file in Library folder." << std::endl;
    }
}

void ModuleScene::setMainCamera(ComponentCamera* cam) 
{
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
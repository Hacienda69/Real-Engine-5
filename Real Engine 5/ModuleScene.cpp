#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
//#include <filesystem>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
}

ModuleScene::~ModuleScene() 
{
}

bool ModuleScene::Init()
{
    //CreateLibraryFolder();
    return false;
}

bool ModuleScene::Start() {
    
    root = new GameObject(nullptr);
    root->name = ("Scene");



    //Load Baker House
    App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
   

    return true;
}


update_status ModuleScene::PreUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {
    
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



    return newGameObject;

}
/*
#include <iostream>
#include <fstream>
#include <string>
#include <direct.h>

void CreateLibraryFolder() {
    std::string libraryFolderPath = "Game/Library";

    // Check if the Library folder exists; if not, create it
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

    // Now, you can generate metadata files or perform other operations within this folder
    // For instance, create and write metadata information for assets in the "Assets" folder
    // ...

    // Example of creating a metadata file in the Library folder
    std::string metadataFilePath = libraryFolderPath + "/metadata.txt";
    std::ofstream metadataFile(metadataFilePath);
    if (metadataFile.is_open()) {
        metadataFile << "Example metadata content for assets.\n";
        metadataFile.close();
        std::cout << "Metadata file created in Library folder." << std::endl;
    }
    else {
        std::cout << "Failed to create metadata file in Library folder." << std::endl;
    }
}*/
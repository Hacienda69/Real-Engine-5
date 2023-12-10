#pragma once
#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include"Glew/include/glew.h"
#include "GameObject.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include <string>


#define VERTEX 5

struct Mesh {
    uint indexID = 0;
    uint indexCount = 0;
    uint* index = nullptr;

    uint vertexID = 0;
    uint vertexCount = 0;
    float* vertex = nullptr;

    float* vertexNormalFaces = nullptr;

    aiVector3D* vertexNormals = nullptr; 

    GameObject* owner = nullptr;

    uint VAO;
    uint VBO;
    uint EBO;

    AABB Local_AABB;
    AABB Global_AABB;
    OBB OBB;

    bool showAABB = true;
    bool showOBB = true;

    bool isInFrustum = false;

    GLuint id_texture;
    GLuint texture_width = 0;
    GLuint texture_height = 0;

    void Render();

    void RenderFaceNormals();

    void RenderVertexNormals();

    void GenerateAABB();
    void RenderAABB();
    void DrawBbox(float3* corners, float3 color);
};

class ModuleAssimpMeshes : public Module
{
public:

    ModuleAssimpMeshes(Application* app, bool start_enabled = true);
    ~ModuleAssimpMeshes();

    bool Init() override;
    bool Start() override;
    bool CleanUp() override;
    GameObject* ProcessNode(const aiScene* scene, aiNode* node, GameObject* parent, std::string Path);
    GameObject* LoadMeshFromFile(const char* file_path);
    Mesh* ImportAssimpMesh(aiMesh* aiMesh);
    std::string ImportTexture(const aiScene* scene, int index, std::string path);
    void BufferMesh(Mesh* mesh);
    void RenderScene();
    void RenderGame();

    void DeleteMesh(Mesh* mesh);
    
    bool SerializeMeshToCustomFormat(const Mesh& mesh, const std::string& filePath);

    bool DeserializeMeshFromCustomFormat(Mesh& mesh, const std::string& filePath);
    
public:
    std::vector<Mesh*> meshes;
    const char* file_path;
    bool MeshDebug;

    bool showAABB, showOBB;
};


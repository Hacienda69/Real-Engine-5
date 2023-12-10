#include "ModuleAssimpMeshes.h"
#include "Application.h"
#include "ModuleTexture.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"

// MODULE ASSIMP MESHES -------------------------------------------------------------------------------------------------
ModuleAssimpMeshes::ModuleAssimpMeshes(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleAssimpMeshes::~ModuleAssimpMeshes()
{
}

bool ModuleAssimpMeshes::Init()
{
    return true;
}

bool ModuleAssimpMeshes::Start()
{
    bool ret = true;

    // Stream log messages to Debug window
    struct aiLogStream stream;
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
    aiAttachLogStream(&stream);

    MeshDebug = true;
    
    showAABB = true;
    showOBB = true;
                
    return ret;
}

//PROBLEMA
GameObject* ModuleAssimpMeshes::LoadMeshFromFile(string file_path)
{
    const aiScene* scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene != nullptr&& scene->HasMeshes())
    {
        GameObject* OBJ = ProcessNode(scene, scene->mRootNode, App->scene->root, file_path);


        aiReleaseImport(scene);

        return OBJ;
    }
    else
    {
        LOG("Error loading scene: %s", file_path);
    }
}

Mesh* ModuleAssimpMeshes::ImportAssimpMesh(aiMesh* aiMesh)
{
    Mesh* ourMesh = new Mesh();

    std::string assimpMeshName = "Importing Assimp Mesh: " + std::string(aiMesh->mName.C_Str());
    LOG(assimpMeshName.c_str());

    ourMesh->vertexCount = aiMesh->mNumVertices;
    ourMesh->vertex = new float[ourMesh->vertexCount * VERTEX];

    //ourMesh->vertexNormalFaces = new float[ourMesh->vertexCount * 3];
    //memcpy(ourMesh->vertexNormalFaces, aiMesh->mVertices, sizeof(float) * ourMesh->vertexCount * 3);

    for (int v = 0; v < ourMesh->vertexCount; v++) {    
        ourMesh->vertex[v * VERTEX] = aiMesh->mVertices[v].x;
        ourMesh->vertex[v * VERTEX + 1] = aiMesh->mVertices[v].y;
        ourMesh->vertex[v * VERTEX + 2] = aiMesh->mVertices[v].z;
        if (aiMesh->mTextureCoords[0] == nullptr) continue;
        ourMesh->vertex[v * VERTEX + 3] = aiMesh->mTextureCoords[0][v].x;
        ourMesh->vertex[v * VERTEX + 4] = aiMesh->mTextureCoords[0][v].y;
    }

    if (aiMesh->HasFaces())     
    {
        ourMesh->indexCount = aiMesh->mNumFaces * 3;
       
        ourMesh->index = new uint[ourMesh->indexCount]; 

        for (uint i = 0; i < aiMesh->mNumFaces; ++i)
        {
            if (aiMesh->mFaces[i].mNumIndices != 3)
            {
                LOG("WARNING, geometry face with != 3 indices!");
            }
            else
            {
                memcpy(&ourMesh->index[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
            }
        }

        ourMesh->VBO = 0;
        ourMesh->EBO = 0;
        ourMesh->GenerateAABB(); // Generate 

        BufferMesh(ourMesh);

        meshes.push_back(ourMesh);
        /*
        ComponentMesh* meshComp = new ComponentMesh(CgameObject);
        ourMesh->owner = CgameObject;

        meshComp->mesh = ourMesh;
        CgameObject->AddComponent(meshComp);

        ourMesh->id_texture = App->textures->checkersID;

        //meshComp->type = ComponentType::MESH;
       
        if (scene->HasMaterials()) {
            if (scene->mMaterials[scene->mMeshes[index]->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                
                aiString texture_path;
                scene->mMaterials[scene->mMeshes[index]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
                aiString new_path;
                new_path.Set("Assets/Textures/");
                new_path.Append(texture_path.C_Str());
               
               
                ComponentMaterial* matComp = new ComponentMaterial(CgameObject);
                matComp->mOwner = CgameObject;
                matComp->SetTexture(new_path.C_Str());
                CgameObject->AddComponent(matComp);
            }
            else
            {
                ComponentMaterial* mat = new ComponentMaterial();
                mat->mOwner = CgameObject;
                CgameObject->AddComponent(mat);
            }
        } 
        ourMesh->texture_height = App->textures->textureWidth;
        ourMesh->texture_width = App->textures->textureWidth;*/
        return ourMesh;
    }
    else
    {
        delete ourMesh;
        return nullptr;
    }
}

GameObject* ModuleAssimpMeshes::ProcessNode(const aiScene* scene, aiNode* node, GameObject* parent, std::string Path)
{
    if (node->mNumMeshes == 0 && node->mNumChildren == 0) return nullptr;

    GameObject* gObj = new GameObject(parent);

    gObj->name = node->mName.C_Str();

    aiVector3D scale, position, rotation;
    aiQuaternion QuatRotation;

    node->mTransformation.Decompose(scale, QuatRotation, position);
    rotation = QuatRotation.GetEuler();

    gObj->transform->getScale() = float3(scale.x, scale.y, scale.z);
    gObj->transform->setPosition(float3(position.x, position.y, position.z));
    gObj->transform->calculateMatrix();

    if (node->mNumMeshes != 0) {

        ComponentMesh* component = new ComponentMesh(gObj);


        std::string texture_path = "";


        for (int i = 0; i < node->mNumMeshes; i++)
        {
            Mesh* mesh = ImportAssimpMesh(scene->mMeshes[node->mMeshes[i]]);

            if (mesh == nullptr) {
                LOG("Error loading scene %s", Path);
                continue;
            }

            mesh->owner = gObj;
            component->meshes.push_back(mesh);

            if (texture_path == "") texture_path = ImportTexture(scene, node->mMeshes[i], Path);

        }

        gObj->mComponents.push_back(component);

        if (texture_path != "") {
            ComponentMaterial* componentT = new ComponentMaterial(gObj);
            gObj->mComponents.push_back(componentT);
            componentT->SetTexture(texture_path.c_str());
        }
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(scene, node->mChildren[i], gObj, Path);
    }

    return gObj;
}

std::string ModuleAssimpMeshes::ImportTexture(const aiScene* scene, int index, std::string path)
{

    if (scene->HasMaterials())
    {
        aiMaterial* MaterialIndex = scene->mMaterials[scene->mMeshes[index]->mMaterialIndex];
        if (MaterialIndex->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

            aiString TextPath;
            MaterialIndex->GetTexture(aiTextureType_DIFFUSE, 0, &TextPath);

            for (int i = 0; i < path.size(); i++)
            {
                if (path[i] == '\\')
                {
                    path[i] = '/';
                }
            }
            std::string NormTextPath = TextPath.C_Str();

            for (int i = 0; i < NormTextPath.size(); i++)
            {
                if (NormTextPath[i] == '\\')
                {
                    NormTextPath[i] = '/';
                }
            }
            std::string AssetsPath = path;
            uint AssetsPos = AssetsPath.find("Assets/");

            AssetsPath = AssetsPath.substr(AssetsPos, AssetsPath.find_last_of("/") - AssetsPos);
            AssetsPath = AssetsPath.substr(AssetsPos, AssetsPath.find_last_of("/") - AssetsPos);
            AssetsPath.append("/Textures/").append(TextPath.C_Str());

            return AssetsPath;
        }
    }
    return "";
}

void ModuleAssimpMeshes::BufferMesh(Mesh* mesh)
{
    //Fill buffers with vertex
    // glEnableClientState(GL_VERTEX_ARRAY);


    glGenBuffers(1, (GLuint*)&(mesh->VBO));
    glGenBuffers(1, (GLuint*)&(mesh->EBO));


    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->vertexCount * VERTEX, mesh->vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->indexCount, mesh->index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glDisableClientState(GL_VERTEX_ARRAY);

}

void ModuleAssimpMeshes::RenderScene()
{
    for (int i = 0; i < meshes.size(); i++) {
        glColor3f(1.0f, 1.0f, 1.0f);
        meshes[i]->OBB = meshes[i]->Local_AABB;
        meshes[i]->OBB.Transform(meshes[i]->owner->transform->getGlobalMatrix().Transposed());
        meshes[i]->Global_AABB.SetNegativeInfinity();
        meshes[i]->Global_AABB.Enclose(meshes[i]->OBB);

        //meshes[i]->Render();

        if(meshes[i]->isInFrustum) meshes[i]->Render();
           
        meshes[i]->RenderAABB();

        glColor3f(0.0f, 0.6f, 0.7f);
        if (meshes[i]->owner->GetMeshComponent()->faceNormals) {
            meshes[i]->RenderFaceNormals();
        }
    }
}

void ModuleAssimpMeshes::RenderGame()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(App->scene->mainCamera->GetProjectionMatrix());

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(App->scene->mainCamera->GetViewMatrix());

    glBindFramebuffer(GL_FRAMEBUFFER, App->scene->mainCamera->GetFrameBuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::vector<Mesh*> auxMeshes = App->assimpMeshes->meshes;

    for (int i = 0; i < auxMeshes.size(); i++)
        if (auxMeshes[i]->isInFrustum)
            auxMeshes[i]->Render();
}

void ModuleAssimpMeshes::DeleteMesh(Mesh* mesh) 
{
    auto it = std::find(meshes.begin(), meshes.end(), mesh);

    if (it != meshes.end()) {
        meshes.erase(it);
        delete mesh;
        mesh = nullptr;
    }
    else
    {
        LOG("DELETE MESH NO HA ENCONTRADO LA MESH DESEADA DE ELIMINAR")
    }
}



bool ModuleAssimpMeshes::CleanUp()
{
   
    aiDetachAllLogStreams();
    return true;
}


// MESH ---------------------------------------------------------------------------------------------------------------------------------------
void Mesh::Render()
{
    glEnable(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, id_texture);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 

    glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX, NULL);
    glTexCoordPointer(2, GL_FLOAT, sizeof(float) * VERTEX, (void*)(sizeof(float) * 3));

    glPushMatrix(); // Bind transform matrix

    if (owner != nullptr) {
        
        glMultMatrixf(owner->GetTransformComponent()->getGlobalMatrix().ptr());
    }

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

    glPopMatrix();
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void Mesh::RenderVertexNormals()
{
    //Vertex normals
    float normalLenght = 0.05f;
    
    glBegin(GL_LINES);
    for (unsigned int i = 0; i < /*indexCount*/vertexCount * 3; i += 3)
    {
        glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
        glVertex3f(vertex[i] + vertexNormals[i].x * normalLenght, vertex[i + 1] + vertexNormals[i].y * normalLenght, vertex[i + 2] + vertexNormals[i].z * normalLenght);
    }
    glEnd();
    

}

void Mesh::RenderFaceNormals()
{
    //---num_index
    for (size_t i = 0; i < indexCount; i += 3)
    {
        // Obtener los índices de los vértices para esta cara
        size_t index1 = index[i];
        size_t index2 = index[i + 1];
        size_t index3 = index[i + 2];

        // Calcular el punto medio de la cara del triángulo
        double xMidpoint = (vertexNormalFaces[index1 * 3] + vertexNormalFaces[index2 * 3] + vertexNormalFaces[index3 * 3]) / 3.0;
        double yMidPoint = (vertexNormalFaces[index1 * 3 + 1] + vertexNormalFaces[index2 * 3 + 1] + vertexNormalFaces[index3 * 3 + 1]) / 3.0;
        double zMidPoint = (vertexNormalFaces[index1 * 3 + 2] + vertexNormalFaces[index2 * 3 + 2] + vertexNormalFaces[index3 * 3 + 2]) / 3.0;

        // Calcular la normal de la cara del triángulo usando el producto cruz
        double edge1x = vertexNormalFaces[index2 * 3] - vertexNormalFaces[index1 * 3];
        double edge1y = vertexNormalFaces[index2 * 3 + 1] - vertexNormalFaces[index1 * 3 + 1];
        double edge1z = vertexNormalFaces[index2 * 3 + 2] - vertexNormalFaces[index1 * 3 + 2];

        double edge2x = vertexNormalFaces[index3 * 3] - vertexNormalFaces[index1 * 3];
        double edge2y = vertexNormalFaces[index3 * 3 + 1] - vertexNormalFaces[index1 * 3 + 1];
        double edge2z = vertexNormalFaces[index3 * 3 + 2] - vertexNormalFaces[index1 * 3 + 2];

        double normalx = edge1y * edge2z - edge1z * edge2y;
        double normaly = edge1z * edge2x - edge1x * edge2z;
        double normalz = edge1x * edge2y - edge1y * edge2x;

        // Normaliza la normal
        double length = sqrt(normalx * normalx + normaly * normaly + normalz * normalz);
        normalx /= length;
        normaly /= length;
        normalz /= length;

        // Define la longitud de la línea en la dirección de la normal
        double lineLength = 0.3;

        // Calcula el punto final de la línea
        double xFinal = xMidpoint + normalx * lineLength;
        double yFinal = yMidPoint + normaly * lineLength;
        double zFinal = zMidPoint + normalz * lineLength;


        // Dibujar la normal como una línea roja desde el punto medio de la cara
        glLineWidth(0.8f);
        glBegin(GL_LINES);
        glVertex3d(xMidpoint, yMidPoint, zMidPoint);
        glVertex3d(xFinal, yFinal, zFinal);
        glEnd();
        glLineWidth(1.0f);
    }
    

    ////---num_vertex
    //for (size_t i = 0; i < num_vertex; i += 9)
    //{

    //    // Calcular el punto medio de la cara del triángulo
    //    double xMedio = (vertex[i] + vertex[i + 3] + vertex[i + 6]) / 3.0;
    //    double yMedio = (vertex[i + 1] + vertex[i + 4] + vertex[i + 7]) / 3.0;
    //    double zMedio = (vertex[i + 2] + vertex[i + 5] + vertex[i + 8]) / 3.0;

    //    // Calcular la normal de la cara del triángulo usando el producto cruz
    //    double edge1x = vertex[i + 3] - vertex[i];
    //    double edge1y = vertex[i + 4] - vertex[i + 1];
    //    double edge1z = vertex[i + 5] - vertex[i + 2];

    //    double edge2x = vertex[i + 6] - vertex[i];
    //    double edge2y = vertex[i + 7] - vertex[i + 1];
    //    double edge2z = vertex[i + 8] - vertex[i + 2];

    //    double normalx = edge1y * edge2z - edge1z * edge2y;
    //    double normaly = edge1z * edge2x - edge1x * edge2z;
    //    double normalz = edge1x * edge2y - edge1y * edge2x;

    //    // Normaliza la normal
    //    double length = sqrt(normalx * normalx + normaly * normaly + normalz * normalz);
    //    normalx /= length;
    //    normaly /= length;
    //    normalz /= length;

    //    // Define la longitud de la línea en la dirección de la normal
    //    double lineaLongitud = 3.0;

    //    // Calcula el punto final de la línea
    //    double xFinal = xMedio + normalx * lineaLongitud;
    //    double yFinal = yMedio + normaly * lineaLongitud;
    //    double zFinal = zMedio + normalz * lineaLongitud;

    //    // Establecer el color a rojo
    //    glColor3f(1.0f, 0.0f, 0.0f);

    //    // Dibujar la normal como una línea roja desde el punto medio de la cara
    //    glLineWidth(2.0f);
    //    glBegin(GL_LINES);
    //    glVertex3d(xMedio, yMedio, zMedio);
    //    glVertex3d(xFinal, yFinal, zFinal);
    //    glEnd();
    //    glLineWidth(1.0f);


    //    glEnd();
    //}
    
}

void Mesh::GenerateAABB() 
{
    if (vertexCount == 0) { LOG("Couldn't generate AABB: Mesh doesn't contain any vertex"); return; }

    std::vector<float3> newVertex;
    newVertex.reserve(vertexCount);  // Reservar espacio para evitar reasignaciones innecesarias.

    // Iterar sobre los vértices y emplazarlos en el vector.
    for (uint i = 0; i < vertexCount * VERTEX; i += VERTEX)
    {
        newVertex.emplace_back(vertex[i], vertex[i + 1], vertex[i + 2]);
    }

    // Llamar a SetFrom con el vector de float3.
    Local_AABB.SetFrom(&newVertex[0], newVertex.size());
}

void Mesh::RenderAABB() 
{
    // Obtener los vértices del OBB (que ya está transformada)
    float3 obbCorners[8];
    OBB.GetCornerPoints(obbCorners);

    // Obtener los vértices del Global AABB (que ya está transformada)
    float3 globalAABBCorners[8];
    Global_AABB.GetCornerPoints(globalAABBCorners);

    if (showOBB) {
        // Dibujar el OBB en pantalla
        DrawBbox(obbCorners, float3(0.0f, 1.0f, 0.0f));  // Color verde para el OBB
    }

    if (showAABB) {
        // Dibujar el Global AABB en pantalla
        DrawBbox(globalAABBCorners, float3(0.0f, 0.0f, 1.0f));  // Color azul para el Global AABB
    }
}

void Mesh::DrawBbox(float3* corners, float3 color)
{
    int indices[24] = { 0,2,2,6,6,4,4,0,0,1,1,3,3,2,4,5,6,7,5,7,3,7,1,5 };
    glBegin(GL_LINES);
    glColor3fv(color.ptr());

    for (size_t i = 0; i < 24; i++)
    {
        glVertex3fv(corners[indices[i]].ptr());
    }

    //glColor3f(255.f, 255.f, 255.f);
    glEnd();
}

#include <fstream>

// Assume Mesh structure is already defined as per your code

// Function to serialize Mesh data into custom format
bool ModuleAssimpMeshes::SerializeMeshToCustomFormat(const Mesh& mesh, const std::string& filePath) {
    // Open a file for writing in binary mode
    std::ofstream outputFile(filePath, std::ios::out | std::ios::binary);
    if (!outputFile.is_open()) {
        // Failed to open file
        return false;
    }

    // Example: Writing header information
    uint vertexCount = mesh.vertexCount;
    uint indexCount = mesh.indexCount;

    // Write header information to the file
    outputFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint));
    outputFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(uint));

    // Example: Writing vertex and index data
    outputFile.write(reinterpret_cast<const char*>(mesh.vertex), sizeof(float) * mesh.vertexCount * VERTEX);
    outputFile.write(reinterpret_cast<const char*>(mesh.index), sizeof(uint) * mesh.indexCount);

    // Close the file
    outputFile.close();

    return true;
}

// Function to deserialize Mesh data from custom format
bool ModuleAssimpMeshes::DeserializeMeshFromCustomFormat(Mesh& mesh, const std::string& filePath) {
    // Open a file for reading in binary mode
    std::ifstream inputFile(filePath, std::ios::in | std::ios::binary);
    if (!inputFile.is_open()) {
        // Failed to open file
        return false;
    }

    // Example: Read header information
    uint vertexCount = 0, indexCount = 0;
    inputFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(uint));
    inputFile.read(reinterpret_cast<char*>(&indexCount), sizeof(uint));

    // Allocate memory for vertex and index data
    mesh.vertexCount = vertexCount;
    mesh.indexCount = indexCount;

    mesh.vertex = new float[mesh.vertexCount * VERTEX];
    mesh.index = new uint[mesh.indexCount];

    // Example: Read vertex and index data
    inputFile.read(reinterpret_cast<char*>(mesh.vertex), sizeof(float) * mesh.vertexCount * VERTEX);
    inputFile.read(reinterpret_cast<char*>(mesh.index), sizeof(uint) * mesh.indexCount);

    // Close the file
    inputFile.close();

    return true;
}
#include "Application.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "Globals.h"

ComponentCamera::ComponentCamera() : Component(nullptr)
{
	type = ComponentType::CAMERA;
	this->mOwner = nullptr;
	fov = 60;
    SetCam();
    SetBuffers();
}

ComponentCamera::ComponentCamera(GameObject* owner) : Component(owner)
{
	type = ComponentType::CAMERA;
	this->mOwner = owner;
	fov = 60;
    SetCam();
    SetBuffers();
}

ComponentCamera::~ComponentCamera() 
{
	//glDeleteFramebuffers(1, &frameBuffer);
	//glDeleteFramebuffers(1, &camBuffer);
	//glDeleteFramebuffers(1, &objBuffer);

    for (int i = 0; i < App->scene->sceneCameras.size(); i++)
    {
        if (App->scene->sceneCameras[i] == this) App->scene->sceneCameras.erase(App->scene->sceneCameras.begin() + i);
    }

    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteTextures(1, &camBuffer); // Cambiado a glDeleteTextures
    glDeleteRenderbuffers(1, &objBuffer); // Cambiado a glDeleteRenderbuffers
}

void ComponentCamera::Update()
{
    // Actualiza el frustum

    //La cámara necesita estar dentro de un gameObject o ModuleCamera3D para que se actualize
    //Sólo tienen frustum culling la mainCamera y la cámara de ModuleCamera3D
    if (mOwner == nullptr) 
    {
        for (int i = 0; i < App->assimpMeshes->meshes.size(); i++) {
            if (ContainsAABB(App->assimpMeshes->meshes[i]->Global_AABB) > 0) {
                App->assimpMeshes->meshes[i]->isInFrustum = true;
                //LOG("Mesh is inside frustm");
            }
            else
            {
                App->assimpMeshes->meshes[i]->isInFrustum = false;
                //LOG("There are meshes outside frustum");
            }
        }
        return;
    }
    else 
    {
        frustum.pos = mOwner->transform->getPosition();
        float4x4 m = mOwner->transform->getGlobalMatrix();
        frustum.up = m.RotatePart().Col(1).Normalized();
        frustum.front = m.RotatePart().Col(2).Normalized();

        if (renderFrustum) RenderFrustum();

        if (mainCam && frustumCulling) 
        {
            for (int i = 0; i < App->assimpMeshes->meshes.size(); i++) {
                if (ContainsAABB(App->assimpMeshes->meshes[i]->Global_AABB) > 0) {
                    App->assimpMeshes->meshes[i]->isInFrustum = true;
                    //LOG("Mesh is inside frustm");
                }
                else
                {
                    App->assimpMeshes->meshes[i]->isInFrustum = false;
                    //LOG("There are meshes outside frustum");
                }
            }
            return;
        }

    }


    //ViewMatrix = frustum.ViewMatrix();
    //ProjectionMatrix = frustum.ProjectionMatrix();

    //frustum = Frustum(ViewMatrix * ProjectionMatrix);

}

// SETCAM --------------------------------------------------------------------------------
void ComponentCamera::SetCam()
{
    frustum.type = FrustumType::PerspectiveFrustum;

    frustum.nearPlaneDistance = 0.1f;
    frustum.farPlaneDistance = farDist;
    frustum.front = float3::unitZ;
    frustum.up = float3::unitY;
    
    frustum.verticalFov = fov * DEGTORAD;
    frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * 1.7f);
    
    frustum.pos = float3(0, 0, 0);
}

// SETBUFFERS ----------------------------------------------------------------------------
void ComponentCamera::SetBuffers()
{
    // Crear el framebuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // Crear un color attachment (puedes agregar más según tus necesidades)
    glGenTextures(1, &camBuffer); 
    glBindTexture(GL_TEXTURE_2D, camBuffer); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, camBuffer, 0); 

    float color[4] = { 0.1,0.1,0.1,0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glBindTexture(GL_TEXTURE_2D, 0); 

    // Crear un depth attachment (z-buffer)
    glGenRenderbuffers(1, &objBuffer); 
    glBindRenderbuffer(GL_RENDERBUFFER, objBuffer); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT); 
    glBindRenderbuffer(GL_RENDERBUFFER, 0); 

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, objBuffer);

    // Verificar que el framebuffer se haya creado correctamente
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) LOG("Framebuffer is uncompleted!");

    // Restaurar el framebuffer predeterminado
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// GETTERS -------------------------------------------------------------------------------
float* ComponentCamera::GetViewMatrix()
{
    ViewMatrix = frustum.ViewMatrix();
    ViewMatrix.Transpose();
    return ViewMatrix.ptr();
}

float* ComponentCamera::GetProjectionMatrix()
{
    ProjectionMatrix = frustum.ProjectionMatrix();
    ProjectionMatrix.Transpose();
    return ProjectionMatrix.ptr();
}

uint* ComponentCamera::GetFrameBuffer() { return &frameBuffer; }
uint* ComponentCamera::GetCamBuffer()   { return &camBuffer; }
uint* ComponentCamera::GetObjBuffer()   { return &objBuffer; }

// OLD MODULECAMERA FUNCTIONS ------------------------------------------------------------
void ComponentCamera::Look(const float3& pos, const float3& ref)
{
    frustum.pos = pos;
    reference = ref;

    LookAt(reference);
}

void ComponentCamera::LookAt(const float3& Spot)
{
    reference = Spot;
    frustum.front = (Spot - frustum.pos).Normalized();
    float3 X = float3(0, 1, 0).Cross(frustum.front).Normalized();
    frustum.up = frustum.front.Cross(X);
}

void ComponentCamera::PrintInspector() 
{
    const char* camType[] = { "Perspective", "Orthographic" };

    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        if (mainCam) { ImGui::SameLine(); ImGui::Text(" ( Main Camera ) "); }

        if (ImGui::Checkbox("Main Camera", &mainCam)) 
        {
            App->scene->setMainCamera(this);
        }

        if (ImGui::Combo("Camera Type", &cameraType, camType, IM_ARRAYSIZE(camType)))
        {
            if (cameraType == 0) frustum.type = FrustumType::PerspectiveFrustum;
            if (cameraType == 1) frustum.type = FrustumType::OrthographicFrustum;
        }

        ImGui::Text(" ");

        if (ImGui::SliderInt("Field of View", &fov, 50, 110))
        {
            frustum.verticalFov = fov * DEGTORAD;
            frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * 1.7f);
        }

        if (ImGui::Checkbox("Render Frustum", &renderFrustum));
        ImGui::SameLine();
        if (ImGui::Checkbox("Frustum Culling", &frustumCulling));
    }
}

// Returns: 0 = OUT - 1 = IN - 2 = INTERSECT
int ComponentCamera::ContainsAABB(AABB& refBox) 
{
    float3 vCorner[8];
    int iTotalIn = 0;
    refBox.GetCornerPoints(vCorner); // get the corners of the box into the vCorner array

    Plane m_plane[6];
    frustum.GetPlanes(m_plane);

    // test all 8 corners against the 6 sides
    // if all points are behind 1 specific plane, we are out
    // if we are in with all points, then we are fully in
    for (int p = 0; p < 6; ++p) {
        int iInCount = 8;
        int iPtIn = 1;
        for (int i = 0; i < 8; ++i) {
            // test this point against the planes
            if (m_plane[p].IsOnPositiveSide(vCorner[i])) { //<-- “IsOnPositiveSide” from MathGeoLib
                iPtIn = 0;
                --iInCount;
            }
        }
        // were all the points outside of plane p?
        if(iInCount == 0)
            return 0;
        // check if they were all on the right side of the plane
        iTotalIn += iPtIn;
    }
    // so if iTotalIn is 6, then all are inside the view
    if (iTotalIn == 6)
        return 1;
    // we must be partly in then otherwise
    return 2;

}

void ComponentCamera::RenderFrustum()
{
    // Obtener vértices del frustum en espacio de vista
    float3 frustumCorners[8];
    frustum.GetCornerPoints(frustumCorners);

    float3 color = float3(1.f, 1.f, 1.f);

    glColor3fv(&color.x);
    glLineWidth(2.f);
    glBegin(GL_LINES);

    //Draw plane
    glVertex3fv(&frustumCorners[0].x);
    glVertex3fv(&frustumCorners[2].x);
    glVertex3fv(&frustumCorners[2].x);
    glVertex3fv(&frustumCorners[6].x);
    glVertex3fv(&frustumCorners[6].x);
    glVertex3fv(&frustumCorners[4].x);
    glVertex3fv(&frustumCorners[4].x);
    glVertex3fv(&frustumCorners[0].x);

    glVertex3fv(&frustumCorners[0].x);
    glVertex3fv(&frustumCorners[1].x);
    glVertex3fv(&frustumCorners[1].x);
    glVertex3fv(&frustumCorners[3].x);
    glVertex3fv(&frustumCorners[3].x);
    glVertex3fv(&frustumCorners[2].x);
    glVertex3fv(&frustumCorners[4].x);
    glVertex3fv(&frustumCorners[5].x);

    glVertex3fv(&frustumCorners[6].x);
    glVertex3fv(&frustumCorners[7].x);
    glVertex3fv(&frustumCorners[5].x);
    glVertex3fv(&frustumCorners[7].x);
    glVertex3fv(&frustumCorners[3].x);
    glVertex3fv(&frustumCorners[7].x);
    glVertex3fv(&frustumCorners[1].x);
    glVertex3fv(&frustumCorners[5].x);

    glEnd();
    glLineWidth(1.f);
    glColor3f(1.f, 1.f, 1.f);
}
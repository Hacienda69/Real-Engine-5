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

    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteTextures(1, &camBuffer); // Cambiado a glDeleteTextures
    glDeleteRenderbuffers(1, &objBuffer); // Cambiado a glDeleteRenderbuffers
}

void ComponentCamera::Update()
{
    // Actualiza el frustum
    if (mOwner == nullptr) return;

    frustum.pos = mOwner->transform->getPosition();
    float4x4 m = mOwner->transform->getGlobalMatrix();
    frustum.up = m.RotatePart().Col(1).Normalized();
    frustum.front = m.RotatePart().Col(2).Normalized();

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, camBuffer, 0);

    // Crear un depth attachment (z-buffer)
    glGenRenderbuffers(1, &objBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, objBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, objBuffer);

    // Verificar que el framebuffer se haya creado correctamente
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // Manejar el error aquí según tus necesidades
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
    }

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

float* ComponentCamera::GetProjetionMatrix()
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
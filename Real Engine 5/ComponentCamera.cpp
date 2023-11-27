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
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteFramebuffers(1, &camBuffer);
	glDeleteFramebuffers(1, &objBuffer);
}

void ComponentCamera::SetCam()
{
    frustum.type = FrustumType::PerspectiveFrustum;

    frustum.nearPlaneDistance = 1.0f;
    frustum.farPlaneDistance = 500.f;
    frustum.front = float3::unitZ;
    frustum.up = float3::unitY;
    
    frustum.verticalFov = fov * DEGTORAD;
    frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * 1.7f);
    
    frustum.pos = float3(0, 0, 0);

    ProjectionMatrix = frustum.ProjectionMatrix();
    ViewMatrix = frustum.ViewMatrix();
}

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

void ComponentCamera::UpdateCam()
{
    // Actualiza el frustum
    //frustum = Frustum(ViewMatrix * ProjectionMatrix);

    ViewMatrix = frustum.ViewMatrix();
    ProjectionMatrix = frustum.ProjectionMatrix();
}
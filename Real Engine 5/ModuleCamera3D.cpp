#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/float3.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "MathGeoLib/include/Geometry/Frustum.h"
#include "MathGeoLib/include/MathGeoLib.h"


#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl2.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	state = camStates::NORMAL;
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	sceneCamera = new ComponentCamera();
	sceneCamera->frustum.pos = float3(0, 5, -10);

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	delete sceneCamera;

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!
	
	sceneCamera->Update();

	Frustum auxFrustum = sceneCamera->frustum;

	float3 newPos(0, 0, 0);

	float Sensitivity = 35.f * dt;
	float speed = 7.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 2;

	float dx = -App->input->GetMouseXMotion();
	float dy = -App->input->GetMouseYMotion();
	float dz = -App->input->GetMouseZ();

	// -------------------------------------------------------------
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		state = camStates::FIRST_PERSON;

	else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		state = camStates::FOCUSED;

	else state = camStates::NORMAL;

	switch (state) 
	{
	// ------------------------------------------------------------
	case camStates::NORMAL:

		if (dz != 0) newPos += auxFrustum.front * speed * -dz * 2;
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
		{
			newPos += auxFrustum.WorldRight() * speed * dx / 2;
			newPos -= auxFrustum.up * speed * dy / 2;
		}
		break;
	// ------------------------------------------------------------
	case camStates::FIRST_PERSON:

		// WASD
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += auxFrustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= auxFrustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += auxFrustum.WorldRight() * speed;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= auxFrustum.WorldRight() * speed;
		
		// QE
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y -= speed;

		// MOUSE ROTATION
		MouseRotation(dx, dy, Sensitivity);

		break;
	// ------------------------------------------------------------	
	case camStates::FOCUSED:

		sceneCamera->LookAt(GetSelectedObjPos());

		//Calculate distance
		float distToRef = float3(sceneCamera->reference - sceneCamera->frustum.pos).Length();

		//Rotate
		MouseRotation(dx, dy, Sensitivity);

		//Go back to distance
		sceneCamera->frustum.pos = sceneCamera->reference + (sceneCamera->frustum.front * -distToRef);

		break;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if(App->hierarchy->objSelected != nullptr) sceneCamera->LookAt(GetSelectedObjPos());
		else LOG("Selected object is nullptr");
	}

	sceneCamera->frustum.pos += newPos;

	return UPDATE_CONTINUE;
}

float3 ModuleCamera3D::GetSelectedObjPos()
{
	float3 ret = float3(0, 0, 0);

	if (App->hierarchy->objSelected != nullptr)
	{
		ret = App->hierarchy->objSelected->GetTransformComponent()->getPosition();
	}

	return ret;
}

void ModuleCamera3D::MouseRotation(float dx, float dy, float sensitivity)
{
	//Rotation
	Quat dir;
	sceneCamera->frustum.WorldMatrix().Decompose(float3(), dir, float3());

	//Mouse look direction
	if (dx != 0)
	{
		float DeltaX = (float)dx * sensitivity;
		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(0.0f, 1.0f, 0.0f), DeltaX * DEGTORAD);
		dir = X * dir;
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * sensitivity;
		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(1.0f, 0.0f, 0.0f), DeltaY * DEGTORAD);
		dir = dir * Y;
	}

	//Set direction
	float4x4 rm = sceneCamera->frustum.WorldMatrix();
	rm.SetRotatePart(dir.Normalized());
	sceneCamera->frustum.SetWorldMatrix(rm.Float3x4Part());
}

LineSegment ModuleCamera3D::GenerateRayFromMouse() {

	ImVec2 mousePos = ImGui::GetMousePos();

	ImVec2 normalizedMousePos = NormalizeMousePos(ImGui::GetWindowPos().x,
		ImGui::GetWindowPos().y + ImGui::GetFrameHeight(),
		ImGui::GetWindowSize().x,
		ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), mousePos);

	normalizedMousePos.x -= 0.5f;
	normalizedMousePos.y -= 0.5f;

	LineSegment ray = App->camera->sceneCamera->frustum.UnProjectLineSegment(normalizedMousePos.x, normalizedMousePos.y);
	App->renderer3D->rayCast = ray;

	return ray;
}

ImVec2 ModuleCamera3D::NormalizeMousePos(float x, float y, float w, float h, ImVec2 p)
{
	return ImVec2((p.x - x) / w, 1.0f - (p.y - y) / h);
}
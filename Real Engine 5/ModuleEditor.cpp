#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include <GL/gl.h>
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl2.h"

#include "parson.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleHierarchy.h"
#include "OurPrimitive.h"

#include "Assimp/include/ai_assert.h"
#include "Assimp/include/version.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    logs = new std::deque<std::string>();
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init();

    mFPSLog.reserve(30);
    mMsLog.reserve(30);

    //Editor attributes Initialization
    showConsoleWindow = false;
    showAboutWindow = false;
    wireframeMode = false;

    //Window info
    fullscreen = App->window->IsFullscreen();
    resizable = App->window->IsResizable();
    borderless = App->window->IsBorderless();
    fulldesktop = App->window->IsFulldesktop();
    brightness = SDL_GetWindowBrightness(App->window->window);
    
    //Renderer info
    vsync = App->renderer3D->GetVsync();
    depthTest = App->renderer3D->GetDepthTestAttribute();
    cullFace = App->renderer3D->GetCullFaceAttribute();
    lighting = App->renderer3D->GetLightingAttribute();
    coloMaterial = App->renderer3D->GetColorMaterialAttribute();
    texture2D = App->renderer3D->GetTexture2DAttribute();
    blend = App->renderer3D->GetBlendAttribute();
    alphaTest = App->renderer3D->GetAlphaTestAttribute();
    lineSmooth = App->renderer3D->GetLineSmoothAttribute();
    pointSmooth = App->renderer3D->GetPointSmoothAttribute();
    polygonSmooth = App->renderer3D->GetPolygonSmoothAttribute();

    //Hardware Info
    SDL_version versionSDL;
    SDL_GetVersion(&versionSDL);
   
    sdlVersion = std::string("SDL Version: ") +
                 std::to_string(versionSDL.major) + "." +
                 std::to_string(versionSDL.minor) + "." +
                 std::to_string(versionSDL.patch);

    assimpVersion = std::to_string(aiGetVersionMajor()) + '.' + std::to_string(aiGetVersionMinor()) + '.' + std::to_string(aiGetVersionRevision());

    systemRAM = (float) SDL_GetSystemRAM() / (1024.f);
    cpuCount = SDL_GetCPUCount();
    cpuCacheSize = SDL_GetCPUCacheLineSize();

    int numScreens = SDL_GetNumVideoDisplays();
    if (numScreens > 0)
    {
        for (int i = 0; i < numScreens; ++i) {
            SDL_DisplayMode displayMode;
            if (SDL_GetCurrentDisplayMode(i, &displayMode) == 0) {
                screens.push_back(SDL_GetDisplayName(i));
            }
        }
    }

    Now3D = SDL_Has3DNow();
    AltiVec = SDL_HasAltiVec();
    AVX = SDL_HasAVX();
    AVX2 = SDL_HasAVX2();
    MMX = SDL_HasMMX();
    RDTSC = SDL_HasRDTSC();
    SSE = SDL_HasSSE();
    SSE2 = SDL_HasSSE2();
    SSE3 = SDL_HasSSE3();
    SSE41 = SDL_HasSSE41();
    SSE42 = SDL_HasSSE42();

    //About Window
    license = ReadFileIO("../../License.txt");

    isActiveHierarchy = true;
    isActiveConsole = true;
    isActiveInspector = true;
    isActiveConfig = true;

    return true;
}


bool ModuleEditor::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    RELEASE(logs);


    return true;
}


update_status ModuleEditor::DrawEditor()
{
    update_status ret = update_status::UPDATE_CONTINUE;
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(main_viewport->WorkPos);
    ImGui::SetNextWindowSize(main_viewport->Size);
    ImGui::SetNextWindowViewport(main_viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockingInv", nullptr, flags);

    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    UpdatePlots();

    ImGuiIO& io = ImGui::GetIO();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Quit Application", "ESC")) {
                ret = UPDATE_STOP;
            }
            ImGui::EndMenu();
            
        }
        if (ImGui::BeginMenu("Game Objects"))
        {

            if (ImGui::MenuItem(" EMPTY  ")) {

                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::EMPTY);
            }

            if (ImGui::MenuItem(" CAMERA  ")) {

                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::CAMERA);
            }

            if (ImGui::MenuItem(" CUBE  ")) {
                
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_CUBE);
            }

            if (ImGui::MenuItem(" PLANE "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_PLANE);
            }

            if (ImGui::MenuItem(" SPHERE  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_SPHERE);
            }

            if (ImGui::MenuItem(" ICOSPHERE  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_ICOSHPERE);
            }

            if (ImGui::MenuItem(" CONE  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_CONE);
            }

            if (ImGui::MenuItem(" CYLINDER  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_CYLINDER);
            }

            if (ImGui::MenuItem(" TORUS  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_TORUS);
            }

            if (ImGui::MenuItem(" SUZANNE  "))
            {
                PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType::P_SUZANNE);
            }

            ImGui::EndMenu();
        }
 
        ViewCollapsingHeader();

        if (ImGui::BeginMenu("Help")) DrawAbout();

        //CreateAboutModalPopup(showModalAbout);
        CreateAboutWindow(showAboutWindow);
        CreateConsoleWindow(isActiveConsole);

        if (ImGui::Button("Play")) {
            App->scene->Play();
            App->scene->gameState = 1;
            ImGui::SetWindowFocus("Game");
        }
        if (ImGui::Button("Stop")) {
            App->scene->Stop();
            App->scene->gameState = 2;
            ImGui::SetWindowFocus("Editor");
        }
        if (ImGui::Button("Pause")) {
            App->scene->Pause();
            App->scene->gameState = 3;
        }

        ImGui::EndMainMenuBar();
    }

    if (App->hierarchy->objSelected && isActiveInspector) {

        if (ImGui::Begin("Inspector")) {
            
            App->hierarchy->objSelected->PrintInspector();
        }
        ImGui::End();
    }

    if (isActiveHierarchy) {
        if (ImGui::Begin("GameObjects Hierarchy")) {

            App->hierarchy->GameObjectTree(App->scene->root, 0);
        }
        ImGui::End();
    }

    if (isActiveConfig) DrawConfiguration();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return ret;
}

void ModuleEditor::ViewCollapsingHeader() {

    if (ImGui::BeginMenu("View")) {

        if (ImGui::Checkbox("Configuration", &isActiveConfig)) 
        {
            isActiveConfig != isActiveConfig;
        }
        if (ImGui::Checkbox("Hierarchy", &isActiveHierarchy))
        {
            isActiveHierarchy != isActiveHierarchy;
        }
        if (ImGui::Checkbox("Inspector", &isActiveInspector))
        {
            isActiveInspector != isActiveInspector;
        }
        if (ImGui::Checkbox("Console", &isActiveConsole))
        {
            isActiveConsole != isActiveConsole;
        }

        ImGui::EndMenu();
    }
}

void ModuleEditor::ApplicationCollapsingHeader() 
{
    if (ImGui::CollapsingHeader("Application")) {
        if (ImGui::Checkbox("Vsync", &vsync))
        {
            App->renderer3D->SetVsync(vsync);
        }

        ImGui::SameLine();
        int auxFPS = App->maxFrameRate;
        if (ImGui::SliderInt("Max FPS", &auxFPS, 1, 120))
            App->maxFrameRate = auxFPS;

        char title[25];
        sprintf_s(title, 25, "FrameRate %0.1f", mFPSLog[mFPSLog.size() - 1]);
        ImGui::PlotHistogram("##FPS Log", &mFPSLog[0], mFPSLog.size(), 0, title, 0.0f, 120.0f, ImVec2(300, 100));

        sprintf_s(title, 25, "Milliseconds %0.1f", mMsLog[mMsLog.size() - 1]);
        ImGui::PlotHistogram("##MS Log", &mMsLog[0], mMsLog.size(), 0, title, 0.0f, 50.0f, ImVec2(300, 100));

        ImGui::Text("Average MS: ");  ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "%.3f", 1000.0 / ImGui::GetIO().Framerate);
        ImGui::Text("Average FPS: "); ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "%.1f", ImGui::GetIO().Framerate);

    }
}

void ModuleEditor::WindowCollapsingHeader()
{
    if (ImGui::CollapsingHeader("Window"))
    {
        int oldWidth = App->window->width;
        int oldHeight = App->window->height;

        if (ImGui::Checkbox("Fullscreen", &fullscreen))
        {
            App->window->SetFullscreen(fullscreen);
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Resizable", &resizable))
        {
            App->window->SetResizable(resizable);
        }

        if (ImGui::Checkbox("Borderless", &borderless))
        {
            App->window->SetBorderless(borderless);
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Full Desktop", &fulldesktop))
        {
            App->window->SetFulldesktop(fulldesktop);
        }

        ImGui::SliderInt("Window Height", &App->window->height, 256, 2048);
        ImGui::SliderInt("Window Width", &App->window->width, 320, 2560);

        if (fulldesktop && fullscreen && !resizable)
        {
            App->window->width = oldWidth;
            App->window->height = oldHeight;
        }

        if (oldWidth != App->window->width) { App->window->OnWidthChanged(); }
        if (oldHeight != App->window->height) { App->window->OnHeightChanged(); }

        ImGui::Spacing();

        if (ImGui::SliderFloat("Brightness", &brightness, 0.100f, 1.000f))
        {
            SDL_SetWindowBrightness(App->window->window, brightness);
            
        }
        
    }
}

void ModuleEditor::RenderCollapsingHeader()
{
    if (ImGui::CollapsingHeader("Renderer"))
    {
        if (ImGui::BeginTable("split", 3))
        {
            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_DEPTH_TEST", &depthTest))
            {
                App->renderer3D->SetDepthTestAttribute(depthTest);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_CULL_FACE", &cullFace))
            {
                App->renderer3D->SetCullFaceAttribute(cullFace);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_LIGHTING", &lighting))
            {
                App->renderer3D->SetLightingAttribute(lighting);
            }

            ImGui::Spacing();

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_COLOR_MATERIAL", &coloMaterial))
            {
                App->renderer3D->SetColorMaterialAttribute(coloMaterial);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_TEXTURE_2D", &texture2D))
            {
                App->renderer3D->SetTexture2DAttribute(texture2D);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_BLEND", &blend))
            {
                App->renderer3D->SetBlendAttribute(blend);
            }

            ImGui::Spacing();

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_ALPHA_TEST", &alphaTest))
            {
                App->renderer3D->SetAlphaTestAttribute(alphaTest);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_LINE_SMOOTH", &lineSmooth))
            {
                App->renderer3D->SetLineSmoothAttribute(lineSmooth);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_POINT_SMOOTH", &pointSmooth))
            {
                App->renderer3D->SetPointSmoothAttribute(pointSmooth);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("GL_POLYGON_SMOOTH", &polygonSmooth))
            {
                App->renderer3D->SetPolygonSmoothAttribute(polygonSmooth);
            }

            ImGui::TableNextColumn();
            if (ImGui::Checkbox("WIREFRAME_MODE", &wireframeMode))
            {
                App->renderer3D->SetWireframeMode(wireframeMode);
            }

            ImGui::Spacing();

   
            ImGui::EndTable();
        }
    }
}

void ModuleEditor::InputCollapsingHeader() 
{
    if (ImGui::CollapsingHeader("Inputs"))
    {
        ImGuiIO io = ImGui::GetIO();

        ImGui::Text("Mouse Position:"); ImGui::SameLine();
        ImGui::TextColored(IMGUICOL_CYAN, "%g, %g", io.MousePos.x, io.MousePos.y);
        ImGui::Text("Mouse Delta:"); ImGui::SameLine();
        ImGui::TextColored(IMGUICOL_CYAN, "%g, %g", io.MouseDelta.x, io.MouseDelta.y);
        ImGui::Text("Mouse Wheel:", io.MouseWheel);

        struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
        ImGuiKey start_key = (ImGuiKey)0;

        ImGui::Text("Keys down:");
        for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
        {
            if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue; ImGui::SameLine();
            ImGui::TextColored(IMGUICOL_CYAN, (key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key);
        }
    }
}

void ModuleEditor::HardwareCollapsingHeader()
{
    if (ImGui::CollapsingHeader("Hardware"))
    {
        ImGui::Text(sdlVersion.c_str());
        ImGui::Separator();

        ImGui::Text("CPUs ");
        ImGui::SameLine();
        ImGui::Text("%u", cpuCount);
        ImGui::SameLine();
        ImGui::Text(" (Cache: %u", cpuCacheSize);
        ImGui::SameLine();
        ImGui::Text("kb)\n");


        ImGui::Text("System RAM: ");
        ImGui::SameLine();
        ImGui::TextColored(IMGUICOL_CYAN, "%.2f", systemRAM);
        ImGui::SameLine();
        ImGui::TextColored(IMGUICOL_CYAN, "Gb");

        ImGui::Separator();
        ImGui::Text("Sceen Devices:");

        for (const std::string& screen : screens) {
            ImGui::Text("%s", screen.c_str());
        }

        ImGui::Text("Caps: ");

        if (Now3D)   { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "3DNOW,"); }
        if (AltiVec) { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "AltiVec,"); }
        if (AVX)     { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "AVX,"); }
        if (AVX2)    { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "AVX2,"); }
        if (MMX)     { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "MMX,"); }
        if (RDTSC)   { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "RDTSC,"); }
        if (SSE)     { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "SSE,"); }
        if (SSE2)    { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "SSE2,"); }
        if (SSE3)    { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "SSE3,"); }
        if (SSE41)   { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "SSE41,"); }
        if (SSE42)   { ImGui::SameLine(); ImGui::TextColored(IMGUICOL_CYAN, "SSE42"); }

    }
}

void ModuleEditor::CreateAboutWindow(bool& showAboutWindow)
{
    if (!showAboutWindow)
        return;

    ImGui::SetNextWindowSize(ImVec2(920, 300), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("About", &showAboutWindow))
    {
        showAboutWindow = true;
        ImGui::End();
        return;
    }

    // Basic Info
    ImGui::Text("REAL ENGINE 5");
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Venture into the realm of game development with the legendary Sheesh Engine, \na creation born from the collaborative efforts of two visionary minds at CITM.");
    ImGui::Text("Whether you seek to forge epic tales of heroism or weave enchanting mysteries,\nthis engine is your magical wand.");
    ImGui::Spacing();
    if (ImGui::Button("Autors: Oriol Martin Corella & Xiao Shan Costajussa Bellver"))
    {
        URLButton("https://github.com/Urii98/SheeeshEngine");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Libraries
    ImGui::Text("Libraries and links to their websites:");

    if (ImGui::Button("SDL"))
    {
        URLButton("https://libsdl.org/index.php");
    }
    ImGui::SameLine();
    ImGui::Text(sdlVersion.c_str());

    if (ImGui::Button("OpenGL"))
    {
        URLButton("https://www.opengl.org/");
    }
    ImGui::SameLine();
    ImGui::Text("OpenGL: %s", glewGetString(GLEW_VERSION));

    if (ImGui::Button("ImGui"))
    {
        URLButton("https://github.com/ocornut/imgui/");
    }
    ImGui::SameLine();
    ImGui::Text(ImGui::GetVersion());

    if (ImGui::Button("Glew"))
    {
        URLButton("http://glew.sourceforge.net/");
    }
    ImGui::SameLine();
    ImGui::Text((const char*)glewGetString(GLEW_VERSION));

    // Phys
    // Assimp
    if (ImGui::Button("Assimp"))
    {
        URLButton("https://www.assimp.org/");
    }
    ImGui::SameLine();
    ImGui::Text(assimpVersion.c_str());

    
    //Devil
    if (ImGui::Button("DevIL"))
    {
        URLButton("http://openil.sourceforge.net/");
    }
    ImGui::SameLine();
    ImGui::Text("v1.8.0");

    ImGui::Separator();
    ImGui::Spacing();

    // Licence
    ImGui::Text(license.c_str());

    ImGui::End();
}

void ModuleEditor::CreateConsoleWindow(bool& showConsoleWindow)
{
    if (!showConsoleWindow)
        return;

    bool AutoScroll = false;
    ImGuiTextFilter Filter;

    ImGui::SetNextWindowSize(ImVec2(920, 300), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Console", &showConsoleWindow))
    {
        showConsoleWindow = true;
        ImGui::End();
        return;
    }

    if (ImGui::SmallButton("Clear")) { logs->clear(); }

    ImGui::Separator();
    ImGui::Spacing();

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    //Texto    
    for (std::string& str : *(logs))
    {
        ImGui::Text(str.c_str());
    }    

    ImGui::End();
}


void ModuleEditor::URLButton(const char* url)
{
    
#ifdef _WIN32
    ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    std::string command = "open " + url;
    system(command.c_str());
#elif defined(__linux__)
    std::string command = "xdg-open " + url;
    system(command.c_str());
#else

    ImGui::Text("Sistema operativo no compatible");
#endif
}

void ModuleEditor::UpdatePlots()
{
    AddMs(1000.0 * App->GetDt());
    AddFPS(App->GetFrameRate());
    
}

void ModuleEditor::AddFPS(const float aFPS)
{

    if (mFPSLog.size() == mFPSLog.capacity())
    {

        for (unsigned int i = 0; i < mFPSLog.size(); i++)
        {
            if (i + 1 < mFPSLog.size())
            {
                float iCopy = mFPSLog[i + 1];
                mFPSLog[i] = iCopy;
            }
        }
        mFPSLog[mFPSLog.capacity() - 1] = aFPS;
        
    }
    else
    {
        mFPSLog.push_back(aFPS);
    }
}

void ModuleEditor::AddMs(const float aFPS)
{

    if (mMsLog.size() == mMsLog.capacity())
    {

        for (unsigned int i = 0; i < mMsLog.size(); i++)
        {
            if (i + 1 < mMsLog.size())
            {
                float iCopy = mMsLog[i + 1];
                mMsLog[i] = iCopy;
            }
        }
        mMsLog[mMsLog.capacity() - 1] = aFPS;

    }
    else
    {
        mMsLog.push_back(aFPS);
    }
}

std::string ModuleEditor::ReadFileIO(const char* filePath)
{
    std::ifstream file(filePath); 
    std::string contenido; 

    if (file.is_open()) {
        std::string linea;
        while (std::getline(file, linea)) {
            contenido += linea; 
            contenido += '\n'; 
        }
        file.close(); 
    }
    else {
        return std::string();
    }

    return contenido;
}

float ModuleEditor::AverageValueFloatVector(const std::vector<float>& fps)
{
    if (fps.empty()) {
        return std::numeric_limits<float>::quiet_NaN();
    }

    float total = 0.0f;
    for (float value : fps) {
        total += value;
    }

    float average = total / fps.size();
    return std::round(average * 10) / 10.0f;
}

void ModuleEditor::LOGToConsole(const char* text) {

    if (logs == nullptr) return;

    
    std::time_t tiempoActual = std::time(nullptr);

    
    struct std::tm* tiempoDescompuesto = std::localtime(&tiempoActual);

    
    int horas = tiempoDescompuesto->tm_hour;
    int minutos = tiempoDescompuesto->tm_min;
    int segundos = tiempoDescompuesto->tm_sec;

    char horaFormateada[100];
    std::strftime(horaFormateada, sizeof(horaFormateada), "%H:%M:%S ", tiempoDescompuesto);

    std::string aux = horaFormateada;
    aux += text;

    logs->push_front(aux);

}

void ModuleEditor::DrawAbout() 
{
    if (ImGui::CollapsingHeader("About"))
    {
        ImGui::SeparatorText("ABOUT THE ENGINE:");
        ImGui::Text("Real Engine 5 v0.1");
        ImGui::Text("From the acclaimed studio 'Aprobe Motores', formed by the brilliant");
        ImGui::Text("developers Marc Escandell and Daniel Manas, comes 'Real Engine 5',");
        ImGui::Text("a new cutting - edge video game engine in the thriving virtual");
        ImGui::Text("entertainment industry, we hope you enjoy it.");

        ImGui::NewLine();

        ImGui::SeparatorText("ABOUT THE AUTHORS:");
        ImGui::BulletText("Marc Escandell Alonso");

        if (ImGui::MenuItem("	Marc's Github: https://github.com/MrMonkey420"))
            ShellExecute(NULL, "OPEN", "https://github.com/MrMonkey420", NULL, NULL, SW_SHOW);

        ImGui::NewLine();

        ImGui::BulletText("Daniel Manas Calvo");

        if (ImGui::MenuItem("	Dani's Github: https://github.com/Hacienda69"))
            ShellExecute(NULL, "OPEN", "https://github.com/Hacienda69", NULL, NULL, SW_SHOW);

        ImGui::NewLine();

        ImGui::SeparatorText("3RD PARTY LIBRARIES USED:");

        ImGui::BulletText("SDL 2.0.4");
        if (ImGui::MenuItem("	SDL: https://www.libsdl.org/"))
            ShellExecute(NULL, "OPEN", "https://www.libsdl.org/", NULL, NULL, SW_SHOW);

        ImGui::BulletText("ImGui 1.90 WIP");
        if (ImGui::MenuItem("	ImGui: https://github.com/ocornut/imgui/tree/docking"))
            ShellExecute(NULL, "OPEN", "https://github.com/ocornut/imgui/tree/docking", NULL, NULL, SW_SHOW);

        ImGui::BulletText("Glew 2.1.0");
        if (ImGui::MenuItem("	Glew: https://glew.sourceforge.net/"))
            ShellExecute(NULL, "OPEN", "https://glew.sourceforge.net/", NULL, NULL, SW_SHOW);

        ImGui::BulletText("MathGeoLib 1.0");
        if (ImGui::MenuItem("	MathGeoLib: https://github.com/juj/MathGeoLib"))
            ShellExecute(NULL, "OPEN", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOW);

        ImGui::BulletText("OpenGL %s", glGetString(GL_VERSION));
        if (ImGui::MenuItem("	OpenGL: https://www.opengl.org/"))
            ShellExecute(NULL, "OPEN", "https://www.opengl.org/", NULL, NULL, SW_SHOW);

        ImGui::BulletText("Assimp %d.%d.%d", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
        if (ImGui::MenuItem("	Assimp: https://assimp.org/"))
            ShellExecute(NULL, "OPEN", "https://assimp.org/", NULL, NULL, SW_SHOW);

        ImGui::BulletText("PhysFS 3.0.2");
        if (ImGui::MenuItem("	PhysFS: https://icculus.org/physfs/"))
            ShellExecute(NULL, "OPEN", "https://icculus.org/physfs/", NULL, NULL, SW_SHOW);

        ImGui::BulletText("DevIL 1.8.0");
        if (ImGui::MenuItem("	DevIL: https://github.com/DentonW/DevIL"))
            ShellExecute(NULL, "OPEN", "https://github.com/DentonW/DevIL", NULL, NULL, SW_SHOW);

        ImGui::NewLine();

        ImGui::SeparatorText("lICENSE");

        ImGui::NewLine();

        ImGui::Text("MIT License");

        ImGui::NewLine();

        ImGui::Text("Copyright(c) 2023 Hacienda69");

        ImGui::NewLine();

        ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
        ImGui::Text("of this software and associated documentation files(the 'Software'), to deal");
        ImGui::Text("in the Software without restriction, including without limitation the rights");
        ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and /or sell");
        ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
        ImGui::Text("furnished to do so, subject to the following conditions :");

        ImGui::NewLine();

        ImGui::Text("The above copyright notice and this permission notice shall be included in");
        ImGui::Text("all copies or substantial portions of the Software.");

        ImGui::NewLine();

        ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
        ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
        ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
        ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
        ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
        ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
        ImGui::Text("SOFTWARE.");
    }
    ImGui::EndMenu();
}

void ModuleEditor::DrawConfiguration()
{
    ImGui::Begin("Configuration", &isActiveConfig);

    ImGuiIO& io = ImGui::GetIO();

    static char s1[128] = "Real Engine 5";
    ImGui::InputText("Engine Name", s1, IM_ARRAYSIZE(s1));
    static char s2[128] = "CITM UPC";
    ImGui::InputText("Organization", s2, IM_ARRAYSIZE(s2));

    ImGui::Separator();

    ApplicationCollapsingHeader();
    WindowCollapsingHeader();
    RenderCollapsingHeader();
    InputCollapsingHeader();
    HardwareCollapsingHeader();

    ImGui::End();
}
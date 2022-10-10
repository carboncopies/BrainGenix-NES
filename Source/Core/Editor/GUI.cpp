//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <GUI.h>


GUISystem::GUISystem(ERS_STRUCT_SystemUtils* SystemUtils, GLFWwindow* Window, Cursors3D* Cursors3D, ERS_CLASS_SceneManager* SceneManager, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_CLASS_VisualRenderer* VisualRendererInstance, ERS_STRUCT_HumanInputDeviceUtils* HIDUtils) {

    SystemUtils_ = SystemUtils;
    Window_ = Window;
    Cursors3D_ = Cursors3D;
    SceneManager_ = SceneManager;
    ProjectUtils_ = ProjectUtils;
    VisualRenderer_ = VisualRendererInstance;
    HIDUtils_ = HIDUtils;
    SystemUtils_->Logger_->Log("Initializing DearImGui GUI Library", 5);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Initialize Managers
    ThemeManager_ = std::make_unique<ERS_CLASS_ThemeManager>(SystemUtils_->Logger_.get());
    FontManager_ = std::make_unique<ERS_CLASS_FontManager>(SystemUtils_->Logger_.get());
    UserProfileManager_ = std::make_unique<ERS_CLASS_UserProfileManager>(SystemUtils_->Logger_.get());

    // Load User Profile
    ThemeManager_->ApplyThemes(UserProfileManager_->GetUserColorProfile().c_str());
    FontManager_->UseFont(UserProfileManager_->GetUserFont());
    FontManager_->SetFontSize(UserProfileManager_->GetUserFontSize());

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 440 core");

    // Setup WindowManager Class
    WindowManager_ = std::make_unique<ERS_CLASS_WindowManager>(SystemUtils_);
    WindowManager_->GenerateWindowStruct(ProjectUtils_, HIDUtils_, VisualRenderer_, ThemeManager_.get(), FontManager_.get(), Cursors3D_, SceneManager_);
    

    // Initialize Windows
    SystemUtils_->Logger_->Log("Initializing Editor Menus", 5);
    Menu_File_ = std::make_unique<GUI_Menu_File>(SystemUtils_, SceneManager_, ProjectUtils_, WindowManager_->GetWindowsStruct());
    Menu_Window_ = std::make_unique<GUI_Menu_Window>(SystemUtils_, WindowManager_->GetWindowsStruct(), VisualRenderer_);
    Menu_Debug_ = std::make_unique<GUI_Menu_Debug>(SystemUtils_, WindowManager_->GetWindowsStruct(), WindowManager_.get());
    Menu_Settings_ = std::make_unique<GUI_Menu_Settings>(SystemUtils_, HIDUtils_, WindowManager_->GetWindowsStruct());

    // Disable Dragging Except By Title Bar
    ImGuiIO& IO = ImGui::GetIO();
    IO.ConfigWindowsMoveFromTitleBarOnly = true;


}

GUISystem::~GUISystem() {

    SystemUtils_->Logger_->Log("GUISystem Destructor Called", 6);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

}

void GUISystem::UpdateGUI() {

    
    // CheckUpdate Font
    FontManager_->CheckUpdateFont();

    // Get Window Width, Height
    float Width;
    float Height;
    glfwGetWindowContentScale(Window_, &Width, &Height);

    // Enable Docking
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Setup Dockspace
    ImGui::DockSpaceOverViewport();


    // Begin Guizmo
    ImGuizmo::BeginFrame();


    // Add Main Menu
    if (ImGui::BeginMainMenuBar()) {

        Menu_File_->Draw();
        Menu_Window_->Draw();
        Menu_Settings_->Draw();
        Menu_Debug_->Draw();


    ImGui::EndMainMenuBar();
    }

    // Updates all the windows, draws their content if enabled
    WindowManager_->UpdateAllWindows();


}

void GUISystem::DeferredFrameUpdate() {


}


void GUISystem::UpdateFrame() {

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
}
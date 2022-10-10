//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <RendererManager.h>


// FIXME! MAKE MORE CLEAN LOOKING OR SOMETHING
void ErrorCallback(int, const char* ErrorString) {
    std::cout<<"GLFW ERROR: " << ErrorString << std::endl;
}



RendererManager::RendererManager(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, ERS_STRUCT_HumanInputDeviceUtils* HIDUtils) {

    SystemUtils->Logger_->Log("Populating RendererManager Member Pointers", 5);
    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    HIDUtils_ = HIDUtils;

    Cursors3D_ = std::make_unique<Cursors3D>();


    SystemUtils_->Logger_->Log("Initializing GLFW", 5);
    InitializeGLFW();

    SystemUtils_->Logger_->Log("Instantiating Renderers", 5);
    VisualRenderer_ = std::make_unique<ERS_CLASS_VisualRenderer>(SystemUtils_, ProjectUtils_, Window_, Cursors3D_.get());
    LoadEditorData();
    VisualRenderer_->SetOpenGLDefaults(OpenGLDefaults_.get());




    // Setup Shaders
    ShaderLoader_ = std::make_unique<ERS_CLASS_ShaderLoader>(SystemUtils_);
    for (int i = 0; (long)i < (long)ProjectUtils_->ProjectManager_->Project_.ShaderPrograms.size(); i++) {
        VisualRenderer_->Shaders_.push_back(std::make_unique<ERS_STRUCT_Shader>());
        ERS_STRUCT_Shader* Shader = VisualRenderer_->Shaders_[VisualRenderer_->Shaders_.size()-1].get();

        long VertexShaderID = ProjectUtils_->ProjectManager_->Project_.ShaderPrograms[i].VertexID;
        long FragmentShaderID = ProjectUtils_->ProjectManager_->Project_.ShaderPrograms[i].FragmentID;
        long GeometryShaderID = ProjectUtils_->ProjectManager_->Project_.ShaderPrograms[i].GeometryID;
        long ComputeShaderID = ProjectUtils_->ProjectManager_->Project_.ShaderPrograms[i].ComputeID;
        long TCShaderID = ProjectUtils_->ProjectManager_->Project_.ShaderPrograms[i].TCID;
        long TEShaderID = ProjectUtils_->ProjectManager_->Project_.ShaderPrograms[i].TEID;
        
        std::string ShaderName = ProjectUtils_->ProjectManager_->Project_.ShaderPrograms[i].Name;
        ShaderLoader_->LoadShaderFromAsset(Shader, VertexShaderID, FragmentShaderID, GeometryShaderID, ComputeShaderID, TCShaderID, TEShaderID, ShaderName);

    }
    int DefaultShader = ProjectUtils_->ProjectManager_->Project_.DefaultShaderProgram;
    VisualRenderer_->SetDefaultShader(DefaultShader);

    GuiSystem_ = std::make_shared<GUISystem>(SystemUtils_, Window_, Cursors3D_.get(), ProjectUtils_->SceneManager_.get(), ProjectUtils_, VisualRenderer_.get(), HIDUtils_);

    VisualRenderer_->CreateViewport();
    VisualRenderer_->IsEditorMode_ = !ProjectUtils_->ProjectManager_->Project_.StartPlayingOnLoad;

}


RendererManager::~RendererManager() {

    SystemUtils_->Logger_->Log("RendererManager Destructor Called", 6);

}

void RendererManager::LoadEditorData() {

    OpenGLDefaults_ = std::make_unique<ERS_STRUCT_OpenGLDefaults>();
    FreeImage_Initialise();
    

    // Load Program GLFW Icon
    SystemUtils_->Logger_->Log("Loading System Icon From EditorAssets", 3);
    FREE_IMAGE_FORMAT Format = FreeImage_GetFileType("EditorAssets/Icons/ProgramIcon/Icon.png", 0);
    FIBITMAP* ImageData = FreeImage_Load(Format, "EditorAssets/Icons/ProgramIcon/Icon.png");
    SystemUtils_->Logger_->Log("Applying System Icon", 4);
    GLFWimage Icon[1];
    FreeImage_FlipVertical(ImageData);
    Icon[0].pixels = FreeImage_GetBits(ImageData);
    Icon[0].width = (int)FreeImage_GetWidth(ImageData);
    Icon[0].height = (int)FreeImage_GetHeight(ImageData);
    glfwSetWindowIcon(Window_, 1, Icon);


    // Load Default Textures
    OpenGLDefaults_->DefaultTexture_ = LoadEditorIcon("EditorAssets/Icons/DefaultTexture/8x8/DefaultTexture1024.png");
    OpenGLDefaults_->Loadingtexture_ = LoadEditorIcon("EditorAssets/Icons/LoadingTexture/8x8/LoadingTexture1024.png");
    OpenGLDefaults_->AllBlackTexture_ = LoadEditorIcon("EditorAssets/Icons/AllBlackTexture/AllBlackTexture2.png");
    OpenGLDefaults_->AllWhiteTexture_ = LoadEditorIcon("EditorAssets/Icons/AllWhiteTexture/AllWhiteTexture2.png");
    OpenGLDefaults_->TransparentTexture_ = LoadEditorIcon("EditorAssets/Icons/TransparentTexture/TransparentTexture.png");

    // Load Editor Texture Icons
    OpenGLDefaults_->PointLightTexture_ = LoadEditorIcon("EditorAssets/Icons/LightingIcons/PointLight.png");
    OpenGLDefaults_->DirectionalLightTexture_ = LoadEditorIcon("EditorAssets/Icons/LightingIcons/DirectionalLight.png");
    OpenGLDefaults_->SpotLightTexture_ = LoadEditorIcon("EditorAssets/Icons/LightingIcons/SpotLight.png");

    OpenGLDefaults_->SceneCameraTexture_ = LoadEditorIcon("EditorAssets/Icons/CameraIcon/CameraIcon.png");


    FreeImage_DeInitialise();

}

unsigned int RendererManager::LoadEditorIcon(const char* Path) {


    SystemUtils_->Logger_->Log(std::string("Loading Editor Texture '") + std::string(Path) + std::string("'"), 3);

    FREE_IMAGE_FORMAT TexFormat = FreeImage_GetFileType(Path, 0);
    FIBITMAP* TexImageData = FreeImage_Load(TexFormat, Path);
    unsigned char* RawImageData = FreeImage_GetBits(TexImageData);
    int Width = FreeImage_GetWidth(TexImageData);
    int Height = FreeImage_GetHeight(TexImageData);
    int Channels = FreeImage_GetLine(TexImageData) / Width;

    unsigned int OpenGLTextureID;
    glGenTextures(1, &OpenGLTextureID);
    glBindTexture(GL_TEXTURE_2D, OpenGLTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (Channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, RawImageData);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE, RawImageData);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    FreeImage_Unload(TexImageData);

    return OpenGLTextureID;

}

void RendererManager::InitializeGLFW() {

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Read Out Width, Height
    SystemUtils_->Logger_->Log("Read Configuration File For 'WindowWidth' Parameter", 1);
    WindowWidth_ = (*SystemUtils_->LocalSystemConfiguration_)["WindowWidth"].as<int>();
    SystemUtils_->Logger_->Log("Read Configuration File For 'WindowHeight' Parameter", 1);
    WindowHeight_ = (*SystemUtils_->LocalSystemConfiguration_)["WindowHeight"].as<int>();
    SystemUtils_->Logger_->Log("Read Configuration File For 'WindowTitle' Parameter", 1);
    WindowTitle_ = (*SystemUtils_->LocalSystemConfiguration_)["WindowTitle"].as<std::string>().c_str();

    // Create Window Object
    glfwSetErrorCallback(ErrorCallback);
    Window_ = glfwCreateWindow(WindowWidth_, WindowHeight_, WindowTitle_, NULL, NULL);
    if (Window_ == NULL) {
        glfwTerminate();
    }


    // Bring Window To Front, Unlock Framerate So Our Framerate System Is Used
    glfwMakeContextCurrent(Window_);
    glfwSwapInterval(1);



    // Setup GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SystemUtils_->Logger_->Log("Failed To Initialize GLAD", 10);
    }

    // Setup OpenGL For Blending (For Transparency Issues)
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);


    // Setup Texture Streamer
    SystemUtils_->Logger_->Log("Initializing Texture Streaming Subsystem", 5);
    ProjectUtils_->ModelLoader_->AssetStreamingManager_->SetupTextureStreamer(Window_);
    SystemUtils_->Logger_->Log("Finished Initializing Texture Streaming Subsystem", 4);



}

void RendererManager::UpdateLoop(float DeltaTime) { 

    // Log Any Issues
    ReportOpenGLErrors();



    ERS_STRUCT_Scene* TargetScene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();
    ProjectUtils_->ModelLoader_->AssetStreamingManager_->AsyncTextureUpdater_->SortModels(TargetScene);

    std::vector<ERS_STRUCT_Camera*> Cameras;
    for (unsigned int i = 0; i < VisualRenderer_->Viewports_.size(); i++) {
        Cameras.push_back(VisualRenderer_->Viewports_[i]->Camera.get());
    }
    ProjectUtils_->ModelLoader_->AssetStreamingManager_->UpdateSceneStreamingQueue(ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get(), Cameras);
    



    // Update Window Title
    std::string SceneTitle = ProjectUtils_->ProjectManager_->Project_.ProjectName + std::string(" - BrainGenix-ERS ") + ERS_VERSION;
    glfwSetWindowTitle(Window_, SceneTitle.c_str());

    // Update Scene
    ProjectUtils_->ModelLoader_->ProcessNewModels(ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get());

    // Update IO
    glfwGetWindowSize(Window_, &WindowWidth_, &WindowHeight_);
    glfwPollEvents();

    int Width, Height;
    glfwGetWindowSize(Window_, &Width, &Height);
    SystemUtils_->RenderWidth_ = Width;
    SystemUtils_->RenderHeight_ = Height;


    ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_]->IndexSceneObjects();


    // Update GUI
    GuiSystem_->UpdateGUI();

    // Call Updates
    VisualRenderer_->UpdateViewports(DeltaTime, ProjectUtils_->SceneManager_.get());


    // Update GUI Frame
    ImVec4 Color = GuiSystem_->ThemeManager_->ClearColor_;
    glClearColor(Color.x, Color.y, Color.z, Color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    GuiSystem_->UpdateFrame();
    

    // Update Window Stuff
    glfwSwapBuffers(Window_);

    // Check If System Should Shutdown
    if (glfwWindowShouldClose(Window_)) {
        SystemUtils_->Logger_->Log("System Shutdown Invoked By LocalWindow", 2);
        *SystemUtils_->SystemShouldRun_ = false;
    }


}

void RendererManager::ReportOpenGLErrors() {

    // Get Current Errors
    GLint GLErrorStatus = glGetError();

    if (GLErrorStatus != GL_NO_ERROR) {

        std::string ErrorMeaning = "Unknown";
        if (GLErrorStatus == GL_INVALID_ENUM) {
            ErrorMeaning = "GL_INVALID_ENUM";
        } else if (GLErrorStatus == GL_INVALID_VALUE) {
            ErrorMeaning = "GL_INVALID_VALUE";
        } else if (GLErrorStatus == GL_INVALID_OPERATION) {
            ErrorMeaning = "GL_INVALID_OPERATION";
        } else if (GLErrorStatus == GL_STACK_OVERFLOW) {
            ErrorMeaning = "GL_STACK_OVERFLOW";
        } else if (GLErrorStatus == GL_STACK_UNDERFLOW) {
            ErrorMeaning = "GL_STACK_UNDERFLOW";
        } else if (GLErrorStatus == GL_OUT_OF_MEMORY) {
            ErrorMeaning = "GL_OUT_OF_MEMORY";
        } else if (GLErrorStatus == GL_INVALID_FRAMEBUFFER_OPERATION) {
            ErrorMeaning = "GL_INVALID_FRAMEBUFFER_OPERATION";
        }

        std::string ErrorMessage = std::string("OpenGL Context Reporting Error '") + std::to_string(GLErrorStatus) + std::string("' (") + ErrorMeaning + std::string(")");
        SystemUtils_->Logger_->Log(ErrorMessage, 9);

    }

}
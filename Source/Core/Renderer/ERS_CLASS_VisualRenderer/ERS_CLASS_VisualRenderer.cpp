//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CLASS_VisualRenderer.h>



ERS_CLASS_VisualRenderer::ERS_CLASS_VisualRenderer(ERS_STRUCT_SystemUtils* SystemUtils, ERS_STRUCT_ProjectUtils* ProjectUtils, GLFWwindow* Window, Cursors3D* Cursors3D) {

    SystemUtils->Logger_->Log("Populating Renderer Member Pointers", 5);
    SystemUtils_ = SystemUtils;
    ProjectUtils_ = ProjectUtils;
    Window_ = Window;
    Cursors3D_ = Cursors3D;

    SystemUtils_->Logger_->Log("Initializing MeshRenderer Class", 5);
    MeshRenderer_ = std::make_unique<ERS_CLASS_MeshRenderer>(SystemUtils_);

    SystemUtils_->Logger_->Log("Initializing Viewport Overlay Subsystem", 5);
    ViewportOverlay_ = std::make_unique<ERS_CLASS_ViewportOverlay>(SystemUtils_, ProjectUtils_);

    SystemUtils_->Logger_->Log("Initializing Viewport Menu Subsystem", 5);
    ViewportMenu_ = std::make_unique<ERS_CLASS_ViewportMenu>(SystemUtils_, ProjectUtils_, &GameStartTime_, &IsEditorMode_, &Shaders_);

    ShadowMaps_ = std::make_unique<ERS_CLASS_ShadowMaps>(SystemUtils_, ProjectUtils_, MeshRenderer_.get());

    // DEFAULT MODES, CHANGE THIS LATER! --------------------------------
    IsEditorMode_ = true;
    

}

ERS_CLASS_VisualRenderer::~ERS_CLASS_VisualRenderer() {

    // Destroy Framebuffers
    for (int i = 0; (long)i < (long)Viewports_.size(); i++) {

        glDeleteFramebuffers(1, &Viewports_[i]->FramebufferObject);
        glDeleteTextures(1, &Viewports_[i]->FramebufferColorObject);
        glDeleteRenderbuffers(1, &Viewports_[i]->RenderbufferObject);

    }

}


void ERS_CLASS_VisualRenderer::SetDefaultShader(int ShaderID) {
    
    DefaultShader_ = ShaderID;
}

void ERS_CLASS_VisualRenderer::SetOpenGLDefaults(ERS_STRUCT_OpenGLDefaults* Defaults) {

    OpenGLDefaults_ = Defaults;

}

void ERS_CLASS_VisualRenderer::UpdateViewports(float DeltaTime, ERS_CLASS_SceneManager* SceneManager) {

    // Apply Scene Camera Transforms
    ERS_STRUCT_Scene* Scene = ProjectUtils_->SceneManager_->Scenes_[ProjectUtils_->SceneManager_->ActiveScene_].get();
    if (!IsEditorMode_ && Scene->ActiveSceneCameraIndex != -1) {
        ERS_STRUCT_Camera* Camera = Viewports_[0]->Camera.get();
        ERS_STRUCT_SceneCamera* SceneCamera = Scene->SceneCameras[Scene->ActiveSceneCameraIndex].get();
        if (SceneCamera->EnforceAspectRatio_) {
            Camera->SetAspectRatio(SceneCamera->AspectRatio_);
        }
        Camera->SetClipBoundries(SceneCamera->NearClip_, SceneCamera->FarClip_);
        Camera->SetFOV(SceneCamera->FOV_);
        Camera->SetPosition(SceneCamera->Pos_);
        Camera->SetRotation(SceneCamera->Rot_);
        Camera->SetStreamingPriority(SceneCamera->StreamingPriority_);
    }

    // Set Depth Shader For Shadow System
    DepthMapShader_ = Shaders_[ERS_FUNCTION_FindShaderByName(std::string("_DepthMap"), &Shaders_)].get();
    CubemapDepthShader_ = Shaders_[ERS_FUNCTION_FindShaderByName(std::string("_DepthCubeMap"), &Shaders_)].get();

    // Close Any Viewports That Aren't All Open
    int ViewportsToClose = -1;
    for (int i = 0; (long)i < (long)Viewports_.size(); i++) {
        if (!*Viewports_[i]->Enabled) {
            ViewportsToClose = i;
        }
    }
    if (ViewportsToClose != -1) {
        DeleteViewport(ViewportsToClose);
    }



    // Generate Shadows
    //DepthMapShader_ = Shaders_[ERS_FUNCTION_FindShaderByName(std::string("Preview Shader"), &Shaders_)].get();
    if (Viewports_.size() > 0) {
        ShadowMaps_->UpdateShadowMaps(DepthMapShader_, CubemapDepthShader_, Viewports_[0]->Camera->GetPosition());
    }

    // Setup Vars
    glEnable(GL_DEPTH_TEST);
    CaptureCursor_ = false;
    CaptureIndex_ = -1;
    FrameNumber_++;



    RunTime_ = glfwGetTime() - GameStartTime_;
    SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->UpdateSystemInfoData(RunTime_);


    // Iterate Through Viewports
    for (int i = 0; (long)i < (long)Viewports_.size(); i++) {
        UpdateViewport(i, SceneManager, DeltaTime);
    }

    // Update Mouse Capture State
    if (CaptureCursor_) {
        glfwSetInputMode(Window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(Window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }





    // Handle Window Input
    for (int i = 0; (long)i < (long)Viewports_.size(); i++) {

        // Get Input Processor
        ERS_CLASS_InputProcessor* InputProcessorInstance = Viewports_[i]->Processor.get();

        bool CaptureEnabled = false;
        if ((CaptureIndex_ == i) && (!Cursors3D_->IsUsing())) {         
            CaptureEnabled = true;
        }



        // Update Viewport Camera/Position/Etc.
        if (IsEditorMode_ || i != 0) {
            InputProcessorInstance->Process(DeltaTime, CaptureEnabled);
        } else {
            CaptureEnabled = false;
        }


    }
    CaptureCursor_ = false;

    // BIND To Default Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);




    // RUN SCRIPTS WHEN NOT IN EDITOR MODE
    if (!IsEditorMode_) {

        for (unsigned long i = 0; i < SceneManager->Scenes_[SceneManager->ActiveScene_]->Models.size(); i++) {

            // Get Model
            ERS_STRUCT_Model* Model = SceneManager->Scenes_[SceneManager->ActiveScene_]->Models[i].get();

            // Go Through All Scripts In Model
            for (unsigned long x = 0; x < Model->AttachedScriptIndexes_.size(); x++) {

                long ScriptIndex = Model->AttachedScriptIndexes_[x];
                std::string Code = ProjectUtils_->ProjectManager_->Project_.Scripts[ScriptIndex].Code_;

                bool Status;
                if (x == (unsigned long)SelectedScript_) {
                    Status = SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->ExecuteModelScript(Code, Model, DebugLog_);
                } else {
                    Status = SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->ExecuteModelScript(Code, Model);
                }

                if (!Status) {
                    IsEditorMode_ = true;
                }
            }
        }

        for (unsigned long i = 0; i < SceneManager->Scenes_[SceneManager->ActiveScene_]->PointLights.size(); i++) {

            // Get Model
            ERS_STRUCT_PointLight* Target = SceneManager->Scenes_[SceneManager->ActiveScene_]->PointLights[i].get();

            // Go Through All Scripts In Model
            for (unsigned long x = 0; x < Target->AttachedScriptIndexes_.size(); x++) {

                long ScriptIndex = Target->AttachedScriptIndexes_[x];
                std::string Code = ProjectUtils_->ProjectManager_->Project_.Scripts[ScriptIndex].Code_;

                bool Status;
                if (x == (unsigned long)SelectedScript_) {
                    Status = SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->ExecutePointLightScript(Code, Target, DebugLog_);
                } else {
                    Status = SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->ExecutePointLightScript(Code, Target);
                }

                if (!Status) {
                    IsEditorMode_ = true;
                }
            }
        }

        for (unsigned long i = 0; i < SceneManager->Scenes_[SceneManager->ActiveScene_]->DirectionalLights.size(); i++) {

            // Get Model
            ERS_STRUCT_DirectionalLight* Target = SceneManager->Scenes_[SceneManager->ActiveScene_]->DirectionalLights[i].get();

            // Go Through All Scripts In Model
            for (unsigned long x = 0; x < Target->AttachedScriptIndexes_.size(); x++) {

                long ScriptIndex = Target->AttachedScriptIndexes_[x];
                std::string Code = ProjectUtils_->ProjectManager_->Project_.Scripts[ScriptIndex].Code_;

                bool Status;
                if (x == (unsigned long)SelectedScript_) {
                    Status = SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->ExecuteDirectionalLightScript(Code, Target, DebugLog_);
                } else {
                    Status = SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->ExecuteDirectionalLightScript(Code, Target);
                }

                if (!Status) {
                    IsEditorMode_ = true;
                }
            }
        }

        for (unsigned long i = 0; i < SceneManager->Scenes_[SceneManager->ActiveScene_]->SpotLights.size(); i++) {

            // Get Model
            ERS_STRUCT_SpotLight* Target = SceneManager->Scenes_[SceneManager->ActiveScene_]->SpotLights[i].get();

            // Go Through All Scripts In Model
            for (unsigned long x = 0; x < Target->AttachedScriptIndexes_.size(); x++) {

                long ScriptIndex = Target->AttachedScriptIndexes_[x];
                std::string Code = ProjectUtils_->ProjectManager_->Project_.Scripts[ScriptIndex].Code_;

                bool Status;
                if (x == (unsigned long)SelectedScript_) {
                    Status = SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->ExecuteSpotLightScript(Code, Target, DebugLog_);
                } else {
                    Status = SystemUtils_->ERS_CLASS_PythonInterpreterIntegration_->ExecuteSpotLightScript(Code, Target);
                }

                if (!Status) {
                    IsEditorMode_ = true;
                }
            }
        }

    }

    // Reset Selected Script
    SelectedScript_ = -1;



}

void ERS_CLASS_VisualRenderer::SetScriptDebug(int Index, std::vector<std::string>* DebugLog) {

    SelectedScript_ = Index;
    DebugLog_ = DebugLog;

}

void ERS_CLASS_VisualRenderer::UpdateViewport(int Index, ERS_CLASS_SceneManager* SceneManager, float DeltaTime, bool DrawCursor) {

    //todo: check if is Viewport0. then check if the system is in running mode or editor mode. if it's both, then do the following:
    // on transition, store the current editor position / rotation of the camera
    // update the camera's position/rot to the scene's active scenecamera position/rot 
    // disable user input directly through the editor system (the user will have to handle this via the scripting system)

    // Get Vars
    ERS_STRUCT_Viewport* Viewport = Viewports_[Index].get();
    ERS_STRUCT_Scene* Scene = SceneManager->Scenes_[SceneManager->ActiveScene_].get();

    // Render To ImGui
    ImGuiWindowFlags Flags = ImGuiWindowFlags_None;
    if (Viewport->MenuEnabled) {
        Flags |= ImGuiWindowFlags_MenuBar;
    }

    bool Visible = ImGui::Begin(Viewport->Name.c_str(), Viewport->Enabled.get(), Flags);

    // Set Default Window Size
    ImGui::SetWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);


    // Check If Window Visible
    if (Visible) {

        // Handle Viewport Menu
        if (ImGui::IsKeyPressed(GLFW_KEY_GRAVE_ACCENT)) {
            Viewport->MenuEnabled = !Viewport->MenuEnabled;
        }
        ViewportMenu_->DrawMenu(Viewport, ShadowMaps_.get());


        // Calculate Window Position
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;

        int WindowTopLeftCornerX = vMin.x;
        int WindowTopLeftCornerY = vMin.y;
        int WindowBottomRightCornerX = vMax.x;
        int WindowBottomRightCornerY = vMax.y;


        // Get Window Input
        int RenderWidth = WindowBottomRightCornerX - WindowTopLeftCornerX;
        int RenderHeight = WindowBottomRightCornerY - WindowTopLeftCornerY;


        // Get Mouse Pos
        int MousePositionX = ImGui::GetMousePos().x;
        int MousePositionY = ImGui::GetMousePos().y;

        // Check If In Bounding Box
        bool MouseXInRange = (MousePositionX >= WindowTopLeftCornerX) && (MousePositionX < WindowBottomRightCornerX);
        bool MouseYInRange = (MousePositionY >= WindowTopLeftCornerY) && (MousePositionY < WindowBottomRightCornerY);
        bool MouseInRange = MouseXInRange && MouseYInRange;


        // Check If Input Enabled
        bool EnableCameraMovement = !Cursors3D_->IsUsing();
        if (ImGui::IsKeyDown(341)) { // Bind to left control key
            EnableCameraMovement = true;
        }

        bool EnableCursorCapture;
        if (EnableCameraMovement && ImGui::IsWindowFocused() && (MouseInRange | Viewport->WasSelected) && (glfwGetMouseButton(Window_, 0) == GLFW_PRESS)) {

            if (!IsEditorMode_ && Index == 0) {
                CaptureCursor_ = false;
            } else {
                CaptureCursor_ = true;
            }
            EnableCursorCapture = true;
            CaptureIndex_ = Index;
            Viewport->WasSelected = true;
        } else {
            EnableCursorCapture = false;
            Viewport->WasSelected = false;
        }





        glViewport(0, 0, RenderWidth, RenderHeight);
        glScissor(0, 0, RenderWidth, RenderHeight);


        // Resize Viewport If Needed
        if ((RenderWidth != Viewport->Width) || (RenderHeight != Viewport->Height)) {
            ResizeViewport(Index, RenderWidth, RenderHeight);
        }


        // Bind To Framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, Viewport->FramebufferObject);

        // Rendering Commands Here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update Camera
        float AspectRatio = (float)RenderWidth / (float)RenderHeight;
        Viewport->Camera->Update();
        Viewport->Camera->SetAspectRatio(AspectRatio);
        glm::mat4 Projection;
        glm::mat4 View;
        Viewport->Camera->GetMatrices(Projection, View);
        



        // Use Shader
        int ShaderIndex = Viewport->ShaderIndex;
        Shaders_[ShaderIndex]->MakeActive();

        // Update Shaders
        UpdateShader(DeltaTime, RenderWidth, RenderHeight, SceneManager, Viewport->Camera.get(), Projection, View, Viewport);

        


        // Update Cursor If Selection Changed
        if (Scene->HasSelectionChanged && DrawCursor && (Scene->SceneObjects_.size() != 0)) {

            // Get Selected Model
            int SelectedObject = Scene->SelectedObject;
            if ((unsigned int)SelectedObject >= Scene->SceneObjects_.size()) {
                SelectedObject = 0;
                Scene->SelectedObject = 0;
            }

            // Get LocRotScale
            glm::vec3 Position;        
            glm::vec3 Rotation;      
            glm::vec3 Scale;
            bool HasRotation = false;
            bool HasScale = false;

            if (Scene->SceneObjects_[SelectedObject].Type_ == std::string("Model")) {
                unsigned long ModelIndex = Scene->SceneObjects_[SelectedObject].Index_;
                Position = Scene->Models[ModelIndex]->ModelPosition;        
                Rotation = Scene->Models[ModelIndex]->ModelRotation;        
                Scale = Scene->Models[ModelIndex]->ModelScale;                
                HasRotation = true;
                HasScale = true;
            } else if (Scene->SceneObjects_[SelectedObject].Type_ == std::string("PointLight")) {
                unsigned long Index = Scene->SceneObjects_[SelectedObject].Index_;
                Position = Scene->PointLights[Index]->Pos;        
            } else if (Scene->SceneObjects_[SelectedObject].Type_ == std::string("DirectionalLight")) {
                unsigned long Index = Scene->SceneObjects_[SelectedObject].Index_;
                Position = Scene->DirectionalLights[Index]->Pos;        
                Rotation = Scene->DirectionalLights[Index]->Rot;    
                HasRotation = true;    
            } else if (Scene->SceneObjects_[SelectedObject].Type_ == std::string("SpotLight")) {
                unsigned long Index = Scene->SceneObjects_[SelectedObject].Index_;
                Position = Scene->SpotLights[Index]->Pos;        
                Rotation = Scene->SpotLights[Index]->Rot;    
                HasRotation = true;    
            } else if (Scene->SceneObjects_[SelectedObject].Type_ == std::string("SceneCamera")) {
                unsigned long Index = Scene->SceneObjects_[SelectedObject].Index_;
                Position = Scene->SceneCameras[Index]->Pos_;        
                Rotation = Scene->SceneCameras[Index]->Rot_;    
                HasRotation = true;    
            }


            // Set Cursor Position        
            Cursors3D_->SetLocRotScale(Position, Rotation, Scale, HasRotation, HasScale);

            // Indicate Selection Hasn't Changed
            Scene->HasSelectionChanged = false;
        }


        // // Update Camera Location If System Running
        // if (!IsEditorMode_ && Index == 0 && Scene->ActiveSceneCameraIndex != -1) {
        //     Viewport->Camera->SetPosition(Scene->SceneCameras[Scene->ActiveSceneCameraIndex]->Pos_);
        //     Viewport->Camera->SetRotation(Scene->SceneCameras[Scene->ActiveSceneCameraIndex]->Rot_);
            
        // }


        // Render
        std::vector<ERS_STRUCT_Shader*> ShaderPointers;
        for (unsigned int i = 0; i < Shaders_.size(); i++) {
            ShaderPointers.push_back(Shaders_[i].get());
        }
        MeshRenderer_->RenderScene(Scene, OpenGLDefaults_, ShaderPointers, ShaderIndex, *ShaderUniformData_);


        if (Viewport->GridEnabled) {
            Viewport->Grid->DrawGrid(View, Projection, Viewport->Camera->GetPosition());
        }
        if (Viewport->LightIcons) {
            Viewport->IconRenderer->Draw(Viewport->Camera.get(), SceneManager);
        }

        Viewport->BoundingBoxRenderer->SetDepthTest(Viewport->DisableBoundingBoxDepthTest_);
        Viewport->BoundingBoxRenderer->SetDrawMode(Viewport->WireframeBoundingBoxes_);
        if (Viewport->ShowBoundingBox_) {
            Viewport->BoundingBoxRenderer->DrawAll(Viewport->Camera.get(), Scene);
        }
        if (Scene->SceneObjects_.size() > 0) {
            if (Viewport->ShowBoxOnSelectedModel_ && Scene->SceneObjects_[Scene->SelectedObject].Type_ == std::string("Model")) {
                unsigned long ModelIndex = Scene->SceneObjects_[Scene->SelectedObject].Index_;
                Viewport->BoundingBoxRenderer->DrawModel(Viewport->Camera.get(), Scene->Models[ModelIndex].get());
            }
        }

        // Render Framebuffer To Window
        ImGui::GetWindowDrawList()->AddImage(
            (void*)(intptr_t)Viewport->FramebufferColorObject,
            ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowSize().x, ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );


        // Draw 3D Cursor
        if (Cursors3D_->HasStateChanged()) {
            if (ImGui::IsWindowHovered()) {
                ActiveViewportCursorIndex_ = Index;
            }
        }

        bool DrawCursor;
        Cursors3D_->SetGridSnap(Viewport->GridSnapAmountTranslate_, Viewport->GridSnapAmountRotate_, Viewport->GridSnapAmountScale_);
        if (Cursors3D_->IsUsing() && (ActiveViewportCursorIndex_ == Index)) {
            DrawCursor = true;
        } else if (!Cursors3D_->IsUsing()) {
            DrawCursor = true;
        } else {
            DrawCursor = false;
        }

        if (DrawCursor) {
            Cursors3D_->Draw(Viewport->Camera.get(), EnableCursorCapture, Viewport->ShowCube, Viewport->GizmoEnabled);
        } else {
            Cursors3D_->Draw(Viewport->Camera.get(), false, Viewport->ShowCube, false);

        }



        // Update Selected Object
        if (!Cursors3D_->HasObjectChanged_) {
            SceneManager->UpdateLocRotScale(Cursors3D_->Pos_, Cursors3D_->Rot_, Cursors3D_->Scale_);
        } else {
            Cursors3D_->HasObjectChanged_ = false;
            
        }


        ViewportOverlay_->DrawOverlay(Viewport);


    }
    

    ImGui::End();
}

void ERS_CLASS_VisualRenderer::ResizeViewport(int Index, int Width, int Height) {

    // Update Render Color Buffer Size
    glBindTexture(GL_TEXTURE_2D, Viewports_[Index]->FramebufferColorObject);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);


    // Update RBO Size
    glBindRenderbuffer(GL_RENDERBUFFER, Viewports_[Index]->RenderbufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);

}

void ERS_CLASS_VisualRenderer::DeleteViewport(int Index) {

    // Log Deletion
    SystemUtils_->Logger_->Log(std::string(std::string("Destroying Viewport '") + Viewports_[Index]->Name + std::string("'")).c_str(), 5);

    // Cleanup OpenGL Objects
    glDeleteFramebuffers(1, &Viewports_[Index]->FramebufferObject);
    glDeleteTextures(1, &Viewports_[Index]->FramebufferColorObject);
    glDeleteRenderbuffers(1, &Viewports_[Index]->RenderbufferObject);

    // Delete Viewport Struct
    Viewports_.erase(Viewports_.begin() + Index);

}

void ERS_CLASS_VisualRenderer::CreateViewport() {

    // Generate Name
    std::string Name = std::string("Viewport ") + std::to_string(Viewports_.size() + 1);

    // Create Viewport
    CreateViewport(Name);

}

void ERS_CLASS_VisualRenderer::CreateViewport(std::string ViewportName) {


    // Log Creation
    SystemUtils_->Logger_->Log(std::string(std::string("Creating New Viewport '") + ViewportName + std::string("'")).c_str(), 5);


    // Create Viewport Struct
    std::shared_ptr<ERS_STRUCT_Viewport> Viewport = std::make_shared<ERS_STRUCT_Viewport>();


    // Populate Viewport Struct
    Viewport->ShaderIndex = DefaultShader_;
    
    Viewport->Camera = std::make_unique<ERS_STRUCT_Camera>();
    // Viewport->EditorCamera = std::make_unique<ERS_STRUCT_EditorCamera>();
    // Viewport->EditorCamera->SetupCamera(Viewport->Camera.get());

    Viewport->Grid = std::make_unique<ERS_CLASS_Grid>(SystemUtils_, Shaders_[ERS_FUNCTION_FindShaderByName(std::string("_Grid"), &Shaders_)].get());
    Viewport->IconRenderer = std::make_unique<ERS_CLASS_IconRenderer>(OpenGLDefaults_, SystemUtils_, Shaders_[ERS_FUNCTION_FindShaderByName(std::string("_LightIcon"), &Shaders_)].get()); //Set TO Shader 19 For Billboard Shader, Temp. Disabled As It Doesn't Work ATM
    Viewport->BoundingBoxRenderer = std::make_unique<ERS_CLASS_BoundingBoxRenderer>(SystemUtils_, Shaders_[ERS_FUNCTION_FindShaderByName(std::string("_BoundingBox"), &Shaders_)].get());
    Viewport->Name = ViewportName;
    
    Viewport->Width = 1;
    Viewport->Height = 1;

    Viewport->WasSelected = false;
    Viewport->Enabled = std::make_unique<bool>(true);



    // Create Input Processor
    SystemUtils_->Logger_->Log("Creating New Input Processor", 4);
    Viewport->Processor = std::make_unique<ERS_CLASS_InputProcessor>(Viewport->Camera.get(), Window_);

    // Create Framebuffer
    unsigned int FramebufferObject;
    SystemUtils_->Logger_->Log("Creating Framebuffer Object", 4);
    glGenFramebuffers(1, &FramebufferObject);


    // Bind To Framebuffer
    SystemUtils_->Logger_->Log("Binding To Framebuffer Object", 4);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferObject);
    Viewport->FramebufferObject = FramebufferObject;


    // Create RenderTexture
    unsigned int FramebufferColorObject;
    SystemUtils_->Logger_->Log("Creating Render Texture", 4);
    glGenTextures(1, &FramebufferColorObject);
    glBindTexture(GL_TEXTURE_2D, FramebufferColorObject);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // NOTE: THIS MUST HAPPEN ON RESIZE!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Viewport->FramebufferColorObject = FramebufferColorObject;


    // Attach Texture To Framebuffer
    SystemUtils_->Logger_->Log("Attaching Texture To Framebuffer", 4);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FramebufferColorObject, 0);


    // Create Render Buffer
    unsigned int RenderbufferObject;
    SystemUtils_->Logger_->Log("Creating Render Buffer Object", 5);
    glGenRenderbuffers(1, &RenderbufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, RenderbufferObject);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 800); // RESIZE THIS WITH THE WINDOW!


    // Attach Renderbuffer to Depth And Stencil Attachment
    SystemUtils_->Logger_->Log("Attaching Render Buffer Object To Depth Stencil", 5);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderbufferObject);
    Viewport->RenderbufferObject = RenderbufferObject;


    // Check Framebuffer Status
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {

        // Log Error
        SystemUtils_->Logger_->Log("Failed To Initialize Framebuffer", 9);
    }


    // Add To Viewports Vector
    Viewports_.push_back(Viewport);

}

void ERS_CLASS_VisualRenderer::UpdateShader(float DeltaTime, int RenderWidth, int RenderHeight, 
ERS_CLASS_SceneManager*SceneManager, ERS_STRUCT_Camera* Camera, glm::mat4 Projection, glm::mat4 View,
ERS_STRUCT_Viewport* Viewport) {

    // Clear Shader Uniform Data
    ShaderUniformData_ = std::make_unique<ERS_STRUCT_ShaderUniformData>();

    // Set Metadata Params
    float Time = glfwGetTime();
    ShaderUniformData_->Time_ = Time;
    ShaderUniformData_->FrameTime_ = DeltaTime;
    ShaderUniformData_->FrameNumber_ = FrameNumber_;
    ShaderUniformData_->ViewportRes_ = glm::vec2(RenderWidth, RenderHeight);
    ShaderUniformData_->CameraPosition_ = Camera->GetPosition();
    ShaderUniformData_->ShininessOffset_ = 0.5f;


    // Viewport Config
    ShaderUniformData_->Projection_ = Projection;
    ShaderUniformData_->View_ = View;

    // Camera Info
    ShaderUniformData_->GammaCorrectionEnabled_ = Viewport->GammaCorrection;
    ShaderUniformData_->Gamma_ = Viewport->Gamma_;
    ShaderUniformData_->HDREnabled_ = Viewport->HDREnabled_;
    ShaderUniformData_->Exposure_ = Viewport->Exposure_;


    // Set Shadow Filter Info
    int ShadowFilterType = 0;
    ERS::Renderer::ShadowFilteringType ShadowFilterEnum = SystemUtils_->RendererSettings_->ShadowFilteringType_;
    if (ShadowFilterEnum == ERS::Renderer::ERS_SHADOW_FILTERING_DISABLED) {
        ShadowFilterType = 0;
    } else if (ShadowFilterEnum == ERS::Renderer::ERS_SHADOW_FILTERING_PCF) {
        ShadowFilterType = 1;
    } else if (ShadowFilterEnum == ERS::Renderer::ERS_SHADOW_FILTERING_POISSON_SAMPLING) {
        ShadowFilterType = 2;
    } else if (ShadowFilterEnum == ERS::Renderer::ERS_SHADOW_FILTERING_STRATIFIED_POISSON_SAMPLING) {
        ShadowFilterType = 3;
    }
    ShaderUniformData_->ShadowFilterType_ = ShadowFilterType;
    ShaderUniformData_->ShadowFilterKernelSize_ = SystemUtils_->RendererSettings_->ShadowFilterKernelSize_;
    ShaderUniformData_->DepthMapArray_ = ShadowMaps_->ERS_CLASS_DepthMaps_->DepthTextureArrayID_;
    ShaderUniformData_->DepthCubemapArray_ = ShadowMaps_->ERS_CLASS_DepthMaps_->DepthTextureCubemapArrayID_;

    // ---- SEND LIGHTING INFORMATION TO SHADERS ---- //
    // NOTE: Due to limitations with shaders, the maximum number of lights is as follows (per object) 
    // Directional lights: 4
    // Point Lights: 32
    // Spot Lights: 16
    // TO DO BELOW:
    // When these limitations are reached, ERS will remove the lights that are farthest from the object

    // Prepare To Handle Lights
    //const int DirectionalLightLimit = 4;
    //const int PointLightLimit = 64;
    //const int SpotLightLimit = 32;


    ERS_STRUCT_Scene* ActiveScene = SceneManager->Scenes_[SceneManager->ActiveScene_].get();


    // ~-------------------------------------------------------------------!!!!!!!!!!!!!!!FIXME: IMPLEMENT SYSTEM TO USE THE LIGHTS CLOSEST TO THE OBJECT !!!!!!!!!!!!!!!!!!!!!~----------------------------------- //


    // Directional Lights
    int NumberDirectionalLights = ActiveScene->DirectionalLights.size();
    ShaderUniformData_->NumberDirectionalLights_ = NumberDirectionalLights;
    for (int i = 0; i < NumberDirectionalLights; i++) {
        ShaderUniformData_->DirectionalLights_.push_back(ERS_STRUCT_ShaderUniformDataDirectionalLight());
        ShaderUniformData_->DirectionalLights_[i].Direction_         = ERS_FUNCTION_ConvertRotationToFrontVector(ActiveScene->DirectionalLights[i]->Rot);
        ShaderUniformData_->DirectionalLights_[i].Color_             = ActiveScene->DirectionalLights[i]->Color;
        ShaderUniformData_->DirectionalLights_[i].Intensity_         = ActiveScene->DirectionalLights[i]->Intensity;
        ShaderUniformData_->DirectionalLights_[i].MaxDistance_       = ActiveScene->DirectionalLights[i]->MaxDistance;
        ShaderUniformData_->DirectionalLights_[i].CastsShadows_      = ActiveScene->DirectionalLights[i]->CastsShadows_;
        ShaderUniformData_->DirectionalLights_[i].DepthMapIndex_     = ActiveScene->DirectionalLights[i]->DepthMap.DepthMapTextureIndex;
        ShaderUniformData_->DirectionalLights_[i].LightSpaceMatrix_  = ActiveScene->DirectionalLights[i]->DepthMap.TransformationMatrix;
    }

    // Point Lights
    int NumberPointLights = ActiveScene->PointLights.size();
    ShaderUniformData_->NumberPointLights_ = NumberPointLights;
    for (int i = 0; i < NumberPointLights; i++) {
        ShaderUniformData_->PointLights_.push_back(ERS_STRUCT_ShaderUniformDataPointLight());
        ShaderUniformData_->PointLights_[i].Position_           = ActiveScene->PointLights[i]->Pos;
        ShaderUniformData_->PointLights_[i].Intensity_          = ActiveScene->PointLights[i]->Intensity;
        ShaderUniformData_->PointLights_[i].Color_              = ActiveScene->PointLights[i]->Color;
        ShaderUniformData_->PointLights_[i].MaxDistance_        = ActiveScene->PointLights[i]->MaxDistance;
        ShaderUniformData_->PointLights_[i].CastsShadows_       = ActiveScene->PointLights[i]->CastsShadows_;
        ShaderUniformData_->PointLights_[i].DepthCubemapIndex_  = ActiveScene->PointLights[i]->DepthMap.DepthMapTextureIndex;
    }

    // Spot Lights
    int NumberSpotLights = ActiveScene->SpotLights.size();
    ShaderUniformData_->NumberSpotLights_ = NumberSpotLights;
    for (int i = 0; i < NumberSpotLights; i++) {
        ShaderUniformData_->SpotLights_.push_back(ERS_STRUCT_ShaderUniformDataSpotLight());
        ShaderUniformData_->SpotLights_[i].Position_ = ActiveScene->SpotLights[i]->Pos;
        ShaderUniformData_->SpotLights_[i].Direction_ = ERS_FUNCTION_ConvertRotationToFrontVector(ActiveScene->SpotLights[i]->Rot);
        ShaderUniformData_->SpotLights_[i].Intensity_ = ActiveScene->SpotLights[i]->Intensity;
        ShaderUniformData_->SpotLights_[i].CutOff_ = 1.0f - (ActiveScene->SpotLights[i]->CutOff * (0.01745329 / 4));
        ShaderUniformData_->SpotLights_[i].RollOff_ = glm::radians(ActiveScene->SpotLights[i]->Rolloff);
        ShaderUniformData_->SpotLights_[i].Color_ = ActiveScene->SpotLights[i]->Color;
        ShaderUniformData_->SpotLights_[i].MaxDistance_ = ActiveScene->SpotLights[i]->MaxDistance;
        ShaderUniformData_->SpotLights_[i].CastsShadows_ = ActiveScene->SpotLights[i]->CastsShadows_;
        ShaderUniformData_->SpotLights_[i].DepthMapIndex_ = ActiveScene->SpotLights[i]->DepthMap.DepthMapTextureIndex;
        ShaderUniformData_->SpotLights_[i].LightSpaceMatrix_ = ActiveScene->SpotLights[i]->DepthMap.TransformationMatrix;
    }
}


//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include "ERS_CLASS_InputProcessor.h"


// Constructor / Destructor
ERS_CLASS_InputProcessor::ERS_CLASS_InputProcessor(ERS_STRUCT_Camera* Camera, GLFWwindow *Window) {

    Camera_ = Camera;
    Window_ = Window;
    //FramebufferManager_ = FramebufferManager;

}
ERS_CLASS_InputProcessor::~ERS_CLASS_InputProcessor() {
}


// Processing Functions
void ERS_CLASS_InputProcessor::ProcessMouseScroll(bool CaptureEnabled) {

    // Get Mouse Scroll Info
    float MouseScrollX = ImGui::GetIO().MouseWheel;
    //float MouseScrollY = ImGui::GetIO().MouseWheelH;


    // If Input Processing Enabled
    if (CaptureEnabled) {

        // Pass To Callback
        ScrollCallback(MouseScrollX);

    }

}
void ERS_CLASS_InputProcessor::Process(float DeltaTime, bool CaptureEnabled) {

    // Update Internal State
    ProcessKeyboardInput (DeltaTime, CaptureEnabled);
    UpdateFramebuffer    ();
    UpdateMouse          (CaptureEnabled);
    ProcessMouseScroll   (CaptureEnabled);

    // Update Associated Camera
    if (CaptureEnabled || ForceUpdate_) {
        ForceUpdate_ = false;
        Camera_->SetPosition      (Position_);
        Camera_->SetRotation      (Orientation_);
        Camera_->SetFOV           (FOV_);
        Camera_->SetClipBoundries (NearClip_, FarClip_);
        Camera_->Update           ();
    }

}
void ERS_CLASS_InputProcessor::UpdateFramebuffer() {

    // Get Window Size
    int WindowX;
    int WindowY;
    glfwGetFramebufferSize(Window_, &WindowX, &WindowY);

    // Call UpdateFramebuffer
    FramebufferSizeCallback(WindowX, WindowY);

}
void ERS_CLASS_InputProcessor::UpdateMouse( bool WindowMouseCaptureEnabled) {

    // Get Mouse Position
    double MouseX;
    double MouseY;
    glfwGetCursorPos(Window_, &MouseX, &MouseY);

    // Call Update
    if (WindowMouseCaptureEnabled) {
        MouseCallback(MouseX, MouseY);
    } else {
        LastX = MouseX;
        LastY = MouseY;

    }

}
void ERS_CLASS_InputProcessor::ProcessKeyboardInput(float DeltaTime, bool WindowCaptureEnabled) {


    // Get Keyboard Input
    if (WindowCaptureEnabled) {

        if (glfwGetKey(Window_, GLFW_KEY_W)          == GLFW_PRESS)
            ProcessKey(FORWARD,  DeltaTime);
        if (glfwGetKey(Window_, GLFW_KEY_S)          == GLFW_PRESS)
            ProcessKey(BACKWARD, DeltaTime);
        if (glfwGetKey(Window_, GLFW_KEY_A)          == GLFW_PRESS)
            ProcessKey(LEFT,     DeltaTime);
        if (glfwGetKey(Window_, GLFW_KEY_D)          == GLFW_PRESS)
            ProcessKey(RIGHT,    DeltaTime);
        if (glfwGetKey(Window_, GLFW_KEY_SPACE)      == GLFW_PRESS)
            ProcessKey(UP,       DeltaTime);
        if (glfwGetKey(Window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            ProcessKey(DOWN,     DeltaTime);

    }

}
void ERS_CLASS_InputProcessor::ProcessKey(CameraMovement Direction, float DeltaTime) {

    // Calculate Movement Direction Vectors
    glm::mat4 Perspective, ViewMatrix;
    glm::vec3 Right, Front, Up;
    Camera_->GetMatrices(Perspective, ViewMatrix);
    Right  =  glm::normalize(glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]));
    Up     =  glm::normalize(glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]));
    Front  = -glm::normalize(glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]));

    // Calculate Velocity
    float Velocity = MovementSpeed_ * DeltaTime;

    // Update Position(s)
    if (Direction == FORWARD)
        Position_ += Front  * Velocity;
    if (Direction == BACKWARD)
        Position_ -= Front  * Velocity;
    if (Direction == LEFT)
        Position_ -= Right  * Velocity;
    if (Direction == RIGHT)
        Position_ += Right  * Velocity;
    if (Direction == UP)
        Position_ += Up     * Velocity;
    if (Direction == DOWN)
        Position_ -= Up     * Velocity;

}
void ERS_CLASS_InputProcessor::FramebufferSizeCallback(int Width, int Height) {

    // Update Viewport
    glViewport(0, 0, Width, Height);
    glScissor(0, 0, Width, Height);

}
void ERS_CLASS_InputProcessor::MouseCallback(double XPos, double YPos) {

    // Update Positions
    if (FirstMouse) {

        LastX = XPos;
        LastY = YPos;

        FirstMouse = false;

    }

    // Calculate Offsets
    float XOffset = XPos - LastX;
    float YOffset = YPos - LastY;

    // Update Last Positions
    LastX = XPos;
    LastY = YPos;



    // Change Offset By Sensitivity
    XOffset *= MouseSensitivity_;
    YOffset *= MouseSensitivity_;

    // Update Pitch/Yaw
    Orientation_.y += XOffset;
    Orientation_.p += YOffset;



    // Bound Pitch
    if (ConstrainPitch_) {

        if (Orientation_.p > 89.0f) {
            Orientation_.p = 89.0f;
        }
        if (Orientation_.p < -89.0f) {
            Orientation_.p = -89.0f;
        }
    }


}
void ERS_CLASS_InputProcessor::ScrollCallback(double YOffset) {

    // Update Movement Speed
    MovementSpeed_ += (MovementSpeed_*(float)YOffset/10.0f);

    // Adjust Movement Speed
    if (MovementSpeed_ < MinMovementSpeed_)
        MovementSpeed_ = MinMovementSpeed_;
    if (MovementSpeed_ > MaxMovementSpeed_)
        MovementSpeed_ = MaxMovementSpeed_;


}




// Camera Parameter Helper Functions
void ERS_CLASS_InputProcessor::SetMovementSpeedBoundries(float MinSpeed, float MaxSpeed) {
    MinMovementSpeed_ = MinSpeed;
    MaxMovementSpeed_ = MaxSpeed;
}
void ERS_CLASS_InputProcessor::GetMovementSpeedBoundries(float &MinSpeed, float &MaxSpeed) {
    MinSpeed = MinMovementSpeed_;
    MaxSpeed = MaxMovementSpeed_;
}
void ERS_CLASS_InputProcessor::GetMovementSpeed(float &Speed) {
    Speed = MovementSpeed_;
}
void ERS_CLASS_InputProcessor::SetMovementSpeed(float Speed, bool EnforceSpeedBoundries) {
    if (EnforceSpeedBoundries) {
        Speed = std::max(MinMovementSpeed_, Speed);
        Speed = std::min(MaxMovementSpeed_, Speed);
        MovementSpeed_ = Speed;
    } else {
        MovementSpeed_ = Speed;
    }
}
void ERS_CLASS_InputProcessor::GetMouseSensitivity(float &Sensitivity) {
    Sensitivity = MouseSensitivity_;
}
void ERS_CLASS_InputProcessor::SetMouseSensitivity(float Sensitivity) {
    MouseSensitivity_ = Sensitivity;
}
void ERS_CLASS_InputProcessor::SetRotation(glm::vec3 Rotation) {
    Orientation_ = Rotation;
}
void ERS_CLASS_InputProcessor::GetRotation(glm::vec3 &Rotation) {
    Rotation = Orientation_;
}
glm::vec3 ERS_CLASS_InputProcessor::GetRotation() {
    return Orientation_;
}
void ERS_CLASS_InputProcessor::SetPosition(glm::vec3 Position) {
    Position_ = Position;
}
void ERS_CLASS_InputProcessor::GetPosition(glm::vec3 &Position) {
    Position = Position_;
}
glm::vec3 ERS_CLASS_InputProcessor::GetPosition() {
    return Position_;
}
void ERS_CLASS_InputProcessor::GetPitchConstraint(bool &ConstrainPitch) {
    ConstrainPitch = ConstrainPitch_;
}
bool ERS_CLASS_InputProcessor::GetPitchConstraint() {
    return ConstrainPitch_;
}
void ERS_CLASS_InputProcessor::SetPitchConstraint(bool ConstrainPitch) {
    ConstrainPitch_ = ConstrainPitch;
}
void ERS_CLASS_InputProcessor::SetClipBoundries(float NearClip, float FarClip) {
    NearClip_ = NearClip;
    FarClip_ = FarClip;
}
void ERS_CLASS_InputProcessor::GetClipBoundires(float &NearClip, float &FarClip) {
    NearClip = NearClip_;
    FarClip = FarClip_;
}
void ERS_CLASS_InputProcessor::SetFOV(float FOV) {
    FOV_ = FOV;
}
void ERS_CLASS_InputProcessor::GetFOV(float &FOV) {
    FOV = FOV_;
}
float ERS_CLASS_InputProcessor::GetFOV() {
    return FOV_;
}
void ERS_CLASS_InputProcessor::SetForceUpdate() {
    ForceUpdate_ = true;
}
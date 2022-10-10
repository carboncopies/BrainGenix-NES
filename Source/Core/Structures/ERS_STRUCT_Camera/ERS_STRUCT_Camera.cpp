//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_STRUCT_Camera.h>


// Constructor/Destructor
ERS_STRUCT_Camera::ERS_STRUCT_Camera() {
}
ERS_STRUCT_Camera::~ERS_STRUCT_Camera() {

}


// Update Matricies
void ERS_STRUCT_Camera::Update() {
    
    // Recalculate Orientation Quat
    glm::quat QuatOrientation;
    QuatOrientation *= glm::angleAxis(glm::radians(Orientation_.r), glm::vec3(0, 0, 1) * QuatOrientation);
    QuatOrientation *= glm::angleAxis(glm::radians(Orientation_.y), glm::vec3(0, 1, 0) * QuatOrientation);
    QuatOrientation *= glm::angleAxis(glm::radians(Orientation_.p), glm::vec3(1, 0, 0) * QuatOrientation);
    QuatOrientation = glm::normalize(QuatOrientation);

    // Update Matricies
    PerspectiveMatrix_          = glm::perspective(glm::radians(FOV_), AspectRatio_, NearClip_, FarClip_);	
    glm::mat4 RotationMatrix    = glm::mat4_cast(QuatOrientation);
    glm::mat4 TranslationMatrix = glm::mat4(1.0f);
    TranslationMatrix           = glm::translate(TranslationMatrix, -Position_);
    ViewMatrix_                 = RotationMatrix * TranslationMatrix;

    // Calculate Movement Direction Vectors
    Right_  =  glm::normalize(glm::vec3(ViewMatrix_[0][0], ViewMatrix_[1][0], ViewMatrix_[2][0]));
    Up_     =  glm::normalize(glm::vec3(ViewMatrix_[0][1], ViewMatrix_[1][1], ViewMatrix_[2][1]));
    Front_  = -glm::normalize(glm::vec3(ViewMatrix_[0][2], ViewMatrix_[1][2], ViewMatrix_[2][2]));
}


// Camera Parameter Helper Functions
void ERS_STRUCT_Camera::GetMatrices(glm::mat4& Perspective, glm::mat4& View){
    Perspective = PerspectiveMatrix_;
    View = ViewMatrix_;
}
void ERS_STRUCT_Camera::SetClipBoundries(float NearClip, float FarClip) {
    NearClip_ = NearClip;
    FarClip_ = FarClip;
}
void ERS_STRUCT_Camera::GetClipBoundires(float &NearClip, float &FarClip) {
    NearClip = NearClip_;
    FarClip = FarClip_;
}
void ERS_STRUCT_Camera::SetFOV(float FOV) {
    FOV_ = FOV;
}
void ERS_STRUCT_Camera::GetFOV(float &FOV) {
    FOV = FOV_;
}
float ERS_STRUCT_Camera::GetFOV() {
    return FOV_;
}
void ERS_STRUCT_Camera::SetAspectRatio(float AspectRatio) {
    AspectRatio_ = AspectRatio;
}
void ERS_STRUCT_Camera::SetRotation(glm::vec3 Rotation) {
    Orientation_ = Rotation;
}
void ERS_STRUCT_Camera::GetRotation(glm::vec3 &Rotation) {
    Rotation = Orientation_;
}
glm::vec3 ERS_STRUCT_Camera::GetRotation() {
    return Orientation_;
}
void ERS_STRUCT_Camera::SetPosition(glm::vec3 Position) {
    Position_ = Position;
}
void ERS_STRUCT_Camera::GetPosition(glm::vec3 &Position) {
    Position = Position_;
}
glm::vec3 ERS_STRUCT_Camera::GetPosition() {
    return Position_;
}
void ERS_STRUCT_Camera::GetStreamingPriority(int &StreamingPriority) {
    StreamingPriority = StreamingPriority_;
}
int ERS_STRUCT_Camera::GetStreamingPriority() {
    return StreamingPriority_;
}
void ERS_STRUCT_Camera::SetStreamingPriority(int StreamingPriority) {
    StreamingPriority_ = StreamingPriority;
}

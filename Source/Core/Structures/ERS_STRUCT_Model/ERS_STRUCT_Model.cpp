//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//


#include "ERS_STRUCT_Model.h"


void ERS_STRUCT_Model::SetPosition(double X, double Y, double Z) {

    glm::vec3 Position = glm::vec3(X, Y, Z);
    ModelPosition = Position;

}

void ERS_STRUCT_Model::SetPosition(glm::vec3 Position) {

    ModelPosition = Position;

}

void ERS_STRUCT_Model::SetRotation(double X, double Y, double Z) {

    ModelRotation = glm::vec3((float)X, (float)Y, (float)Z);

}

void ERS_STRUCT_Model::SetRotation(glm::vec3 Rotation) {

    ModelRotation = Rotation;

}


void ERS_STRUCT_Model::SetScale(double X, double Y, double Z) {

    glm::vec3 Scale = glm::vec3(X, Y, Z);
    ModelScale = Scale;
    TrueModelScale = ModelScale / 100.0f;

}

void ERS_STRUCT_Model::SetScale(glm::vec3 Scale) {

    ModelScale = Scale;
    TrueModelScale = ModelScale / 100.0f;

}


void ERS_STRUCT_Model::SetLocRotScale(double LocX, double LocY, double LocZ, double RotX, double RotY, double RotZ, double ScaleX, double ScaleY, double ScaleZ) {

    glm::vec3 Position = glm::vec3(LocX, LocY, LocZ);
    glm::vec3 Rotation = glm::vec3(RotX, RotY, RotZ);
    glm::vec3 Scale = glm::vec3(ScaleX, ScaleY, ScaleZ);

    ModelPosition = Position;
    ModelRotation = Rotation;
    ModelScale = Scale;
    TrueModelScale = ModelScale / 100.0f;

}


void ERS_STRUCT_Model::SetLocRotScale(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale) {

    ModelPosition = Position;
    ModelRotation = Rotation;
    ModelScale = Scale;
    TrueModelScale = ModelScale / 100.0f;
}


bool ERS_STRUCT_Model::ApplyTransformations() {

    // Reset ModelLocRotScale
    ModelLocRotScale_ = SourceModelLocRotScale_;



    // Apply Transforms
    ModelLocRotScale_ = glm::translate(ModelLocRotScale_, ModelPosition);
    ModelLocRotScale_ = glm::rotate(ModelLocRotScale_, glm::radians(ModelRotation[2]), glm::vec3(0, 0, 1));
    ModelLocRotScale_ = glm::rotate(ModelLocRotScale_, glm::radians(ModelRotation[1]), glm::vec3(0, 1, 0));
    ModelLocRotScale_ = glm::rotate(ModelLocRotScale_, glm::radians(ModelRotation[0]), glm::vec3(1, 0, 0));
    ModelLocRotScale_ = glm::scale(ModelLocRotScale_, TrueModelScale);

    
    return true;

}

glm::mat4 ERS_STRUCT_Model::GetMat4() {

    return ModelLocRotScale_;

}


void ERS_STRUCT_Model::UpdateMeshTransparency() {

    for (unsigned long i = 0; i < Meshes.size(); i++) {
        Meshes[i].UpdateTransparencyStatus();
    }

}
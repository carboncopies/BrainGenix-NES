//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CLASS_ModelLoader.h>

// FIXME:
// Cleanup Doxygen, fix headers, etc.
// optionally optimize processgpu function to provide least amount of lag as possible
// Add Placeholder meshes during loading

ERS_CLASS_ModelLoader::ERS_CLASS_ModelLoader(ERS_STRUCT_SystemUtils* SystemUtils, int MaxModelLoadingThreads) {

    SystemUtils_ = SystemUtils;
    SystemUtils_->Logger_->Log("Initializing Model Loader", 5);
    FreeImage_Initialise();



    AssetStreamingManager_ = std::make_unique<ERS_CLASS_AssetStreamingManager>(SystemUtils_);
    


    if (MaxModelLoadingThreads == -1) {
        SystemUtils_->Logger_->Log("Identifying Number Of CPU Cores", 4);
        MaxModelLoadingThreads = std::thread::hardware_concurrency();
        SystemUtils_->Logger_->Log(std::string(std::string("Identified ") + std::to_string(MaxModelLoadingThreads) + std::string(" Cores")).c_str(), 4);
    }
    SystemUtils_->Logger_->Log(std::string(std::string("Creating ") + std::to_string(MaxModelLoadingThreads) + std::string("Model Loading Threads")).c_str(), 4);
    for (int i = 0; i < MaxModelLoadingThreads; i++) {
        SystemUtils_->Logger_->Log(std::string(std::string("Creating Worker Thread ") + std::to_string(i)).c_str(), 3);
        WorkerThreads_.push_back(std::thread(&ERS_CLASS_ModelLoader::WorkerThread, this, i));
    }

    //SystemUtils_->Logger_->Log("Creating Reference Loading Thread", 5);
    //ModelRefrenceThread_ = std::thread(&ERS_CLASS_ModelLoader::ReferenceThread, this);


}

ERS_CLASS_ModelLoader::~ERS_CLASS_ModelLoader() {

    SystemUtils_->Logger_->Log("ERS_CLASS_ModelLoader Destructor Called", 6);
    FreeImage_DeInitialise();



    // Shutdown Threads
    SystemUtils_->Logger_->Log("Sending Join Command To Worker Threads", 5);
    BlockThread_.lock();
    ExitThreads_ = true;
    BlockThread_.unlock();

    SystemUtils_->Logger_->Log("Joining Worker Threads", 6);
    for (int i = 0; (long)i < (long)WorkerThreads_.size(); i++) {
        SystemUtils_->Logger_->Log(std::string(std::string("Joining Worker Thread ") + std::to_string(i)).c_str(), 3);
        WorkerThreads_[i].join();
    }
    SystemUtils_->Logger_->Log("Finished Joining Worker Threads", 6);


    // SystemUtils_->Logger_->Log("Sending Join Command To Reference Thread", 5);
    // std::lock_guard<std::mutex> Deleteme(BlockRefThread_);
    // ExitRefThread_ = true;

    // SystemUtils_->Logger_->Log("Joining Reference Loader Thread", 5);
    // ModelRefrenceThread_.join();
    // SystemUtils_->Logger_->Log("Finished Joining Reference Loader Thread", 5);


}

void ERS_CLASS_ModelLoader::WorkerThread(int WorkerThreadNumber) {

    std::string ThreadName = std::string("ERS_GLT-") + std::to_string(WorkerThreadNumber);
    SetThreadName(ThreadName);

    bool ThreadShouldRun = true;
    while (ThreadShouldRun) {

        // Acquire Check Lock
        BlockThread_.lock();
        if (ExitThreads_) {
            ThreadShouldRun = false;
            BlockThread_.unlock();
        } else {

            // Check If Items In Work Queue
            int Size = WorkItems_.size();
            if (Size > 0) {

                // Get Item, Remove From Queue, Unlock
                std::shared_ptr<ERS_STRUCT_Model> WorkItem = WorkItems_[0];
                long WorkID = WorkIDs_[0];


                WorkItems_.erase(WorkItems_.begin());
                WorkIDs_.erase(WorkIDs_.begin());


                BlockThread_.unlock();

                // Process Item
                LoadModel(WorkID, WorkItem);

            } else {
                
                // No Work Items, Unlock Mutex, Sleep Thread
                BlockThread_.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));

            }
        }


    }

}

void ERS_CLASS_ModelLoader::ProcessNewModels(ERS_STRUCT_Scene* ActiveScene) {

    // Check List Of Models
    for (unsigned long i = 0; i <ActiveScene->Models.size(); i++) {
        
        if ( (ActiveScene->Models[i]->IsReadyForGPU) && !(ActiveScene->Models[i]->FullyReady) ) {

            SystemUtils_->Logger_->Log(std::string(std::string("Pushing Material Information To GPU For Asset: ") + std::to_string(ActiveScene->Models[i]->AssetID)).c_str(), 4);

            ProcessGPU(ActiveScene->Models[i]);
            ActiveScene->Models[i]->FullyReady = true;

        }

    }

}

void ERS_CLASS_ModelLoader::AddModelToLoadingQueue(std::shared_ptr<ERS_STRUCT_Model> Model) {
    AddModelToLoadingQueue(Model->AssetID, Model);
}

void ERS_CLASS_ModelLoader::AddModelToLoadingQueue(long AssetID, std::shared_ptr<ERS_STRUCT_Model> Model) {

    // Log Addition
    SystemUtils_->Logger_->Log(std::string(std::string("Adding Model '") + std::to_string(AssetID) + std::string("' To Load Queue")).c_str(), 4);

    // Add To Queue
    BlockThread_.lock();

    WorkIDs_.push_back(AssetID);
    WorkItems_.push_back(Model);
    

    BlockThread_.unlock();

}


void ERS_CLASS_ModelLoader::ProcessGPU(std::shared_ptr<ERS_STRUCT_Model> Model) {

        

    // // Push Textures To GPU RAM
    // for (unsigned long i = 0; i < Model->TexturesToPushToGPU_.size(); i++) {

    //     // Generate Texture
    //     unsigned int TextureID;
    //     glGenTextures(1, &TextureID);
    //     glBindTexture(GL_TEXTURE_2D, TextureID);

    //     // Set Texture Properties
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    //     // Convert FIBITMAP* To Raw Image Bytes
    //     unsigned char* RawImageData = Model->TexturesToPushToGPU_[i].ImageBytes;

    //     if (RawImageData != NULL) {
    //         if (Model->TexturesToPushToGPU_[i].FreeImageBackend) {
    //             if (Model->TexturesToPushToGPU_[i].Channels == 4) {
    //                 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Model->TexturesToPushToGPU_[i].Width, Model->TexturesToPushToGPU_[i].Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, RawImageData);
    //             } else if (Model->TexturesToPushToGPU_[i].Channels == 3) {
    //                 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Model->TexturesToPushToGPU_[i].Width, Model->TexturesToPushToGPU_[i].Height, 0, GL_BGR, GL_UNSIGNED_BYTE, RawImageData);
    //             } else if (Model->TexturesToPushToGPU_[i].Channels == 2) {
    //                 glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, Model->TexturesToPushToGPU_[i].Width, Model->TexturesToPushToGPU_[i].Height, 0, GL_RG, GL_UNSIGNED_BYTE, RawImageData);
    //             } else if (Model->TexturesToPushToGPU_[i].Channels == 1) {
    //                 glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Model->TexturesToPushToGPU_[i].Width, Model->TexturesToPushToGPU_[i].Height, 0, GL_RED, GL_UNSIGNED_BYTE, RawImageData);
    //             } else {
    //                 SystemUtils_->Logger_->Log(std::string("Texture With ID '") + Model->TexturesToPushToGPU_[i].Path + std::string("' For Model '") + Model->Name + std::string("' Has Unsupported Number Of Channels: ") + std::to_string(Model->TexturesToPushToGPU_[i].Channels), 8);
    //             }
    //         } else {
    //             if (Model->TexturesToPushToGPU_[i].Channels == 4) {
    //                 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Model->TexturesToPushToGPU_[i].Width, Model->TexturesToPushToGPU_[i].Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, RawImageData);
    //             } else if (Model->TexturesToPushToGPU_[i].Channels == 3) {
    //                 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Model->TexturesToPushToGPU_[i].Width, Model->TexturesToPushToGPU_[i].Height, 0, GL_RGB, GL_UNSIGNED_BYTE, RawImageData);
    //             } else if (Model->TexturesToPushToGPU_[i].Channels == 2) {
    //                 glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, Model->TexturesToPushToGPU_[i].Width, Model->TexturesToPushToGPU_[i].Height, 0, GL_RG, GL_UNSIGNED_BYTE, RawImageData);
    //             } else if (Model->TexturesToPushToGPU_[i].Channels == 1) {
    //                 glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Model->TexturesToPushToGPU_[i].Width, Model->TexturesToPushToGPU_[i].Height, 0, GL_RED, GL_UNSIGNED_BYTE, RawImageData);
    //             } else {
    //                 SystemUtils_->Logger_->Log(std::string("Texture With ID '") + Model->TexturesToPushToGPU_[i].Path + std::string("' For Model '") + Model->Name + std::string("' Has Unsupported Number Of Channels: ") + std::to_string(Model->TexturesToPushToGPU_[i].Channels), 8);
    //             }  
    //         }

    //         glGenerateMipmap(GL_TEXTURE_2D);

    //     } else {
    //         SystemUtils_->Logger_->Log("Texture Failed To Load, Cannot Push To GPU", 9);
    //     }



    //     // Unload Image Data
    //     if (Model->TexturesToPushToGPU_[i].FreeImageBackend) {
    //         FreeImage_Unload(Model->TexturesToPushToGPU_[i].ImageData);
    //     } else {
    //         stbi_image_free(Model->TexturesToPushToGPU_[i].ImageBytes);
    //     }

    //     // Append To Texture Index
    //     Model->OpenGLTextureIDs_.push_back(TextureID);
    // }



    // Erase List To Save Memory
    //Model->TexturesToPushToGPU_.erase(Model->TexturesToPushToGPU_.begin(), Model->TexturesToPushToGPU_.end());
    Model->IsTemplateModel = true;



    // Collect Vertex Count Analytics
    for (unsigned long i = 0; i < Model->Meshes.size(); i++) {
        Model->TotalVertices_ += Model->Meshes[i].Vertices.size();
        Model->TotalIndices_ += Model->Meshes[i].Indices.size();
    }

    // Process Texture References, Setup Meshes
    for (unsigned long i = 0; i < Model->Meshes.size(); i++) {

        // Set Shadow Configuration Pointers
        Model->Meshes[i].CastDynamicShadows_ = &Model->CastDynamicShadows_;
        Model->Meshes[i].CastStaticShadows_ = &Model->CastStaticShadows_;
        Model->Meshes[i].ReceiveShadows_ = &Model->ReceiveShadows_;

        Model->Meshes[i].TreatMissingTexturesAsTransparent_ = &Model->TreatMissingTexturesAsTransparent_;


        // for (unsigned long Index = 0; Index < Model->Meshes[i].TextureReferences_.size(); Index++) { // IF TEXTURES DONT WORK, CHECK HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        //     int TextureIndex = Model->Meshes[i].TextureReferences_[Index];
        
        //     if (TextureIndex == -1) {
        //         SystemUtils_->Logger_->Log("Failed To Find Corresponding Texture", 8);
        //         Model->Meshes[i].TextureIDs.push_back(-1);
        //         Model->Meshes[i].TextureColorChannels_.push_back(0);
        //     } else {
        //         Model->Meshes[i].TextureIDs.push_back(Model->OpenGLTextureIDs_[TextureIndex]);
        //         Model->Meshes[i].TextureColorChannels_.push_back(Model->TexturesToPushToGPU_[TextureIndex].Channels);
        //     }
    

        // }
        Model->Meshes[i].SetupMesh();
        // Perhaps save mem by erasing the vertices after pusning? (also indices)
    }


    Model->LoadingFinishTime_ = glfwGetTime();
    Model->TotalLoadingTime_ = Model->LoadingFinishTime_ - Model->LoadingStartTime_;
    double VertsPerSec = Model->TotalVertices_ / Model->TotalLoadingTime_;
    SystemUtils_->Logger_->Log(
        std::string("Model Loading Completed In ")
        + std::to_string(Model->TotalLoadingTime_)
        + std::string(" Seconds, ")
        + std::to_string(Model->TotalVertices_)
        + std::string(" Vertices, ")
        + std::to_string(VertsPerSec)
        + std::string(" Verts/Sec")
        ,5);



}


void ERS_CLASS_ModelLoader::ReferenceThread() {

    while (!ExitRefThread_) {

        // Extra Scope To Make The Lock Go Out Of Scope Before The Delay Runs
        {
            // Check Reference List
            std::lock_guard<std::mutex> LockReferenceThread(BlockRefThread_);
            for (unsigned long i = 0; i < ModelsToRefrence_.size(); i++) {
                unsigned long TargetID = ModelsToRefrence_[i]->AssetID;
                long MatchIndex = CheckIfModelAlreadyLoaded(TargetID);
                if (MatchIndex != -1) {
                    if (LoadedModelRefrences_[MatchIndex]->FullyReady) {


                        std::shared_ptr<ERS_STRUCT_Model> Target = ModelsToRefrence_[i];
                        std::shared_ptr<ERS_STRUCT_Model> Source = LoadedModelRefrences_[MatchIndex];



                        Target->Meshes = Source->Meshes;
                        //Target->OpenGLTextureIDs_ = Source->OpenGLTextureIDs_;
                        Target->TextureIDs = Source->TextureIDs;
                        Target->TotalIndices_ = Source->TotalIndices_;
                        Target->TotalVertices_ = Source->TotalVertices_;
                        Target->TotalLoadingTime_ = Source->TotalLoadingTime_;
                        Target->BoxOffset_ = Source->BoxOffset_;
                        Target->BoxScale_ = Source->BoxScale_;
                        Target->IsTemplateModel = false;
                        Target->FullyReady = true;
                        
                        Target->MaxTextureLevel_ = Source->MaxTextureLevel_;

                        if (Target->Name == "Loading...") {
                            Target->Name = Source->Name;
                        }

                        ModelsToRefrence_.erase(ModelsToRefrence_.begin() + i);
                        break;
                    }
                }

            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));



    }

}

void ERS_CLASS_ModelLoader::AddModelToReferenceQueue(long AssetID, std::shared_ptr<ERS_STRUCT_Model> Model) {

    Model->AssetID = AssetID;
    ModelsToRefrence_.push_back(Model);

}

void ERS_CLASS_ModelLoader::LoadModel(long AssetID, std::shared_ptr<ERS_STRUCT_Model> Model) {

    // Check If Already In Refs
    if (EnableReferenceLoading_) {
        std::lock_guard<std::mutex> BlockRefThread(BlockRefThread_);
        if (CheckIfModelAlreadyLoaded(AssetID) != -1) {
            AddModelToReferenceQueue(AssetID, Model);
            return;
        }
        else {
            LoadedModelRefrences_.push_back(Model);

        }
    }
    
    // Log Loading For Debugging Purposes
    //SystemUtils_->Logger_->Log(std::string(std::string("Loading Model '") + std::to_string(AssetID) + std::string("'")).c_str(), 4);
    Model->LoadingStartTime_ = glfwGetTime();

    // Read Metadata From Asset
    std::unique_ptr<ERS_STRUCT_IOData> ModelMetadata = std::make_unique<ERS_STRUCT_IOData>();
    SystemUtils_->ERS_IOSubsystem_->ReadAsset(AssetID, ModelMetadata.get());
    std::string ModelMetadataString = std::string((const char*)ModelMetadata->Data.get());
    YAML::Node Metadata = YAML::Load(ModelMetadataString);


    ERS_FUNCTION_DecodeModelMetadata(Metadata, Model.get(), SystemUtils_, AssetID, false);

    // Read Mesh
    Assimp::Importer Importer;
    //SystemUtils_->Logger_->Log(std::string(std::string("Loading Model With ID: ") + std::to_string(AssetID)).c_str(), 3);

    std::unique_ptr<ERS_STRUCT_IOData> ModelData = std::make_unique<ERS_STRUCT_IOData>();
    SystemUtils_->ERS_IOSubsystem_->ReadAsset(Model->ModelDataID, ModelData.get());
    const aiScene* Scene = Importer.ReadFileFromMemory(ModelData->Data.get(), (int)ModelData->Size_B, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices | aiProcess_JoinIdenticalVertices, "");

    // Log Errors
    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
        SystemUtils_->Logger_->Log(std::string(std::string("Model Loading Error: ") + std::string(Importer.GetErrorString())).c_str(), 10);
        Model->IsReadyForGPU = false;
        return;
    }

    // Decode Mesh, Create Texture Pointers
    ProcessNode(Model.get(), Scene->mRootNode, Scene);

    // Math Textures To Meshes
    MatchTextures(Model.get());


    // Set Ready For GPU
    Model->IsReadyForGPU = true;
}

void ERS_CLASS_ModelLoader::MatchTextures(ERS_STRUCT_Model* Model) {

    // Extract Information About Textures For Simplicity
    std::vector<ERS_STRUCT_Mesh*> Meshes;
    for (unsigned int i = 0; i < Model->Meshes.size(); i++) {
        Meshes.push_back(&Model->Meshes[i]);
    }

    // Match
    for (unsigned int MeshIndex = 0; MeshIndex < Meshes.size(); MeshIndex++) {
        ERS_STRUCT_Mesh* CurrentMesh = Meshes[MeshIndex];

        // Iterate Over Mesh's Requested Textures, Try To Match With What Model Has
        for (unsigned int TextureIndex = 0; TextureIndex < CurrentMesh->Loader_RequestedTextureInformation_.size(); TextureIndex++) {
            std::string TextureType = CurrentMesh->Loader_RequestedTextureInformation_[TextureIndex].first;
            std::string TextureIdentifier = CurrentMesh->Loader_RequestedTextureInformation_[TextureIndex].second;

            // Now, Search Model's Textures For Match
            for (unsigned int ModelTextureIndex = 0; ModelTextureIndex < Model->Textures_.size(); ModelTextureIndex++) {
                std::string ModelTextureIdentifier = Model->Textures_[ModelTextureIndex].Path;
                
                if (ModelTextureIdentifier == TextureIdentifier) {
                    Model->Textures_[ModelTextureIndex].Type = TextureType;
                    CurrentMesh->Textures_.push_back(&Model->Textures_[ModelTextureIndex]);
                    break;
                }

            }

        }

    }  

}


long ERS_CLASS_ModelLoader::CheckIfModelAlreadyLoaded(long AssetID) {

    long Index = -1;    

    // Iterate Through List OF Models Aready Loading/Loaded
    for (unsigned long i = 0; i < LoadedModelRefrences_.size(); i++) {
        LoadedModelRefrences_[i]->AssetID;
        if (LoadedModelRefrences_[i]->AssetID == AssetID) {
            Index = i;
        }
    }

    return Index;

}

void ERS_CLASS_ModelLoader::ProcessNode(ERS_STRUCT_Model* Model, aiNode *Node, const aiScene *Scene) {


    // Process Meshes In Current Node
    for (unsigned int i = 0; i < Node->mNumMeshes; i++) {
        aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[i]];
        ProcessMesh(
            Model,
            (unsigned long)Mesh->mNumVertices,
            (unsigned long)Mesh->mNumFaces*3,
            Mesh
        );

    }

    // Process Children Nodes
    for (unsigned int i = 0; i < Node->mNumChildren; i++) {
        ProcessNode(Model, Node->mChildren[i], Scene);
    }


}

ERS_STRUCT_Mesh ERS_CLASS_ModelLoader::ProcessMesh(ERS_STRUCT_Model* Model, unsigned long PreallocVertSize, unsigned long PreallocIndSize, aiMesh *Mesh) {

    // Create Data Holders
    ERS_STRUCT_Mesh OutputMesh;
    OutputMesh = Model->Meshes[Model->NumMeshes_];



    OutputMesh.Vertices.reserve(PreallocVertSize);
    OutputMesh.Indices.reserve(PreallocIndSize);

    // Iterate Through Meshes' Vertices
    for (unsigned int i = 0; i < Mesh->mNumVertices; i++) {

        // Hold Vertex Data
        ERS_STRUCT_Vertex Vertex;
        glm::vec3 Vector;


        Vector.x = Mesh->mVertices[i].x;
        Vector.y = Mesh->mVertices[i].y;
        Vector.z = Mesh->mVertices[i].z;
        Vertex.Position = Vector - Model->BoxOffset_;

        if (Mesh->HasNormals())
        {
            Vector.x = Mesh->mNormals[i].x;
            Vector.y = Mesh->mNormals[i].y;
            Vector.z = Mesh->mNormals[i].z;
            Vertex.Normal = Vector;
        }

        if (Mesh->mTextureCoords[0]) {

            glm::vec2 Vec;

            // Get Texture Coordinates
            Vec.x = Mesh->mTextureCoords[0][i].x;
            Vec.y = Mesh->mTextureCoords[0][i].y;
            Vertex.TexCoords = Vec;

            // Tangent
            Vector.x = Mesh->mTangents[i].x;
            Vector.y = Mesh->mTangents[i].y;
            Vector.z = Mesh->mTangents[i].z;
            Vertex.Tangent = Vector;

            // Bitangent
            Vector.x = Mesh->mBitangents[i].x;
            Vector.y = Mesh->mBitangents[i].y;
            Vector.z = Mesh->mBitangents[i].z;
            Vertex.Bitangent = Vector;

        } else {
            Vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        OutputMesh.Vertices.push_back(Vertex);



    }

    // Iterate Through Faces
    for (unsigned int i = 0; i < Mesh->mNumFaces; i++) {

        aiFace Face = Mesh->mFaces[i];

        // Get Face Indices
        for (unsigned int j = 0; j < Face.mNumIndices; j++) {
            OutputMesh.Indices.push_back(Face.mIndices[j]);
        }
    }

    // // Process Materials
    // aiMaterial* Material = Scene->mMaterials[Mesh->mMaterialIndex];
    // IdentifyMeshTextures(Material, &OutputMesh);


    // Return Populated Mesh
    Model->Meshes[Model->NumMeshes_] = OutputMesh;
    Model->NumMeshes_++;

    return OutputMesh;

}

void ERS_CLASS_ModelLoader::IdentifyMeshTextures(aiMaterial* Mat, ERS_STRUCT_Mesh* Mesh) {

    std::vector<std::pair<aiTextureType, std::string>> TextureTypes;
    TextureTypes.push_back(std::make_pair(aiTextureType_AMBIENT,           "texture_ambient"));
    TextureTypes.push_back(std::make_pair(aiTextureType_AMBIENT_OCCLUSION, "texture_ambient_occlusion"));
    TextureTypes.push_back(std::make_pair(aiTextureType_BASE_COLOR,        "texture_base_color"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DIFFUSE,           "texture_diffuse"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DIFFUSE_ROUGHNESS, "texture_diffuse_roughness"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DISPLACEMENT,      "texture_displacement"));
    TextureTypes.push_back(std::make_pair(aiTextureType_EMISSION_COLOR,    "texture_emission_color"));
    TextureTypes.push_back(std::make_pair(aiTextureType_EMISSIVE,          "texture_emissive"));
    TextureTypes.push_back(std::make_pair(aiTextureType_HEIGHT,            "texture_height"));
    TextureTypes.push_back(std::make_pair(aiTextureType_LIGHTMAP,          "texture_lightmap"));
    TextureTypes.push_back(std::make_pair(aiTextureType_METALNESS,         "texture_metalness"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NONE,              "texture_none"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NORMAL_CAMERA,     "texture_normal_camera"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NORMALS,           "texture_normals"));
    TextureTypes.push_back(std::make_pair(aiTextureType_OPACITY,           "texture_opacity"));
    TextureTypes.push_back(std::make_pair(aiTextureType_REFLECTION,        "texture_reflection"));
    TextureTypes.push_back(std::make_pair(aiTextureType_SHININESS,         "texture_shininess"));
    TextureTypes.push_back(std::make_pair(aiTextureType_SPECULAR,          "texture_specular"));
    TextureTypes.push_back(std::make_pair(aiTextureType_UNKNOWN,           "texture_unknown"));

    // Iterate Over All Texture Types
    for (unsigned int TextureTypeIndex = 0; TextureTypeIndex < TextureTypes.size(); TextureTypeIndex++) {

        aiTextureType Type = TextureTypes[TextureTypeIndex].first;
        std::string TypeName = TextureTypes[TextureTypeIndex].second;

        // Iterate Through Textures For This Type
        for (unsigned int i=0; i< Mat->GetTextureCount(Type); i++) {


            // Calculate Texture Path
            aiString TextureString;
            Mat->GetTexture(Type, i, &TextureString);
            std::string TextureIdentifier = std::string(std::string(TextureString.C_Str()));

            //std::string Message = std::string("Model Requesting Texture Of Type '") + TypeName + std::string("' With Identifier '") + TextureIdentifier + std::string("'");
            //SystemUtils_->Logger_->Log(Message, 3);

            // Search Texture List For Index Of Same Match, Add To List Of Unique Textures If Not Found
            bool AlreadyHasTexture = false;
            for (unsigned long x = 0; x < Mesh->Loader_RequestedTextureInformation_.size(); x++) {
                if (Mesh->Loader_RequestedTextureInformation_[x].second == TextureIdentifier) {
                    SystemUtils_->Logger_->Log(std::string("Found Matching Texture '") + Mesh->Loader_RequestedTextureInformation_[x].second + "'", 3);
                    AlreadyHasTexture = true;
                    break;
                }
            }

            // If It's Not Already In The List, Add IT
            if (!AlreadyHasTexture) {
                Mesh->Loader_RequestedTextureInformation_.push_back(std::make_pair(TypeName, TextureIdentifier));
            }


        }

    }

}


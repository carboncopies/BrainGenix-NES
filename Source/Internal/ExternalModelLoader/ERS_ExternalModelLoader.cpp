//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_ExternalModelLoader.h>


namespace BrainGenix {
namespace ERS {
namespace Module {

ExternalModelLoader::ExternalModelLoader(ERS_STRUCT_SystemUtils* SystemUtils) {

    SystemUtils_ = SystemUtils;

    ImageProcessor_ = std::make_unique<Lucifer::Lucifer>();

}
ExternalModelLoader::~ExternalModelLoader() {

}


// Image Helpers
FIBITMAP* FindTextureBitmap(std::string TexturePath, std::vector<std::pair<std::string, FIBITMAP*>>* LoadedTextures) {

    // Iterate Over Array, Try And Find Match
    for (unsigned int LoadedTextureIndex = 0; LoadedTextureIndex < LoadedTextures->size(); LoadedTextureIndex++) {
        if ((*LoadedTextures)[LoadedTextureIndex].first == TexturePath) {
            return (*LoadedTextures)[LoadedTextureIndex].second;
        }
    }

    // Return NULL On Failure
    return NULL;
}
void DeleteTextureBitmap(std::string TexturePath, std::vector<std::pair<std::string, FIBITMAP*>>* LoadedTextures) {

    int Index = -1;

    // Iterate Over Array, Try And Find Match
    for (unsigned int LoadedTextureIndex = 0; LoadedTextureIndex < LoadedTextures->size(); LoadedTextureIndex++) {
        if ((*LoadedTextures)[LoadedTextureIndex].first == TexturePath) {
            Index = LoadedTextureIndex;
            break;
        }
    }

    // Delete
    if (Index != -1) {
        LoadedTextures->erase(LoadedTextures->begin() + Index);
    }


}
std::pair<std::string, std::string> FindTextureMatches(ERS_STRUCT_Mesh* Mesh, std::string Type1, std::string Type2) {

    // Setup Initialization Variables
    bool HasType1 = false;
    std::string Type1Name;

    bool HasType2 = false;
    std::string Type2Name;

    // Traverse Textures Array, Try And find Types
    for (unsigned int i = 0; i < Mesh->Textures_.size(); i++) {
        if (Mesh->Textures_[i]->Type == Type1) {
            HasType1 = true;
            Type1Name = Mesh->Textures_[i]->Path;
            break;
        }
    }
    for (unsigned int i = 0; i < Mesh->Textures_.size(); i++) {
        if (Mesh->Textures_[i]->Type == Type2) {
            HasType2 = true;
            Type2Name = Mesh->Textures_[i]->Path;
            break;
        }
    }

    // Check That Both Are Sorted
    if (HasType1 && HasType2) {
        return std::make_pair(Type1Name, Type2Name);
    } else {
        return std::make_pair(std::string(""), std::string(""));
    }

}


// Model Loading Helpers
void ExternalModelLoader::DetectBoundingBox(ERS_STRUCT_Model* Model) {

    // Calculate Bounding Box
    glm::vec3 ModelMinXYZ = Model->Meshes[0].Vertices[0].Position;
    glm::vec3 ModelMaxXYZ = Model->Meshes[0].Vertices[0].Position;
    for (unsigned int MeshIndex = 0; MeshIndex < Model->Meshes.size(); MeshIndex++) {

        for (unsigned int VertIndex = 0; VertIndex < Model->Meshes[MeshIndex].Vertices.size(); VertIndex++) {

            // Get Mesh Min/Max
            glm::vec3 VertPos = Model->Meshes[MeshIndex].Vertices[VertIndex].Position;

            // Check If Larger/Smaller Than Model Min/Max
            if (VertPos.x < ModelMinXYZ.x) {
                ModelMinXYZ.x = VertPos.x;
            }
            if (VertPos.y < ModelMinXYZ.y) {
                ModelMinXYZ.y = VertPos.y;
            }
            if (VertPos.z < ModelMinXYZ.z) {
                ModelMinXYZ.z = VertPos.z;
            }
            if (VertPos.x > ModelMaxXYZ.x) {
                ModelMaxXYZ.x = VertPos.x;
            }
            if (VertPos.y > ModelMaxXYZ.y) {
                ModelMaxXYZ.y = VertPos.y;
            }
            if (VertPos.z > ModelMaxXYZ.z) {
                ModelMaxXYZ.z = VertPos.z;
            }

        }
    }
    Model->BoxScale_ = ModelMaxXYZ - ModelMinXYZ;
    Model->BoxOffset_ = (Model->BoxScale_ / 2.0f) + ModelMinXYZ;

    // Set Min Size For Bounding Box
    Model->BoxScale_.x = std::max(Model->BoxScale_.x, 0.025f);
    Model->BoxScale_.y = std::max(Model->BoxScale_.y, 0.025f);
    Model->BoxScale_.z = std::max(Model->BoxScale_.z, 0.025f);

    std::string LogMsg = std::string("Calculated Model Bounding Box To Be '") 
    + std::to_string(Model->BoxScale_.x) + "X, "
    + std::to_string(Model->BoxScale_.y) + "Y, "
    + std::to_string(Model->BoxScale_.z) + "Z' With Offset Of '"
    + std::to_string(Model->BoxOffset_.x) + "X, "
    + std::to_string(Model->BoxOffset_.y) + "Y, "
    + std::to_string(Model->BoxOffset_.z) + "Z'";
    SystemUtils_->Logger_->Log(LogMsg, 3);

}
void ExternalModelLoader::CalculateTotalVertsIndices(ERS_STRUCT_Model* Model) {

    // Get Vert/Indice Metadata Info
    Model->TotalVertices_ = 0;
    Model->TotalIndices_ = 0;

    for (unsigned long i = 0; i < Model->Meshes.size(); i++) {

        unsigned long VertSize = Model->Meshes[i].Vertices.size();
        unsigned long IndSize = Model->Meshes[i].Indices.size();
        Model->TotalVertices_ += VertSize;
        Model->TotalIndices_ += IndSize;

    }

}
void ExternalModelLoader::MergeTextures(ERS_STRUCT_Model* Model, std::vector<std::pair<std::string, FIBITMAP*>>* LoadedTextures) {

    // Create Pair Of All Textures With Opacity/Alpha Maps
    std::vector<std::pair<std::string, std::string>> OpacityAlphaMaps;
    for (unsigned int i = 0; i < Model->Meshes.size(); i++) {

        // Find Matching Types For The Same Mesh
        std::pair<std::string, std::string> Match = FindTextureMatches(&Model->Meshes[i], "texture_opacity", "texture_diffuse");

        // If Not Empty (Matching Failed) And It's Not Already In The Opacity Map, Add It
        if (Match != std::make_pair(std::string(""), std::string(""))) {

            bool InArray = false;
            for (unsigned int x = 0; x < OpacityAlphaMaps.size(); x++) {
                if (OpacityAlphaMaps[x] == Match) {
                    InArray = true;
                    break;
                } 
            }
            if (!InArray) {

                SystemUtils_->Logger_->Log(std::string("Found Opacity Map For Diffuse Texture '") + Match.second + "', Adding To Merge Queue", 2);
                OpacityAlphaMaps.push_back(Match);
            }

        }
    }

    // Iterate Over All Matches, Merge The Two
    for (unsigned int i = 0; i < OpacityAlphaMaps.size(); i++) {
        
        // Get Alpha, Diffuse From Real Loaded Texture Maps
        FIBITMAP* AlphaTexture = FindTextureBitmap(OpacityAlphaMaps[i].first, LoadedTextures);
        FIBITMAP* DiffuseTexture = FindTextureBitmap(OpacityAlphaMaps[i].second, LoadedTextures);

        // Merge Together (If Images Are Not NULL)
        if (AlphaTexture != NULL && DiffuseTexture != NULL) {
            FIBITMAP* AlphaChannel = FreeImage_GetChannel(AlphaTexture, FICC_ALPHA);
            FreeImage_SetChannel(DiffuseTexture, AlphaChannel, FICC_ALPHA);
            SystemUtils_->Logger_->Log(std::string("Merging Opacity Map For Texture '") + OpacityAlphaMaps[i].second + "'", 2);

        }

        // Delete ALpha Texture
        DeleteTextureBitmap(OpacityAlphaMaps[i].first, LoadedTextures);

    }

}
void ExternalModelLoader::ProcessModelTextures(ERS_STRUCT_ModelWriterData &Data) {

    // Create List Of Texture Files To Be Copied
    std::vector<std::pair<std::string, std::future<FIBITMAP*>>> ImageFutures;
    std::vector<std::pair<std::string, FIBITMAP*>> ImageBytes;
    for (int i = 0; (long)i < (long)Data.TextureList.size(); i++) {

        ERS_STRUCT_IOData IOData;
        std::string TexturePath = Data.TextureList[i];
        bool Success = ReadFile(Data.TextureList[i], &IOData);
        IOData.AssetTypeName = "Texture";
        IOData.AssetFileName = Data.TextureList[i].substr(Data.ModelOriginDirectoryPath.find_last_of("/") + 1, Data.ModelOriginDirectoryPath.size() - 1);
        IOData.AssetCreationDate = SystemUtils_->ERS_IOSubsystem_->GetCurrentTime();


        bool SecondTryStatus = false;
        if (!Success) {
            SystemUtils_->Logger_->Log(std::string("Error Loading Texture From Given Path '") + Data.ModelOriginDirectoryPath + std::string("', Will Search Current Directory For Texture"), 7);

            // Strip To Last Item In Path (With Forward Slashes And Backward Slashes)
            std::string Path = Data.TextureList[i];
            std::replace(Path.begin(), Path.end(), '\\', '/');
            if (Path.find("/") != std::string::npos) {
                Path = Path.substr(Path.find_last_of("/") + 1, Path.size()-1);
            }

            // Create Reference String To Be Tested Against
            std::string RefString = Path.substr(0, Path.find_first_of("."));
            size_t Pos = 0;
            while ((Pos = RefString.find(" ", Pos)) != std::string::npos) {
                RefString.replace(Pos, 1, "_");
                Pos ++;
            }

            // Check Against Filesystem
            std::replace(Data.ModelOriginDirectoryPath.begin(), Data.ModelOriginDirectoryPath.end(), '\\', '/');
            for (const auto &Entry : std::filesystem::recursive_directory_iterator(Data.ModelOriginDirectoryPath.substr(0, Data.ModelOriginDirectoryPath.find_last_of("/")))) {

                std::string FilePath{Entry.path().u8string()};
                std::replace(FilePath.begin(), FilePath.end(), '\\', '/');
                std::string FileName = FilePath.substr(FilePath.find_last_of('/') + 1, FilePath.size() - 1);
                std::string FileNameWithoutExtension = FileName.substr(0, FileName.find_first_of("."));

                // Remove Spaces From Filename And Replace With Underscores
                Pos = 0;
                while ((Pos = FileNameWithoutExtension.find(" ", Pos)) != std::string::npos) {
                    FileNameWithoutExtension.replace(Pos, 1, "_");
                    Pos ++;
                }


                if (FileNameWithoutExtension == RefString) {
                    Path = FilePath;
                    SystemUtils_->Logger_->Log(std::string("Found Potential Match '") + FilePath + std::string("', Attempting To Load"), 5);
                    break;
                }

            
            }


            SecondTryStatus = ReadFile(Path, &IOData);
            TexturePath = Path;
            
            if (!SecondTryStatus) {
                SystemUtils_->Logger_->Log("Failed To Find Texture During Second Try Effort, Abandoning Texture", 8);
            } else {
                SystemUtils_->Logger_->Log("Found Probable File, However This Is Not Guarenteed To Be Correct", 6);
            }
        }

        if (Success || SecondTryStatus) {
            SystemUtils_->Logger_->Log(std::string("Starting Loading Texture '")  + TexturePath + "'", 3);

            Lucifer::Image LuciferImage;
            ImageProcessor_->Load(IOData.Data.get(), IOData.Size_B, LuciferImage);
            FIBITMAP* Image = Lucifer::Lucifer_CreateFIBitmapFromImage(LuciferImage);
            

            int Width, Height;
            Width = FreeImage_GetWidth(Image);
            Height = FreeImage_GetHeight(Image);
            if ((Width < 1) || (Height < 1)) {
                SystemUtils_->Logger_->Log("Error Loading Image, Resulting Raw Image Has Invalid Size!", 7);
            }

            std::string Size = "(" + std::to_string(FreeImage_GetWidth(Image)) + "x" + std::to_string(FreeImage_GetWidth(Image)) + ")";
            SystemUtils_->Logger_->Log(std::string("Loaded Image Texture Of Size ") + Size, 2);

            ImageBytes.push_back(std::make_pair(TexturePath, Image));
        }
    }

    // Remove Duplicate Stuff (Like Alpha Maps), Just Generally Consolidate Stuff
    MergeTextures(Data.Model, &ImageBytes);

    Data.ImageBytes = ImageBytes;

}
void ExternalModelLoader::ProcessNode(ERS_STRUCT_ModelWriterData &Data, ERS_STRUCT_Model* Model, aiNode *Node, const aiScene *Scene, std::string ModelDirectory) {

    // Process Meshes In Current Node
    for (unsigned int i = 0; i < Node->mNumMeshes; i++) {
        aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[i]];
        Model->Meshes.push_back(ProcessMesh(Data, Model, Mesh, Scene, ModelDirectory));
    }

    // Process Children Nodes
    for (unsigned int i = 0; i < Node->mNumChildren; i++) {
        ProcessNode(Data, Model, Node->mChildren[i], Scene, ModelDirectory);
    }


}




void IdentifyMeshTextures(aiMaterial* Mat, ERS_STRUCT_Mesh* Mesh) {

    std::vector<std::pair<aiTextureType, std::string>> TextureTypes;
    TextureTypes.push_back(std::make_pair(aiTextureType_AMBIENT, "texture_ambient"));
    TextureTypes.push_back(std::make_pair(aiTextureType_AMBIENT_OCCLUSION, "texture_ambient_occlusion"));
    TextureTypes.push_back(std::make_pair(aiTextureType_BASE_COLOR, "texture_base_color"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DIFFUSE, "texture_diffuse"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DIFFUSE_ROUGHNESS, "texture_diffuse_roughness"));
    TextureTypes.push_back(std::make_pair(aiTextureType_DISPLACEMENT, "texture_displacement"));
    TextureTypes.push_back(std::make_pair(aiTextureType_EMISSION_COLOR, "texture_emission_color"));
    TextureTypes.push_back(std::make_pair(aiTextureType_EMISSIVE, "texture_emissive"));
    TextureTypes.push_back(std::make_pair(aiTextureType_HEIGHT, "texture_height"));
    TextureTypes.push_back(std::make_pair(aiTextureType_LIGHTMAP, "texture_lightmap"));
    TextureTypes.push_back(std::make_pair(aiTextureType_METALNESS, "texture_metalness"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NONE, "texture_none"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NORMAL_CAMERA, "texture_normal_camera"));
    TextureTypes.push_back(std::make_pair(aiTextureType_NORMALS, "texture_normals"));
    TextureTypes.push_back(std::make_pair(aiTextureType_OPACITY, "texture_opacity"));
    TextureTypes.push_back(std::make_pair(aiTextureType_REFLECTION, "texture_reflection"));
    TextureTypes.push_back(std::make_pair(aiTextureType_SHININESS, "texture_shininess"));
    TextureTypes.push_back(std::make_pair(aiTextureType_SPECULAR, "texture_specular"));
    TextureTypes.push_back(std::make_pair(aiTextureType_UNKNOWN, "texture_unknown"));

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
                    //SystemUtils_->Logger_->Log(std::string("Found Matching Texture '") + Mesh->Loader_RequestedTextureInformation_[x].second + "'", 3);
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




ERS_STRUCT_Mesh ExternalModelLoader::ProcessMesh(ERS_STRUCT_ModelWriterData &Data, ERS_STRUCT_Model* Model, aiMesh *Mesh, const aiScene *Scene, std::string ModelDirectory) {

    // Create Data Holders
    ERS_STRUCT_Mesh OutputMesh;

    // Process Materials
    aiMaterial* Material = Scene->mMaterials[Mesh->mMaterialIndex];
    HandleMeshTextures(Data, Model, Material, ModelDirectory, &OutputMesh);
    // IdentifyMeshTextures(Material, &OutputMesh);

    // Iterate Through Meshes' Vertices
    for (unsigned int i = 0; i < Mesh->mNumVertices; i++) {

        // Hold Vertex Data
        ERS_STRUCT_Vertex Vertex;
        glm::vec3 Vector;


        Vector.x = Mesh->mVertices[i].x;
        Vector.y = Mesh->mVertices[i].y;
        Vector.z = Mesh->mVertices[i].z;
        Vertex.Position = Vector;

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

    return OutputMesh;

}






void ExternalModelLoader::HandleMeshTextures(ERS_STRUCT_ModelWriterData &Data, ERS_STRUCT_Model* Model, aiMaterial* Material, std::string ModelDirectory, ERS_STRUCT_Mesh* TargetMesh) {

    SystemUtils_->Logger_->Log("Identifying Mesh Textures", 3);
    AddTexture(Data, Model, Material, aiTextureType_AMBIENT,           "texture_ambient",           ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_AMBIENT_OCCLUSION, "texture_ambient_occlusion", ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_BASE_COLOR,        "texture_base_color",        ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_DIFFUSE,           "texture_diffuse",           ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_diffuse_roughness", ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_DISPLACEMENT,      "texture_displacement",      ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_EMISSION_COLOR,    "texture_emission_color",    ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_EMISSIVE,          "texture_emissive",          ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_HEIGHT,            "texture_height",            ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_LIGHTMAP,          "texture_lightmap",          ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_METALNESS,         "texture_metalness",         ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_NORMAL_CAMERA,     "texture_normal_camera",     ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_NORMALS,           "texture_normals",           ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_OPACITY,           "texture_opacity",           ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_REFLECTION,        "texture_reflection",        ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_SHININESS,         "texture_shininess",         ModelDirectory, TargetMesh);
    AddTexture(Data, Model, Material, aiTextureType_SPECULAR,          "texture_specular",          ModelDirectory, TargetMesh);
    SystemUtils_->Logger_->Log("Finshed Mesh Texture Identification", 4);

}
void ExternalModelLoader::AddTexture(ERS_STRUCT_ModelWriterData &Data, ERS_STRUCT_Model* Model, aiMaterial *Mat, aiTextureType Type, std::string TypeName, std::string ModelDirectory, ERS_STRUCT_Mesh* TargetMesh) {


    for (unsigned int i=0; i< Mat->GetTextureCount(Type); i++) {

        aiString Str;
        Mat->GetTexture(Type, i, &Str);

        // Calculate Texture Path, Append If Not Already In List
        std::string FilePath = std::string(ModelDirectory + std::string(Model->Directory)  + std::string("/") + std::string(Str.C_Str()));
        std::string Message = std::string("Found Texture Of Type '") + TypeName + std::string("' At Path '") + FilePath + std::string("'");
        SystemUtils_->Logger_->Log(Message, 3);
        if (std::find(Data.TextureList.begin(), Data.TextureList.end(), FilePath) == Data.TextureList.end()) {
            Data.TextureList.push_back(FilePath);
            Data.TextureNames.push_back(Str.C_Str());
        }   
        TargetMesh->Loader_RequestedTextureInformation_.push_back(std::make_pair(TypeName, Str.C_Str()));

    }

}
bool ExternalModelLoader::ReadFile(std::string FilePath, ERS_STRUCT_IOData* OutputData) {

    struct stat Buffer;
    int FileStatus = stat(FilePath.c_str(), &Buffer);


        if (FileStatus == 0) {

            OutputData->Data.reset(new unsigned char[Buffer.st_size]);
            if (OutputData->Data) {

                FILE *Stream = fopen(FilePath.c_str(), "rb");
                if (Stream) {

                    [[maybe_unused]]size_t _ = fread(OutputData->Data.get(), sizeof(unsigned char), Buffer.st_size, Stream);
                    OutputData->Size_B = Buffer.st_size;
                    fclose(Stream);
                    OutputData->HasLoaded = true;

                } else {
                    SystemUtils_->Logger_->Log(std::string(std::string("Error Loading Asset '") + FilePath + std::string("', Failed To Open Filestream")).c_str(), 9);
                    OutputData->HasLoaded = false;
                    return false;
                }

            } else {
                SystemUtils_->Logger_->Log(std::string(std::string("Error Loading Asset '") + FilePath + std::string("', Memory Allocation Failed")).c_str(), 9);            
                OutputData->HasLoaded = false;
                return false;
            }
        
        } else {
            SystemUtils_->Logger_->Log(std::string(std::string("Error Loading Asset '") + FilePath + std::string("', File Not Found")).c_str(), 9);
            OutputData->HasLoaded = false;
            return false;
        }

    return true;

}

bool ExternalModelLoader::PerformModelSanityChecks(ERS_STRUCT_Model &Model) {

    // Check For Meshes
    if (Model.Meshes.size() == 0) {
        SystemUtils_->Logger_->Log(std::string("Model Has No Meshes, Aborting"), 8);
        return false;
    }

    // Check Meshes For Verts
    for (unsigned int i = 0; i < Model.Meshes.size(); i++) {
        if (Model.Meshes[i].Vertices.size() == 0) {
            SystemUtils_->Logger_->Log(std::string("Warning, Mesh '") + std::to_string(i) + "' Has No Verts", 7);
            return false;
        }
        if (Model.Meshes[i].Indices.size() == 0) {
            SystemUtils_->Logger_->Log(std::string("Warning, Mesh '") + std::to_string(i) + "' Has No Indices", 7);
            return false;
        }
    }

    // Passed Check
    return true;
}

// Load Model From File
bool ExternalModelLoader::LoadModel(std::string ModelPath, ERS_STRUCT_ModelWriterData &Data) {

    SystemUtils_->Logger_->Log(std::string("Loading External Model '") + ModelPath + "'", 5);

    // Calculate Paths
    std::string ModelDirectory = ModelPath.substr(0, std::string(ModelPath).find_last_of("/"));
    std::string ModelFileName = ModelPath.substr(ModelPath.find_last_of("/") + 1, ModelPath.size() - 1);

    // Load Via Assimp
    try {
        const aiScene* Scene = Data.ModelImporter.ReadFile(ModelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices | aiProcess_JoinIdenticalVertices);

        if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
            SystemUtils_->Logger_->Log(std::string(std::string("External Model Loading Error: ") + std::string(Data.ModelImporter.GetErrorString())).c_str(), 10);
            Data.ModelScene = nullptr;
            return false;
        }
        SystemUtils_->Logger_->Log("Finished Loading External Model, Processing Geometry/Textures", 3);

        // Process Geometry, Identify Textures
        ProcessNode(Data, Data.Model, Scene->mRootNode, Scene, ModelDirectory);
        if (!PerformModelSanityChecks(*Data.Model)) {
            return false;
        }
        DetectBoundingBox(Data.Model);
        CalculateTotalVertsIndices(Data.Model);

        // Update Struct
        Data.ModelOriginDirectoryPath = ModelPath;
        Data.ModelScene               = (aiScene*)Scene;
        Data.ModelFileName            = ModelFileName;

        // Load Textures
        ProcessModelTextures(Data);

        return true;

    } catch (const DeadlyImportError& e) {
        SystemUtils_->Logger_->Log("Assimp Threw DeadlyImportError, Aborting Load", 8);
        return false;
    }

}


}
}
}

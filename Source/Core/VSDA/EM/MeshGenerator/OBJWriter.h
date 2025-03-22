#pragma once
#include <VSDA/EM/MeshGenerator/MarchingCubes.h>
#include <BG/Common/Logger/Logger.h>
#include <filesystem>
#include <fstream>


namespace BG {
namespace NES {
namespace Simulator {

class ChunkedOBJWriter {
public:
    static void WriteChunk(
        const std::string& outputDir,
        int chunkIndex,
        const std::unordered_map<uint64_t, Mesh>& neuronMeshes,
        BG::Common::Logger::LoggingSystem* logger,
        bool debugMode = false
    ) {
        namespace fs = std::filesystem;
        
        try {
            // Create directory if needed
            fs::create_directories(outputDir);
            
            // Write each neuron mesh in the chunk
            for (const auto& [uid, mesh] : neuronMeshes) {
                // Create filename: chunkIndex_uid.obj
                fs::path filePath = fs::path(outputDir) / 
                    ("chunk_" + std::to_string(chunkIndex) + "_uid_" + std::to_string(uid) + ".obj");
                
                // Open file
                std::ofstream file(filePath);
                if (!file.is_open()) {
                    logger->Log("Failed to create OBJ file for chunk " + std::to_string(chunkIndex) + 
                                ", UID: " + std::to_string(uid), 3);
                    continue;
                }

                if (debugMode) {
                    WriteDebugCube(file, chunkIndex, uid, logger);
                } else {
                    WriteNeuronMesh(file, chunkIndex, uid, mesh, logger);
                }

                file.close();  // Explicitly close the file
                logger->Log("Wrote chunk " + std::to_string(chunkIndex) + 
                            ", UID: " + std::to_string(uid) + " to " + filePath.string(), 4);
            }

        } catch (const std::exception& e) {
            logger->Log("Error writing chunk " + std::to_string(chunkIndex) + 
                        ": " + e.what(), 3);
        }
    }

private:
    static void WriteDebugCube(std::ofstream& file, int chunkIndex, uint64_t uid, 
                                BG::Common::Logger::LoggingSystem* logger) {
        // Simple unit cube vertices (centered at origin)
        const std::vector<Geometries::Vec3D> cubeVertices = {
            {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
            {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5},
            {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5},
            {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}
        };

        // Cube faces (OBJ uses 1-based indices)
        const std::vector<uint32_t> cubeFaces = {
            0,1,2, 0,2,3,  // Bottom
            4,5,6, 4,6,7,  // Top
            0,4,7, 0,7,3,  // Left
            1,5,6, 1,6,2,  // Right
            0,1,5, 0,5,4,  // Front
            3,2,6, 3,6,7   // Back
        };

        // Write header
        file << "# Debug Cube for Chunk " << chunkIndex << ", UID: " << uid << "\n";
        file << "# Vertices: 8\n";
        file << "# Faces: 12\n\n";

        // Write vertices
        for (const auto& v : cubeVertices) {
            file << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }

        // Write faces
        for (size_t i = 0; i < cubeFaces.size(); i += 3) {
            file << "f " << cubeFaces[i]+1 << " "
                    << cubeFaces[i+1]+1 << " "
                    << cubeFaces[i+2]+1 << "\n";
        }

        logger->Log("DEBUG: Wrote cube for chunk " + std::to_string(chunkIndex) + 
                    ", UID: " + std::to_string(uid), 4);
    }

    static void WriteNeuronMesh(std::ofstream& file, int chunkIndex, uint64_t uid, 
                                const Mesh& mesh, BG::Common::Logger::LoggingSystem* logger) {
        // Write header
        file << "# BrainGenix Neuron Mesh for Chunk " << chunkIndex << ", UID: " << uid << "\n";
        file << "# Vertices: " << mesh.vertices.size() << "\n";
        file << "# Faces: " << mesh.indices.size()/3 << "\n\n";
        
        // Write vertices
        for (const auto& v : mesh.vertices) {
            file << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }
        
        // Write faces
        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            file << "f " << mesh.indices[i]+1 << " "
                    << mesh.indices[i+1]+1 << " "
                    << mesh.indices[i+2]+1 << "\n";
        }
        
        logger->Log("Wrote mesh for chunk " + std::to_string(chunkIndex) + 
                    ", UID: " + std::to_string(uid) + 
                    " (" + std::to_string(mesh.vertices.size()) + " vertices, " +
                    std::to_string(mesh.indices.size()/3) + " faces)", 4);
    }
};

} // namespace Simulator
} // namespace NES
} // namespace BG
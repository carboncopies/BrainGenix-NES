# A simple breakdown of the Neuroglancer Legacy Mesh definition

The following is based on:
https://github.com/google/neuroglancer/blob/master/src/datasource/precomputed/meshes.md

Prerequisite:

Every segmented object - has non-zero integer label (as per segmentation files).

Each subvolume receives a "fragment" of the object mesh.

If what they mean is the info.json file at the top level and a `/mesh` subdirectory at the top level
then it may be that we get to define our own subvolumes in any way we like.

Add to the info.json file of a segmentation volume (subvolume):

- add property: `"mesh"`

Legacy single-resolution mesh format:

- add subdirectory: `/mesh`

- add sub-directory info file: `/mesh/info.json`

contains:

```
{
	"@type": "neuroglancer_legacy_mesh"
}
```

Within `/mesh`, for each object with mesh surface definitions,
a JSON file:

`<segment-id>:0` where segment-id == the integer label as a string

E.g. file `3764:0` (for object with segmentation label 3764) contains:

```
{
	"fragments": ["3764:1", "3764:2", "3764:3"]
}
```

The files containing the mesh data for each fragment are then:

```
/mesh/3764:1
/mesh/3764:2
/mesh/3764:3
```

Each fragment file is in binary format, as follows:

```
<uint32> num_vertices
<float 32-bit> vertex0-x
<float 32-bit> vertex0-y
<float 32-bit> vertex0-z
<float 32-bit> vertex1-x
<float 32-bit> vertex1-y
<float 32-bit> vertex1-z
<float 32-bit> vertex2-x
<float 32-bit> vertex2-y
<float 32-bit> vertex2-z
... (num_vertices of these)
<uint32> triangle0 vertexA
<uint32> triangle0 vertexB
<uint32> triangle0 vertexC
<uint32> triangle1 vertexA
<uint32> triangle1 vertexB
<uint32> triangle1 vertexC
<uint32> triangle2 vertexA
<uint32> triangle2 vertexB
<uint32> triangle2 vertexC
... (etc)
```

## Example code:

```
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>

// Function to save mesh data in Neuroglancer Legacy Mesh format
void saveNeuroglancerMesh(const std::string& baseDir, uint32_t segmentId, const std::vector<float>& vertices, const std::vector<uint32_t>& triangles) {
    // Create the mesh directory if it doesn't exist
    std::string meshDir = baseDir + "/mesh";
    std::filesystem::create_directories(meshDir);

    // Create the info.json file for the mesh directory
    std::ofstream infoFile(meshDir + "/info.json");
    infoFile << "{\n  \"@type\": \"neuroglancer_legacy_mesh\"\n}";
    infoFile.close();

    // Create the JSON file for the segment
    std::string segmentJsonFile = meshDir + "/" + std::to_string(segmentId) + ":0";
    std::ofstream jsonFile(segmentJsonFile);
    jsonFile << "{\n  \"fragments\": [\"" << segmentId << ":1\"]\n}";
    jsonFile.close();

    // Create the binary fragment file
    std::string fragmentFile = meshDir + "/" + std::to_string(segmentId) + ":1";
    std::ofstream binFile(fragmentFile, std::ios::binary);

    // Write the number of vertices
    uint32_t numVertices = vertices.size() / 3;
    binFile.write(reinterpret_cast<const char*>(&numVertices), sizeof(numVertices));

    // Write the vertices
    for (size_t i = 0; i < vertices.size(); ++i) {
        float vertex = vertices[i];
        binFile.write(reinterpret_cast<const char*>(&vertex), sizeof(vertex));
    }

    // Write the number of triangles
    uint32_t numTriangles = triangles.size() / 3;
    binFile.write(reinterpret_cast<const char*>(&numTriangles), sizeof(numTriangles));

    // Write the triangles
    for (size_t i = 0; i < triangles.size(); ++i) {
        uint32_t triangleVertex = triangles[i];
        binFile.write(reinterpret_cast<const char*>(&triangleVertex), sizeof(triangleVertex));
    }

    binFile.close();
}

int main() {
    // Example usage
    std::vector<float> vertices = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    std::vector<uint32_t> triangles = {
        0, 1, 2
    };

    saveNeuroglancerMesh("path/to/segmentation", 3764, vertices, triangles);

    return 0;
}
```

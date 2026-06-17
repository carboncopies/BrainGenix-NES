//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides thread safe STL container templates.
    Additional Notes: None
    Date Created: 2026-06-06
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <shared_mutex>

namespace BG {
namespace NES {

template <typename T>
class ConcurrentUniquePtrRegistry {
private:
    std::vector<std::unique_ptr<T>> vec;
    mutable std::shared_mutex smtx;

public:
    // 1. Read: Returns a non-const raw pointer so contents can be modified.
    // Notice the function itself is still marked 'const' because it doesn't 
    // modify the vector structure, but it returns a mutable pointer 'T*'.
    T* read(size_t index) const {
        std::shared_lock<std::shared_mutex> lock(smtx);
        if (index >= vec.size()) {
            return nullptr; 
        }
        return vec[index].get(); // Returns T* (can be nullptr)
    }

    // 2. Extend: Appends a new item and returns its assigned ID (index)
    size_t append(std::unique_ptr<T> item) {
        std::unique_lock<std::shared_mutex> lock(smtx);
        vec.push_back(std::move(item));
        return vec.size() - 1; // The index acts as a permanent ID
    }

    // 3. Mark Deleted: Sets the pointer to null to free the object's memory.
    // Does not shrink the vector, preserving all other element indices.
    void remove(size_t index) {
        std::unique_lock<std::shared_mutex> lock(smtx);
        if (index < vec.size()) {
            vec[index].reset(); // Safely deletes the object and sets slot to nullptr
        }
    }

    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(smtx);
        return vec.size();
    }
};

}; // Close Namespace NES
}; // Close Namespace BG

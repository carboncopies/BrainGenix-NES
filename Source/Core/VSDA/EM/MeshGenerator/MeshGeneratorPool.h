#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include <VSDA/EM/MeshGenerator/MeshTask.h>

namespace BG {
namespace NES {
namespace Simulator {

class MeshGeneratorPool {
public:
    MeshGeneratorPool(size_t numThreads);
    ~MeshGeneratorPool();
    
    std::future<std::unordered_map<uint64_t, Mesh>> SubmitTask(const MeshTask& task);
    void WaitAll();

private:
    void WorkerThread();

    std::vector<std::thread> workers;
    std::queue<std::pair<MeshTask, std::promise<std::unordered_map<uint64_t, Mesh>>>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

} // namespace Simulator
} // namespace NES
} // namespace BG
#include <VSDA/EM/MeshGenerator/MeshGeneratorPool.h>

namespace BG {
namespace NES {
namespace Simulator {

MeshGeneratorPool::MeshGeneratorPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] { WorkerThread(); });
    }
}

MeshGeneratorPool::~MeshGeneratorPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers) {
        worker.join();
    }
}

std::future<std::unordered_map<uint64_t, Mesh>> MeshGeneratorPool::SubmitTask(const MeshTask& task) {
    std::promise<std::unordered_map<uint64_t, Mesh>> promise;
    auto future = promise.get_future();
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        taskQueue.emplace(task, std::move(promise));
    }
    condition.notify_one();
    return future;
}

void MeshGeneratorPool::WorkerThread() {
    while (true) {
        std::pair<MeshTask, std::promise<std::unordered_map<uint64_t, Mesh>>> taskPair;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !taskQueue.empty(); });
            if (stop && taskQueue.empty()) return;
            taskPair = std::move(taskQueue.front());
            taskQueue.pop();
        }
        try {
            auto result = taskPair.first.Execute();
            taskPair.second.set_value(result);
        } catch (...) {
            taskPair.second.set_exception(std::current_exception());
        }
    }
}

void MeshGeneratorPool::WaitAll() {
    std::unique_lock<std::mutex> lock(queueMutex);
    condition.wait(lock, [this] { return taskQueue.empty(); });
}

} // namespace Simulator
} // namespace NES
} // namespace BG
#include <BG/Renderer/Shader/ShaderCompiler.h>



namespace BG {
namespace NES {
namespace Renderer {
namespace Internal {


ShaderCompiler::ShaderCompiler(BG::Common::Logger::LoggingSystem* _Logger) {
    assert(_Logger != nullptr);
    Logger_ = _Logger;

    Logger_->Log("Initializing Shader Compiler Subsystem", 2);
    ResetWorkQueue();

}

ShaderCompiler::~ShaderCompiler() {

}



int ShaderCompiler::AppendShaderToWorkQueue(std::string _GLSLSourceCode, std::string _SourceName, shaderc_shader_kind _ShaderType, bool _Optimize) {
    
    // Build Work Item
    std::shared_ptr<ShaderCompileObject> WorkItem = std::make_shared<ShaderCompileObject>();
    WorkItem->GLSLSource_     = _GLSLSourceCode;
    WorkItem->SourceName_     = _SourceName;
    WorkItem->ShaderType_     = _ShaderType;
    WorkItem->Optimize_       = _Optimize;
    WorkItem->ReadyToCompile_ = true;
    
    // Append To Work Queue
    std::lock_guard<std::mutex> WorkQueueLock(AllowWorkQueueAccess_);
    WorkQueue_.push_back(WorkItem);
    return WorkQueue_.size() - 1;

}

void ShaderCompiler::ProcessWorkQueue(int _NumThreads) {

    // Firstly, Build Threadpool
    std::lock_guard<std::mutex> WorkQueueLock(AllowWorkQueueAccess_);
    KeepThreadsAlive_ = true;
    NextItemToWorkOn_ = 0;
    for (unsigned int i = 0; i < _NumThreads; i++) {
        WorkerThreads_.push_back(std::thread(&ShaderCompiler::WorkerFunction, this));
    }

    // Next, Process Items
    while (NextItemToWorkOn_ < WorkQueue_.size()) {
        // add a sleep function for like 5 ms
    }

    // Finally, Terminate Threads
    KeepThreadsAlive_ = false;
    for (unsigned int i = 0; i < WorkerThreads_.size(); i++) {
        WorkerThreads_[i].join();
    }

}

bool ShaderCompiler::GetShaderBytecode(int _ShaderID, std::vector<uint32_t>* _DestintationVector) {
    assert(_DestintationVector != nullptr && "Cannot Get Shader Bytecode, Destination Is Null");

    // Early Out If Not Yet Compiled
    if (NextItemToWorkOn_ < _ShaderID) {
        return false;
    }

    // Get Work Data, Early Out If Compiler Still Not Yet Done
    std::shared_ptr<ShaderCompileObject> WorkObject = WorkQueue_[_ShaderID];
    if (!WorkObject->HasCompiled_) {
        return false;
    }

    // Check Status, Log Error If There's One, Exit
    if (WorkObject->Result_.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::string Msg = "Failed To Compile Shader '" + WorkObject->SourceName_ + "' With Error '";
        Msg += WorkObject->Result_.GetErrorMessage();
        Msg += "'";
        Logger_->Log(Msg, 7);
        return false;
    }

    // Finally, Write To Destination Vector, Return True
    (*_DestintationVector) = {WorkObject->Result_.cbegin(), WorkObject->Result_.cend()};
    return true;

}

void ShaderCompiler::ResetWorkQueue() {
    WorkQueue_.clear();
    NextItemToWorkOn_ = 0;
    KeepThreadsAlive_ = false;
}

void ShaderCompiler::WorkerFunction() {

    // Firstly, Setup Shader Compiler
    int MaxSize = WorkQueue_.size();
    shaderc::Compiler Compiler;
    
    while (KeepThreadsAlive_ && NextItemToWorkOn_ < MaxSize) {

        // Get Work Item
        WorkQueueReadAccess_.lock();
        int ItemIndex = NextItemToWorkOn_++;
        if (ItemIndex >= MaxSize) {
            continue;
        }
        std::shared_ptr<ShaderCompileObject> WorkItem = WorkQueue_[ItemIndex];
        WorkQueueReadAccess_.unlock();

        // Setup Options
        shaderc::CompileOptions Options;
        Options.SetTargetEnvironment( shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0 );
        Options.SetSourceLanguage( shaderc_source_language_glsl );

        std::vector<std::pair<std::string, std::string>> Definitions;
        Definitions.push_back(std::make_pair("__VK_GLSL__", "1"));

        for (std::pair<std::string, std::string> &DefinitionPairs : Definitions) {
            Options.AddMacroDefinition(DefinitionPairs.first, DefinitionPairs.second);
        }




        // Preprocess
        shaderc::PreprocessedSourceCompilationResult PreprocessedResult = Compiler.PreprocessGlsl(WorkItem->GLSLSource_, WorkItem->ShaderType_, WorkItem->SourceName_.c_str(), Options);
        if (PreprocessedResult.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::string Msg = "Failed To Preprocess Shader '" + WorkItem->SourceName_ + "' With Error '";
            Msg += PreprocessedResult.GetErrorMessage();
            Msg += "'";
            Logger_->Log(Msg, 7);
            continue;
        }
        std::string PreprocessedSource = {PreprocessedResult.cbegin(), PreprocessedResult.cend()};

        // Compile
        WorkItem->Result_ = Compiler.CompileGlslToSpv(PreprocessedSource, WorkItem->ShaderType_, WorkItem->SourceName_.c_str(), Options);
        WorkItem->HasCompiled_ = true;

    }

    // Now the compiler goes out of scope and is deallocated

}




}; // Close Namespace Internal
}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
#include <BG/Renderer/VulkanShaders.h>



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
    std::unique_lock WorkQueueLock = std::lock(AllowWorkQueueAccess_);
    WorkQueue.push_back(WorkItem);
    return WorkQueue.size() - 1;

}

void ShaderCompiler::ProcessWorkQueue(int _NumThreads) {

    // Firstly, Build Threadpool
    std::unique_lock LockQueue_ = std::lock(AllowWorkQueueAccess_);
    KeepWorkerThreadsAlive_ = true;
    for (unsigned int i = 0; i < _NumThreads) {
        WorkerThreads.push_back(std::thread(&ShaderCompiler::WorkerFunction));
    }

    // Next, Process Items
    while (NextItemToWorkOn_ < WorkQueue.size()) {
        // add a sleep function for like 5 ms
    }

    // Finally, Terminate Threads
    KeepWorkerThreadsAlive_ = false;
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
    if (WorkObject->!HasCompiled_) {
        return false;
    }

    // Check Status, Log Error If There's One, Exit
    if (WorkObject->Result->GetCompilationStatus() != shaderc_compilation_status_success) {
        std::string Msg = "Failed To Compile Shader '" + _SourceName + "' With Error '";
        Msg += WorkObject->Result->GetErrorMessage();
        Msg += "'";
        _Logger->Log(Msg, 7);
        return false;
    }

    // Finally, Write To Destination Vector, Return True
    (*_DestintationVector) = {WorkObject->Result->cbegin(), WorkObject->Result->cend()};
    return true;

}

void ShaderCompiler::ResetWorkQueue() {
    WorkQueue.reset();
    NextItemToWorkOn_ = 0;
    KeepThreadsAlive_ = false;
}

void ShaderCompiler::WorkerFunction() {

}




bool Shaderc_PreprocessShaderGLSL(BG::Common::Logger::LoggingSystem* _Logger, std::string _Source, std::string _SourceName, shaderc_shader_kind _ShaderType, std::string* _PreprocessedResult, bool _Verbose) {
    assert(_Logger != nullptr);
    assert(_PreprocessedResult != nullptr);

    // Setup Shaderc Compiler
    shaderc::Compiler Compiler;
    shaderc::CompileOptions Options;
	Options.SetTargetEnvironment( shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0 );
	Options.SetSourceLanguage( shaderc_source_language_glsl );

	std::vector<std::pair<std::string, std::string>> Definitions;
	Definitions.push_back(std::make_pair("__VK_GLSL__", "1"));

	for (std::pair<std::string, std::string> &DefinitionPairs : Definitions) {
		Options.AddMacroDefinition(DefinitionPairs.first, DefinitionPairs.second);
    }

    // If we want to add our own macros, do so like this (can be useful for say numlights or something like that):
    // options.AddMacroDefinition("MY_DEFINE", "1");

    // Preprocess, then check result
    if (_Verbose) {
        std::string Msg = "Preprocessing Shader '" + _SourceName + "'";
        _Logger->Log(Msg, 3);
    }
    shaderc::PreprocessedSourceCompilationResult PreprocessedResult = Compiler.PreprocessGlsl(_Source, _ShaderType, _SourceName.c_str(), Options);

    if (PreprocessedResult.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::string Msg = "Failed To Preprocess Shader '" + _SourceName + "' With Error '";
        Msg += PreprocessedResult.GetErrorMessage();
        Msg += "'";
        _Logger->Log(Msg, 7);
        return false;
    }
    (*_PreprocessedResult) = {PreprocessedResult.cbegin(), PreprocessedResult.cend()};
    return true;
}

bool Shaderc_CompileToAssembly(BG::Common::Logger::LoggingSystem* _Logger, std::string _Source, std::string _SourceName, shaderc_shader_kind _ShaderType, std::string* _CompiledResult, bool _Optimize, bool _Verbose) {
    assert(_Logger != nullptr);
    assert(_CompiledResult != nullptr);    

    // Setup Shaderc Compiler
    shaderc::Compiler Compiler;
    shaderc::CompileOptions Options;
	Options.SetTargetEnvironment( shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0 );
	Options.SetSourceLanguage( shaderc_source_language_glsl );

	std::vector<std::pair<std::string, std::string>> Definitions;
	Definitions.push_back(std::make_pair("__VK_GLSL__", "1"));

	for (std::pair<std::string, std::string> &DefinitionPairs : Definitions) {
		Options.AddMacroDefinition(DefinitionPairs.first, DefinitionPairs.second);
    }


    // If we want to add our own macros, do so like this (can be useful for say numlights or something like that):
    // options.AddMacroDefinition("MY_DEFINE", "1");

    // Set Optimization Enable/Disable
    if (_Optimize) {
        Options.SetOptimizationLevel(shaderc_optimization_level_size);
    }

    // Compile Now
    if (_Verbose) {
        std::string Msg = "Compiling Shader '" + _SourceName + "' To Assembly With Optimizations ";
        if (_Optimize) {
            Msg += "'Enabled'";
        } else {
            Msg += "'Disabled'";
        }
        _Logger->Log(Msg, 3);
    }
    shaderc::AssemblyCompilationResult AssmResult = Compiler.CompileGlslToSpvAssembly(_Source, _ShaderType, _SourceName.c_str(), Options);

    if (AssmResult.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::string Msg = "Failed To Compile Shader '" + _SourceName + "' With Error '";
        Msg += AssmResult.GetErrorMessage();
        Msg += "'";
        _Logger->Log(Msg, 7);
        return false;
    }
    (*_CompiledResult) = {AssmResult.cbegin(), AssmResult.cend()};
    return true;


}

bool Shaderc_CompileToBinary(BG::Common::Logger::LoggingSystem* _Logger, std::string _Source, std::string _SourceName, shaderc_shader_kind _ShaderType, std::vector<uint32_t>* _CompiledResult, bool _Optimize, bool _Verbose) {
    assert(_Logger != nullptr);
    assert(_CompiledResult != nullptr);    

    // Setup Shaderc Compiler
    shaderc::Compiler Compiler;
    shaderc::CompileOptions Options;
	Options.SetTargetEnvironment( shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0 );
	Options.SetSourceLanguage( shaderc_source_language_glsl );

	std::vector<std::pair<std::string, std::string>> Definitions;
	Definitions.push_back(std::make_pair("__VK_GLSL__", "1"));

	for (std::pair<std::string, std::string> &DefinitionPairs : Definitions) {
		Options.AddMacroDefinition(DefinitionPairs.first, DefinitionPairs.second);
    }


    // If we want to add our own macros, do so like this (can be useful for say numlights or something like that):
    // options.AddMacroDefinition("MY_DEFINE", "1");


    // Set Optimization Enable/Disable
    if (_Optimize) {
        Options.SetOptimizationLevel(shaderc_optimization_level_size);
    }

    // Compile Now
    if (_Verbose) {
        std::string Msg = "Compiling Shader '" + _SourceName + "' To Binary With Optimizations ";
        if (_Optimize) {
            Msg += "'Enabled'";
        } else {
            Msg += "'Disabled'";
        }
        _Logger->Log(Msg, 3);
    }
    shaderc::SpvCompilationResult BinResult = Compiler.CompileGlslToSpv(_Source, _ShaderType, _SourceName.c_str(), Options);

    if (BinResult.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::string Msg = "Failed To Compile Shader '" + _SourceName + "' With Error '";
        Msg += BinResult.GetErrorMessage();
        Msg += "'";
        _Logger->Log(Msg, 7);
        return false;
    }
    (*_CompiledResult) = {BinResult.cbegin(), BinResult.cend()};
    
    if (_Verbose) {
        std::string LogMessage = "Compiled Shader Is '" + std::to_string(_CompiledResult->size() * 4) + "' Bytes Long";
        _Logger->Log(LogMessage, 2);
    }

    return true;

}

bool Vulkan_DynamicallyCompileShader(BG::Common::Logger::LoggingSystem* _Logger, std::string _Source, std::string _SourceName, shaderc_shader_kind _ShaderType, std::vector<uint32_t>* _CompiledResult, bool _Optimize, bool _Verbose) {
    assert(_Logger != nullptr);
    assert(_CompiledResult != nullptr);    


    // First, Preprocess
    std::string PreprocessedShader;
    bool PreprocessStatus = Shaderc_PreprocessShaderGLSL(_Logger, _Source, _SourceName, _ShaderType, &PreprocessedShader, _Verbose);
    if (!PreprocessStatus) {
        return false;
    }

    // Next, Compile To Bytecode
    return Shaderc_CompileToBinary(_Logger, PreprocessedShader, _SourceName, _ShaderType, _CompiledResult, _Optimize, _Verbose);
    

}

bool Vulkan_CreateShaderModule(BG::Common::Logger::LoggingSystem* _Logger, RenderData* _RD, const std::vector<uint32_t>* _ShaderBytecode, VkShaderModule* _ShaderModule) {
    assert(_Logger != nullptr);
    assert(_RD != nullptr);
    assert(_ShaderBytecode != nullptr);
    assert(_ShaderModule != nullptr);

    // Setup Shader Create Info
    VkShaderModuleCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreateInfo.codeSize = _ShaderBytecode->size() * sizeof(uint32_t);
    CreateInfo.pCode = _ShaderBytecode->data();

    // Now Setup The Module
    VkResult ShaderCreateStatus = vkCreateShaderModule(_RD->VulkanDevice_.device, &CreateInfo, nullptr, _ShaderModule);
    if (ShaderCreateStatus != VK_SUCCESS) {
        _Logger->Log("Error During Shader Module Creation, Failed To Create Shader Module", 8);
        return false;
    }
    return true;
}

}; // Close Namespace Internal


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
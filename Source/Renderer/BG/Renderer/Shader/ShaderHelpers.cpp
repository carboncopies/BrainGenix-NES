#include <BG/Renderer/Shader/Helpers.h>



namespace BG {
namespace NES {
namespace Renderer {
namespace Internal {

// copy pasted from vulkan-tutorial's shader_module page. Temporary, until we implement dynamic shader compilation.
std::vector<char> ReadFile(std::string filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;

}

std::string ReadFileToString(std::string _Filename) {
    std::vector<char> FileData = ReadFile(_Filename);
    std::string Out(FileData.begin(), FileData.end());
    return Out;

}



// Note: these shaderc tools should probably be made into a class with multithreading (one shader compiler per thread) - as the compilation process is *REALLY* slow otherwise.

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
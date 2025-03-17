#include "IgneousPipeline.h"
#include <cstdlib>
//#include <unistd.h>

namespace {
    bool ExecuteCommand(BG::Common::Logger::LoggingSystem* _Logger, const std::string& command, const std::filesystem::path& absPythonVenv) {
        _Logger->Log("Executing system command: " + command, 4);

        // Set up the environment for the virtual environment
        const std::string modifiedCommand =
            "export VIRTUAL_ENV=\"" + absPythonVenv.string() + "\" && "
            "export PATH=\"" + absPythonVenv.string() + "/bin:$PATH\" && "
            "unset PYTHONHOME && " + command;

        const int status = std::system(modifiedCommand.c_str());

        if (status != 0) {
            _Logger->Log("Command failed with exit code: " + std::to_string(status), 1);
            _Logger->Log("Failed command: " + modifiedCommand, 4);
            return false;
        }

        _Logger->Log("Command executed successfully", 4);
        return true;
    }

    void ClearQueue(BG::Common::Logger::LoggingSystem* _Logger, const std::string& queuePath) {
        _Logger->Log("Clearing queue directory: " + queuePath, 4);
        try {
            std::filesystem::remove_all(queuePath);
            std::filesystem::create_directory(queuePath);
            _Logger->Log("Queue directory cleared successfully", 4);
        } catch (const std::filesystem::filesystem_error& e) {
            _Logger->Log("Failed to clear queue directory: " + std::string(e.what()), 0);
            throw;
        }
    }
}
bool ProcessIgneousPipeline(BG::Common::Logger::LoggingSystem* _Logger,
    const std::string& datasetPath,
    const std::string& outputDir,
    bool enableDownsampling,
    int mipLevel,
    int parallelJobs,
    const std::string& pythonVenv) {
_Logger->Log("Starting Igneous processing pipeline", 3);
_Logger->Log("Parameters:", 4);
_Logger->Log("- Dataset path: " + datasetPath, 4);
_Logger->Log("- Output directory: " + outputDir, 4);
_Logger->Log("- Downsampling enabled: " + std::to_string(enableDownsampling), 4);
_Logger->Log("- MIP level: " + std::to_string(mipLevel), 4);
_Logger->Log("- Parallel jobs: " + std::to_string(parallelJobs), 4);
_Logger->Log("- Python venv: " + pythonVenv, 4);

try {
const std::filesystem::path absPythonVenv = std::filesystem::absolute(pythonVenv);
const std::filesystem::path absDatasetPath = std::filesystem::absolute(datasetPath);
const std::filesystem::path absOutputDir = std::filesystem::absolute(outputDir);

_Logger->Log("Resolved absolute paths:", 4);
_Logger->Log("- Python venv: " + absPythonVenv.string(), 4);
_Logger->Log("- Dataset: " + absDatasetPath.string(), 4);
_Logger->Log("- Output: " + absOutputDir.string(), 4);

std::filesystem::create_directories(absOutputDir);
const std::string queuePath = (absOutputDir / "queue").string();

// Use the igneous command directly from the venv's bin directory
const std::string igneousBin = "\"" + absPythonVenv.string() + "/bin/igneous\"";

//std::system((std::string("bash /home/rkoene/src/igneous_calls.sh ")+absDatasetPath.string()+" 0 60").c_str());
std::system(("bash "+absPythonVenv.string()+"/bin/activate && python3 ./Python/igneous_local.py --datapath "+absDatasetPath.string()+" --parallel 60").c_str());
return true;
// pid_t pid = fork();
// if (pid == 0) {
//     // Child process
//     execl("/bin/bash", "bash", "/home/rkoene/src/igneous_calls.sh", absDatasetPath.string().c_str(), nullptr);
// }

/*
if (enableDownsampling) {
_Logger->Log("Starting image downsampling", 3);
ClearQueue(_Logger, queuePath);

// Use the igneous command directly
const std::string downsampleCmd = igneousBin + " image downsample " +
absDatasetPath.string() + " --mip " + std::to_string(mipLevel) +
" --queue " + queuePath;

if (!ExecuteCommand(_Logger, downsampleCmd, absPythonVenv)) {
_Logger->Log("Image downsampling phase failed", 1);
return false;
}

const std::string executeCmd = igneousBin + " --parallel " + std::to_string(parallelJobs) + " execute -x " + queuePath;


if (!ExecuteCommand(_Logger, executeCmd, absPythonVenv)) {
_Logger->Log("Downsampling queue execution failed", 1);
return false;
}

_Logger->Log("Image downsampling completed successfully", 3);
}

_Logger->Log("Starting mesh generation pipeline", 3);
ClearQueue(_Logger, queuePath);

// Use the igneous command directly
const std::string forgeCmd = igneousBin + " mesh forge " +
absDatasetPath.string() + " --mip " + std::to_string(mipLevel) +
" --queue " + queuePath;

if (!ExecuteCommand(_Logger, forgeCmd, absPythonVenv)) {
_Logger->Log("Mesh forging phase failed", 1);
return false;
}

const std::string executeForgeCmd = igneousBin + " --parallel " + std::to_string(parallelJobs) + " execute -x " + queuePath;

if (!ExecuteCommand(_Logger, executeForgeCmd, absPythonVenv)) {
_Logger->Log("Mesh forge queue execution failed", 1);
return false;
}

_Logger->Log("Mesh forging completed successfully", 3);

ClearQueue(_Logger, queuePath);
const std::string mergeCmd = igneousBin + " mesh merge " +
absDatasetPath.string() + " --queue " + queuePath;

if (!ExecuteCommand(_Logger, mergeCmd, absPythonVenv)) {
_Logger->Log("Mesh merging phase failed", 1);
return false;
}

const std::string executeMergeCmd = igneousBin + " --parallel " + std::to_string(parallelJobs) + " execute -x " + queuePath;

if (!ExecuteCommand(_Logger, executeMergeCmd, absPythonVenv)) {
_Logger->Log("Mesh merge queue execution failed", 1);
return false;
}

_Logger->Log("Mesh merging completed successfully", 3);

_Logger->Log("Igneous processing completed successfully", 2);
_Logger->Log("Generated data location: " + absOutputDir.string(), 3);
_Logger->Log("Segmentation path: " + absDatasetPath.string(), 3);

return true;

} catch (const std::filesystem::filesystem_error& e) {
_Logger->Log("Filesystem error: " + std::string(e.what()), 0);
return false;
*/
} catch (const std::exception& e) {
_Logger->Log("Unexpected error: " + std::string(e.what()), 0);
return false;
}
}
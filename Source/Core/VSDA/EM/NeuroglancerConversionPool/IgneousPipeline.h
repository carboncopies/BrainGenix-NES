#pragma once

#include <string>
#include <filesystem>

#include <BG/Common/Logger/Logger.h>

/**
 * @file IgneousPipeline.h
 * @brief Header for Igneous pipeline processing functionality
 */

/**
 * @brief Processes a dataset through the Igneous pipeline
 * @param _Logger Pointer to the logging system
 * @param datasetPath Path to the input dataset
 * @param outputDir Directory for output files
 * @param enableDownsampling Whether to enable downsampling (default: false)
 * @param mipLevel MIP level for processing (default: 0)
 * @param parallelJobs Number of parallel jobs (default: 4)
 * @param pythonVenv Path to Python virtual environment (default: "../venv")
 * @return true if processing succeeded, false otherwise
 * @throws std::filesystem::filesystem_error on filesystem operations failure
 */
bool ProcessIgneousPipeline(BG::Common::Logger::LoggingSystem* _Logger,
                          const std::string& datasetPath,
                          const std::string& outputDir,
                          bool enableDownsampling = false,
                          int mipLevel = 0,
                          int parallelJobs = 4,
                          const std::string& pythonVenv = "../venv");
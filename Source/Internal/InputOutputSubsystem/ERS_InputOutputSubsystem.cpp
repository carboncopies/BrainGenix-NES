//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_InputOutputSubsystem.h>

ERS_InputOutputSubsystem::ERS_InputOutputSubsystem(
    ERS_LoggingSystem* Logger,
    YAML::Node SystemConfiguration, 
    std::vector<std::pair<std::string, std::string>> ArgumentPair) {

  Logger_ = Logger;
  Logger_->Log("Initializing Input/Output Subsystem", 5);

  // Setup Classes
  AssetIndexIOManager_ = std::make_unique<ERS_AssetIndexInputOutputManager>(Logger_);

  // Get Database Loading / File Loading Config
  Logger_->Log("Reading Configuration For 'BOOL' 'UseDatabaseLoading'", 1);
  try {
    UseDatabase_ = SystemConfiguration["UseDatabaseLoading"].as<bool>();
  } catch (YAML::TypedBadConversion<bool>&) { // Config Param Doesn't Exist
    Logger_->Log("Configuration Error, Parameter 'UseDatabaseLoading' Is Not "
                 "In Config, System Will Default To False",
                 8);
    UseDatabase_ = false;
  }

  // If Using DB Loading, Get DB Params
  if (UseDatabase_) {
    Logger_->Log(
        "Database Loading Enabled, Reading Config For Database Parameters", 5);

    /*

        DATABASE LOADING STUFF
       HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    */

  } else {
    Logger_->Log("Database Lading Disabled, Reading Config For Asset Path", 5);

    Logger_->Log("Traversing Arguments To Check For Project Dir", 3);
    bool HasProjectDirectory = false;
    for (unsigned int i = 0; i < ArgumentPair.size(); i++) {
      std::pair<std::string, std::string> CurrentPair = ArgumentPair[i];
      if (CurrentPair.first == "ProjectDirectory") {
        Logger_->Log("Found Directory In Arguments, Will Use That", 4);
        AssetPath_ = CurrentPair.second;
        HasProjectDirectory = true;
        break;
      }

    }


    if (!HasProjectDirectory) {
      try {
        Logger_->Log("Reading Configuration For 'STRING' 'DefaultProjectDirectory'", 1);
        AssetPath_ = SystemConfiguration["DefaultProjectDirectory"].as<std::string>();
      } catch (YAML::TypedBadConversion<std::string>&) {
        Logger_->Log("Configuration Error, Parameter 'DefaultProjectDirectory' Is Not In Config, System Will Exit", 10);
        exit(1);
      }
    }
  }

  // Index Already Used AssetIDs
  IndexUsedAssetIDs();
}

ERS_InputOutputSubsystem::~ERS_InputOutputSubsystem() {

  Logger_->Log("Input/Output Subsystem Destructor Called", 6);

  // Save Asset Index Metadata
  Logger_->Log("Saving Asset Index Metadata", 4);
  std::unique_ptr<ERS_STRUCT_IOData> Data = std::make_unique<ERS_STRUCT_IOData>();
  AssetIndexIOManager_->WriteAssetIndex(Data.get());
  WriteAsset(0, Data.get());
  Logger_->Log("Finished Saving Asset Index Metadata", 5);
}

void ERS_InputOutputSubsystem::UpdateAssetPath(std::string AssetPath) {

  // Update Asset Path
  Logger_->Log(
      std::string(std::string("Changing Asset Repository Directory To '") +
                  std::string(AssetPath) + std::string("'"))
          .c_str(),
      5);
  AssetPath_ = AssetPath;
}

long ERS_InputOutputSubsystem::AllocateAssetID() {

  // Make This Thread Safe
  std::unique_lock<std::mutex> lock(LockAssetIDAllocation_);

  // Ensure Only Unique IDs Are Used
  bool FoundUniqueID = false;
  long ProposedID;
  while (!FoundUniqueID) {
  
    // Propose Random Number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 2147483647);
    ProposedID = distr(gen);
  
    // Ensure Number Isn't Already Used
    bool Match = false;
    for (unsigned long i = 0; i < UsedAssetIDs_.size(); i++) {
      if (UsedAssetIDs_[i] == ProposedID) {
        Match = true;
        break;
      }
    }

    // Exit Loop, Use Number When Unique One Found
    if (!Match) {
      break;
    }

  }


  UsedAssetIDs_.push_back(ProposedID);

  return ProposedID;
}

std::vector<long>
ERS_InputOutputSubsystem::BatchAllocateIDs(size_t NumberIDs) {

  std::vector<long> IDs;
  IDs.reserve(NumberIDs);

  size_t CurSize = UsedAssetIDs_.size();
  size_t NewSize = CurSize + NumberIDs;

  UsedAssetIDs_.reserve(NewSize);

  for (unsigned long i = CurSize; i < NewSize; i++) {
    unsigned long AssetID = AllocateAssetID();
    UsedAssetIDs_.push_back(AssetID);
    IDs.push_back(AssetID);
  }

  return IDs;
}

void ERS_InputOutputSubsystem::IndexUsedAssetIDs() {

  Logger_->Log("Indexing Used Asset IDs");
  UsedAssetIDs_ = std::vector<long>();

  // If Using Database Loading
  if (UseDatabase_) {

    // Do Database Indexing here...

  } else { // Default To Regular File Loading

    // Get List Of Files At Path
    try {
      for (const auto &Entry :
           std::filesystem::directory_iterator(std::string(AssetPath_))) {

        // Get File Path, Convert Backslashes Into Forwardslashes
        std::string FilePath{Entry.path().u8string()};
        std::replace(FilePath.begin(), FilePath.end(), '\\', '/');
        
        // Sanity Check For Directories
        if (FilePath.find_last_of("/") > FilePath.find_last_of(".")) {
            continue;
        }

        // Convert To Long, Throw Log Message If Not Number
        try {

          int LastPeriod = FilePath.find_last_of(".");
          std::string PathWithoutExtension = FilePath.substr(0, LastPeriod);

          int LastSlash = PathWithoutExtension.find_last_of("/");
          std::string AssetIDString = PathWithoutExtension.substr(LastSlash + 1, PathWithoutExtension.length());

          long ID = std::stoi(AssetIDString.c_str());

          if (ID >= 0) {
            UsedAssetIDs_.push_back(ID);

            // Log Checked Out ID
            Logger_->Log(std::string(std::string("AssetID '") +
                                     std::to_string(ID) +
                                     std::string("' In Use"))
                             .c_str(),
                         3);

          } else {

            Logger_->Log(std::string(std::string("AssetID '") +
                                     std::to_string(ID) +
                                     std::string("' Cannot Be Negative"))
                             .c_str(),
                         8);
          }

        } catch (std::invalid_argument&) {

          // Log Error
          Logger_->Log(
              std::string(
                  std::string("AssetID Identification Failed On Asset '") +
                  FilePath + std::string("', Make Sure These Are Numbers"))
                  .c_str(),
              9);

        } catch (std::out_of_range&) {

          Logger_->Log(
              std::string(
                  std::string("AssetID Identification Failed On Asset '") +
                  FilePath + std::string("', Invalid Asset ID"))
                  .c_str(),
              9);
        }
      }
    } catch (std::filesystem::filesystem_error&) {
      Logger_->Log("Error Indexing Assets, Local File Loading Enabled But "
                   "Asset Directory Does Not Exist",
                   9);
    }
  }

  // Summarize Checked Out IDs
  Logger_->Log(std::string(std::string("Identified ") +
                           std::to_string(UsedAssetIDs_.size()) +
                           std::string(" Asset IDs Are In Use"))
                   .c_str(),
               4);

  // Load Asset Metadata
  Logger_->Log("Attempting To Load Asset Metadata Index", 3);
  std::unique_ptr<ERS_STRUCT_IOData> Data = std::make_unique<ERS_STRUCT_IOData>();
  bool ReadStatus = ReadAsset(0, Data.get());
  if (!ReadStatus) {
    Logger_->Log("Failed To Load Project Index, Is The Path/Database Valid?", 10);
    exit(EXIT_FAILURE);
  }
  AssetIndexIOManager_->LoadAssetIndex(Data.get());
  Logger_->Log("Finished Loading Asset Metadata Index", 4);

  // Compare Indexes Of Assets
  Logger_->Log("Comparing Asset Metadata With Indexed Assets", 3);
  ERS_FUNCTION_CompareIndexDelta(
      Logger_, std::make_shared<std::vector<long>>(UsedAssetIDs_),
      AssetIndexIOManager_->AssetIDsFound_);
  Logger_->Log("Finished Performing Asset Metadata Sanity Check", 4);
}

bool ERS_InputOutputSubsystem::ReadAsset(
    long AssetID, ERS_STRUCT_IOData* OutputData) {

  // Asset ID Sanity Check
  if (AssetID < 0) {
    Logger_->Log(std::string(std::string("Error Reading Asset '") +
                             std::to_string(AssetID) +
                             std::string("', ID Is Negative"))
                     .c_str(),
                 9);
    return false;
  }

  // Populate Metadata
  AssetIndexIOManager_->ReadAssetIndex(AssetID, OutputData);

  // Start Clock To Measure File Metadata
  auto StartTime = std::chrono::high_resolution_clock::now();
  bool ReadSuccess = false;
  float FileSize = 0;

  // If Database Loading
  if (UseDatabase_) {

    // Load From DB

  } else {

    // Generate File Path
    std::string FilePath =
        AssetPath_ + std::to_string(AssetID) + std::string(".ERS");

    struct stat Buffer;
    int FileStatus = stat(FilePath.c_str(), &Buffer);
    FileSize = Buffer.st_size + 1;

    if (FileStatus == 0) {

      // Allocate Memory
      OutputData->Data.reset(new unsigned char[Buffer.st_size + 1]);
      if (OutputData->Data) {

        FILE *Stream = fopen(FilePath.c_str(), "rb");
        if (Stream) {

          [[maybe_unused]]size_t _ = fread(OutputData->Data.get(), sizeof(unsigned char), Buffer.st_size,Stream);
                
          OutputData->Data.get()[Buffer.st_size] = '\0';
          fclose(Stream);
          OutputData->HasLoaded = true;
          ReadSuccess = true;

        } else {
          Logger_->Log(std::string(std::string("Error Loading Asset '") +
                                   std::to_string(AssetID) +
                                   std::string("', Failed To Open Filestream"))
                           .c_str(),
                       9);
          OutputData->HasLoaded = false;
          ReadSuccess = false;
        }

      } else {
        Logger_->Log(std::string(std::string("Error Loading Asset '") +
                                 std::to_string(AssetID) +
                                 std::string("', Memory Allocation Failed"))
                         .c_str(),
                     9);
        OutputData->HasLoaded = false;
        ReadSuccess = false;
      }

    } else {
      Logger_->Log(std::string(std::string("Error Loading Asset '") +
                               std::to_string(AssetID) +
                               std::string("', File Not Found"))
                       .c_str(),
                   9);
      OutputData->HasLoaded = false;
      ReadSuccess = false;
    }
  }

  // Measure End Time, Calculate Metadata
  auto FinishTime = std::chrono::high_resolution_clock::now();
  float Duration = std::chrono::duration<float>(
                       std::chrono::duration_cast<std::chrono::nanoseconds>(
                           FinishTime - StartTime))
                       .count();

  OutputData->LoadTime_s = Duration;
  OutputData->Size_B = FileSize;
  OutputData->Size_KB = FileSize / 1000;
  OutputData->Size_MB = FileSize / 1000000;
  OutputData->Size_GB = FileSize / 1000000000;

  OutputData->LoadSpeed_KBs = (FileSize / 1000) / Duration;
  OutputData->LoadSpeed_MBs = (FileSize / 1000000) / Duration;
  OutputData->LoadSpeed_GBs = (FileSize / 1000000000) / Duration;

  // Return Data
  return ReadSuccess;
}


std::vector<bool> ERS_InputOutputSubsystem::BatchReadAssets(
    std::vector<long> AssetIDs,
    std::vector<ERS_STRUCT_IOData*> AssetDatas) {

  // Check Lengths Of Inputs
  if (AssetIDs.size() != AssetDatas.size()) {
    Logger_->Log("(BatchReadAssets) Input Vectors Are Not The Same Length, "
                 "Undefined Behavior May Occur",
                 10);
  }

  // Iterate And Read
  std::vector<bool> StatusVector;
  for (size_t i = 0; i < AssetIDs.size(); i++) {
    StatusVector.push_back(ReadAsset(AssetIDs[i], AssetDatas[i]));
  }

  // Return Status
  return StatusVector;
}



bool ERS_InputOutputSubsystem::WriteAsset(
    long AssetID, ERS_STRUCT_IOData* InputData) {

  // Asset ID Sanity Check
  if (AssetID < 0) {
    Logger_->Log(std::string(std::string("Error Writing Asset '") +
                             std::to_string(AssetID) +
                             std::string("', ID Is Negative"))
                     .c_str(),
                 9);
    return false;
  }



  InputData->AssetModificationDate = GetCurrentTime();
  AssetIndexIOManager_->UpdateAssetIndex(AssetID, InputData);

  // Start Clock To Measure File Metadata
  auto StartTime = std::chrono::high_resolution_clock::now();
  bool Success = false;

  // If Database Loading
  if (UseDatabase_) {

    // Load From DB

  } else {

    // Generate File Path
    std::string FilePath =
        AssetPath_ + std::to_string(AssetID) + std::string(".ERS");

    // Check Filesize
    if (InputData->Size_B > 0) {

      FILE *Stream = fopen(FilePath.c_str(), "wb");

      if (Stream) {

        fwrite(InputData->Data.get(), 1,
               sizeof(unsigned char) * InputData->Size_B, Stream);
        fclose(Stream);
        Success = true;

      } else {
        Logger_->Log(std::string(std::string("Error Writing Asset '") +
                                 std::to_string(AssetID) +
                                 std::string("', Failed To Open Filestream"))
                         .c_str(),
                     9);
        Success = false;
      }

    } else {
      Logger_->Log(std::string(std::string("Error Writing Asset '") +
                               std::to_string(AssetID) +
                               std::string("', Indicated 'Size_B' Is Invalid"))
                       .c_str(),
                   9);
      Success = false;
    }
  }

  // Measure End Time, Calculate Metadata
  auto FinishTime = std::chrono::high_resolution_clock::now();
  float Duration = std::chrono::duration<float>(
                       std::chrono::duration_cast<std::chrono::nanoseconds>(
                           FinishTime - StartTime))
                       .count();

  InputData->WriteTime_s = Duration;

  InputData->WriteSpeed_KBs = (InputData->Size_B / 1000) / Duration;
  InputData->WriteSpeed_MBs = (InputData->Size_B / 1000000) / Duration;
  InputData->WriteSpeed_GBs = (InputData->Size_B / 1000000000) / Duration;

  return Success;
}

std::vector<bool> ERS_InputOutputSubsystem::BatchWriteAssets(
    std::vector<long> AssetIDs,
    std::vector<ERS_STRUCT_IOData*> AssetDatas) {

  // Check Lengths Of Inputs
  if (AssetIDs.size() != AssetDatas.size()) {
    Logger_->Log("(BatchWriteAssets) Input Vectors Are Not The Same Length, "
                 "Undefined Behavior May Occur",
                 10);
  }

  // Iterate And Write
  std::vector<bool> StatusVector;
  for (size_t i = 0; i < AssetIDs.size(); i++) {
    StatusVector.push_back(WriteAsset(AssetIDs[i], AssetDatas[i]));
  }

  return StatusVector;
}



std::string ERS_InputOutputSubsystem::GetCurrentTime() {


  std::time_t RawCurrentTime;
  std::tm *TimeInformation;
  char TimeBuffer[80];

  std::time(&RawCurrentTime);
  TimeInformation = std::localtime(&RawCurrentTime);

  std::strftime(TimeBuffer, 80, "%Y-%m-%d-%H-%M-%S", TimeInformation);
  std::string CurrentTime = std::string(TimeBuffer);

  return CurrentTime;

}
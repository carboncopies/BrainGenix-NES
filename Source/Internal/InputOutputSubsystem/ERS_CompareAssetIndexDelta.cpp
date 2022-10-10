//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_CompareAssetIndexDelta.h>




void ERS_FUNCTION_CompareIndexDelta(ERS_LoggingSystem* Logger_, std::shared_ptr<std::vector<long>> UsedAssetIDIndex, std::vector<long> AssetIndexIOM) {

    Logger_->Log("Comparing Indexed Assets With Asset Metadata Index", 4);

    // Make Copy Of Used Asset ID Index To Check Against
    std::vector<long> RemainingAssetIDs = *UsedAssetIDIndex;
    std::vector<long> UsedAssetIDs = *UsedAssetIDIndex;

    // Iterate Through All Asset Index Metadata
    for (long i = 0; i < (long)AssetIndexIOM.size(); i++) {

        // Get Index Checking
        long IndexChecking = AssetIndexIOM[i];
        Logger_->Log(std::string(std::string("Checking Asset Index '") + std::to_string(IndexChecking) + std::string("'")).c_str(), 3);

        // Check If Used asset ID Index Has Asset Found In Metadata
        bool Found = false;
        for (long i = 0; i < (long)UsedAssetIDs.size(); i++) {
            if (UsedAssetIDs[i] == IndexChecking) {
                Found = true;
                Logger_->Log(std::string(std::string("Matched Asset Pair With ID '") + std::to_string(IndexChecking) + std::string("'")).c_str(), 4);
                break;
            }
        }

        // If Found, Remove From IDs Of Remaining Indexes
        if (Found) {
            for (long i = 0; i < (long)RemainingAssetIDs.size(); i++) {
                if (RemainingAssetIDs[i] == IndexChecking) {
                    RemainingAssetIDs.erase(RemainingAssetIDs.begin() + i);
                    break;
                }
            }
        } else {
            Logger_->Log(std::string(std::string("Asset Metadata Index References Asset '") + std::to_string(IndexChecking) + std::string("', But This Asset Does Not Exist")).c_str(), 8);
        }


    }

    // Iterate Through Any Remaining Assets (Should Be Zero)
    // Would Be Caused By An Asset Not Being In Metadata For Some Reason
    for (long i = 0; i < (long)RemainingAssetIDs.size(); i++) { 

        long IndexChecking = RemainingAssetIDs[i];
        Logger_->Log(std::string(std::string("No Asset Metadata Entry Exists For Asset '") + std::to_string(IndexChecking) + std::string("'")).c_str(), 8);

    }


    // Log Comparison Finish
    Logger_->Log("Finished Comparing Index With Metadata", 5);

}

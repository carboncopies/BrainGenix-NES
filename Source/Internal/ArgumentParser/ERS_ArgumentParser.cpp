//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_ArgumentParser.h>


namespace BrainGenix {
namespace ERS {
namespace Module {


ArgumentParser::ArgumentParser(ERS_LoggingSystem* Logger) {

    Logger_ = Logger;
    Logger_->Log("Initialiizng Argument Parser Subsystem", 4);

    ArgumentPairs_.clear();
    Arguments_.clear();
    ArgumentString_ = "";

}

ArgumentParser::~ArgumentParser() {

    Logger_->Log("Argument Parser Destructor Called", 6);

}

bool ArgumentParser::ParseArguments(int NumberArguments, char** ArgumentValues) {

    Logger_->Log("Argument Parser Invoked, Populating Internal State With Argument Data", 5);
    bool HasErrors = false;

    // Clear Args
    Logger_->Log("Clearing Internal State", 2);
    ArgumentPairs_.clear();
    Arguments_.clear();
    ArgumentString_ = "";
    Logger_->Log("Finished Clearing Internal State", 1);

    // Populate Argument List, Argument String
    Logger_->Log("Creating Argument Vector", 3);
    for (int i = 0; i < NumberArguments; i++) {

        std::string CurrentArgument = std::string(ArgumentValues[i]);
        ArgumentString_ += CurrentArgument + " ";
        Arguments_.push_back(CurrentArgument);

    }
    Logger_->Log("Finished Creating Argument Vector", 2);

    

    // Create Internal Argument Pairs
    Logger_->Log("Creating Argument Pair Vector", 4);
    std::vector<std::string> TmpArguments = Arguments_;
    TmpArguments.erase(TmpArguments.begin());
    for (unsigned int i = 0; i < (TmpArguments.size() / 2); i++) {
        
        // Should Skip Every Other Argument
        int Index = i*2;

        // Sanity Check About Array Sizing
        if ((long unsigned int)Index + 1 >= TmpArguments.size()) {
            Logger_->Log("Error Parsing Arguments, Index Out Of Range", 8);
            HasErrors = true;
            break;
        }

        std::string Value1 = TmpArguments[Index];
        std::string Value2 = TmpArguments[Index+1];

        // Strip Out "-" From Key (Value1)
        if (Value1.substr(0, 1) != std::string("-")) {
            Logger_->Log(std::string("Invalid Argument Key '") + Value1 + "' Will Attempt To Parse Anyway", 7);
            HasErrors = true;
        } else { 
            Value1 = Value1.substr(1, Value1.length());
        }


        Logger_->Log(std::string("Found Argument Key '") + Value1 + "' With Value '" + Value2 + "'", 1);

        std::pair<std::string, std::string> ArgPair = std::make_pair(Value1, Value2);
        ArgumentPairs_.push_back(ArgPair);

    }
    Logger_->Log("Finished Creating Argument Pair Vector", 3);
    return !HasErrors;

}

std::string ArgumentParser::GetArgumentString() {
    return ArgumentString_;
}

std::vector<std::pair<std::string, std::string>> ArgumentParser::GetArgumentPairs() {
    return ArgumentPairs_;
}


}
}
}

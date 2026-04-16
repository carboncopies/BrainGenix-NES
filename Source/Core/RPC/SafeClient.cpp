//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/rpc_error.h>

// Internal Libraries (BG convention: use <> instead of "")

#include <RPC/SafeClient.h>

#include <RPC/RPCHandlerHelper.h>
#include <RPC/APIStatusCode.h>

#include <Version.h>


namespace BG {


bool SafeClient::RunVersionCheck() {

    if (Client_ == nullptr) {
        return false;
    }

    // Run a query to force it to connect (or fail)
    std::string Temp;
    MakeJSONQuery("GetAPIVersion", &Temp);
    

    // Update our internal status of how the connection is doing
    ::rpc::client::connection_state RPCStatus = Client_->get_connection_state();
    if (RPCStatus != ::rpc::client::connection_state::connected) {
        Logger_->Log("Unable to connect to RPC Service", 3);
        LastState_ = false;
    } else {
        if (!LastState_) {
            Logger_->Log("NES RPC Connection SERVICE_HEALTHY", 1);
            LastState_ = true;
        }
    }

    // Check Version again (used as a heartbeat 'isAlive' check)
    std::string Version = "undefined";
    bool Status = MakeJSONQuery("GetAPIVersion", &Version, true);
    if (!Status) {
        Logger_->Log("Failed To Get RPC Service API Version String", 1);
        return false;
    }

    if (Version != VERSION) {
        Logger_->Log("Client/Server Version Mismatch! This might make stuff break. Server " + Version + " Client " + VERSION, 9);
        return false;
    }
    return true;


}

bool SafeClient::Connect() {
    IsHealthy_ = false;
    Client_ = nullptr;

    // Extract RPC Service Client Parameters, Connect, Configure
    std::string NESHost = RPCHost_;
    int NESPort = RPCPort_;
    int NESTimeout_ms = RPCTimeout_ms;
    
    Logger_->Log("Connecting to RPC Service on port: " + std::to_string(NESPort), 1);
    Logger_->Log("Connecting to RPC Service on host: " + NESHost, 1);
    Logger_->Log("Connecting to RPC Service with timeout_ms of: " + std::to_string(NESTimeout_ms), 1);


    try {
        Client_ = std::make_unique<::rpc::client>(NESHost.c_str(), NESPort);
    } catch (std::system_error& e) {
        Logger_->Log("Cannot find RPC Service host (authoritative)", 9);
        return false;
    }
    Client_->set_timeout(NESTimeout_ms);

    // Call GetVersion On Remote - allows us to check that versions match, but also ensures the connection is ready
    bool Status = RunVersionCheck();
    if (Status) {
        IsHealthy_ = true;
    }
    return Status;

}


SafeClient::SafeClient(BG::Common::Logger::LoggingSystem* _Logger) {

    Logger_ = _Logger;
    RequestExit_ = false;
    ClientManager_ = std::thread(&SafeClient::RPCManagerThread, this);

}

SafeClient::~SafeClient() {
    RequestExit_ = true;
    ClientManager_.join();
}

bool SafeClient::SetTimeout(int _Timeout_ms) {
    RPCTimeout_ms = _Timeout_ms;
    return true;
}
bool SafeClient::SetHostPort(std::string _Host, int _Port) {
    RPCHost_ = _Host;
    RPCPort_ = _Port;
    return true;
}

void SafeClient::Reconnect() {
    IsHealthy_ = false;
}

bool SafeClient::MakeJSONQuery(std::string _Route, std::string* _Result, bool _ForceQuery) {
    if (!_ForceQuery && !IsHealthy_) {
        return false;
    }
    try {
        (*_Result) = Client_->call(_Route.c_str()).as<std::string>();
    } catch (::rpc::timeout& e) {
        Logger_->Log("RPC Connection Timed Out", 3);
        return false;
    } catch (::rpc::rpc_error& e) {
        Logger_->Log("RPC Remote Returned Error", 3);
        return false;
    } catch (std::system_error& e) {
        Logger_->Log("Cannot Cannot Talk To RPC Host", 3);
        return false;
    }
    return true;
}


bool SafeClient::MakeJSONQuery(std::string _Route, std::string _Query, std::string* _Result, bool _ForceQuery) {
    if (!_ForceQuery && !IsHealthy_) {
        return false;
    }
    try {
        (*_Result) = Client_->call(_Route.c_str(), _Query).as<std::string>();
    } catch (::rpc::timeout& e) {
        Logger_->Log("API Connection timed out!",3);
        return false;
    } catch (::rpc::rpc_error& e) {
        Logger_->Log("API remote returned RPC error",3);
        return false;
    } catch (std::system_error& e) {
        Logger_->Log("Cannot talk to RPC Service host",3);
        return false;
    }
    return true;
}


void SafeClient::RPCManagerThread() {

    // Wait Until Config Valid
    while (RPCHost_ == "") {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }


    // Enter loop
    while (!RequestExit_) {

        // Check Version
        bool IsHealthy = RunVersionCheck();

        // If not healthy, re-establish connection, retry stuff... For now, nothing...
        if (!IsHealthy) {
            if (!Connect()) {
                Logger_->Log("Failed To Reconnect To RPC Service (Connect() returned false)", 3);
            }
        }

        // Wait 1000ms before polling again
        std::this_thread::sleep_for(std::chrono::milliseconds(500));


    }
}

}; // Close Namespace BG
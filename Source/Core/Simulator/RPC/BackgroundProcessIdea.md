# Background API Process Generalization

We came up with this as an idea for how to generalize putting API calls into the
background. The idea was than an extra parameter `InBackground` would be used
and it would change the process as follows:

```
1. Thread from Thread pool picks up API call received.
2. Parameters are decoded from JSON received.
3. The call is carried out.
4. The result is sent back.
```

Changes to:

```
1. Thread from Thread pool picks up API call received.
2. Parameters are decoded from JSON received.
4. The process creates a process Status/Result object that is attached to a list of active background processes (or maybe it's always associated with that Thread)
5. That Thread's Status is set to busy.
6. Respond with "thanks, process started! Woohoo!"
7. Go do the thing.
8. Meanwhile... in a universe far far away... API calls may come in requesting an update on the status of that call (by request ID)... that identifies the stored object with Status/Result info and that is returned.
9. When done with the thing, update the Status/Result object of the Thread.
```

Here is the code that I put into a call but should be in HandlerData:

```
    // *** Putting this here during development of the background processing... will probably
    //     move it all into HanderData for cleanliness. ;-)
    // Note that the ReqID of the API call is available for NES Requests as shown in RPCManager::NESRequest,
    // but, in case this is called from somewhere other than an NES Request, we may as well generate brand
    // new IDs for requests being run in the background and return that with the initial response.

    bool InBackground = false;
    long BgRequestID = -1;
    Handle.GetParBool("InBackgound", InBackground); // Optional.
    if (InBackground) {
        // Make a background process status/result object and set status as busy.
        BgRequestID = RPCManager_->GetBgRequestID();
        nlohmann::json BgStatusResultJSON;
        BgStatusResultJSON["BgRequestID"] = BgRequestID;
        BgStatusResultJSON["StatusCode"] = int(BGStatusSimulationBusy);
        RPCManager_->RegisterBgAPIProcess(BgRequestID, &BgStatusResultJSON);
        // Send back a response notifying that the process has started successfully.
    }
```

and

```
    if (InBackground) {
        if (!RPCManager_->UnRegisterBgAPIProcess(BgRequestID)) {
            Logger_->Log("Background process status/result object not found", 8);
        }
    }
```

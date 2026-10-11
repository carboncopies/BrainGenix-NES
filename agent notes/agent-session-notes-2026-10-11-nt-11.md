# Agent session notes 2026-10-11 (dev_frame NT-11, issue #88)

- Route handlers are public and take a JSON string, but `GeometryRPCInterface`/`ModelRPCInterface` need an `RPCManager*` at construction (they call `AddRoute`). `TestUtil/RouteHarness.h` keeps one loopback manager per process (callback target set so the destructor does not hang, #105), a registry and both interfaces. Every ctest test is its own process, so each pays the ~0.7 s manager start/stop; run the binary directly with `--gtest_filter` to iterate (all 70 tests take ~1.5 s in one process).
- `HandlerData::ErrResponse` stores the request in the model journal even for status 2 and 5 (dev_frame #137). `StoredRequestsToNESRequestArray()` is the way to read the journal from a test (`StoredRequests` is protected).
- Vec3 request fields are named `<Name>X_um`, `<Name>Y_um`, `<Name>Z_um` (`Rotation*_rad` for the Box); Opto uses `InjectLocation*`.
- Create all shapes before the first compartment in a test: compartments cache `ShapePtr` into the shape vector (#112).
- `LIFCReceptor` pairs with the same source and destination neuron are merged by `AddToAbstractedFunctional` (NaN weight with zero peak conductance, #108); tests use a fresh simulation per receptor.
- JSON `null` for a list field (`ControlData`, `TimeNeuronPairs`) iterates zero times and returns status 0 (ControlData is cleared). Not asserted as a bug; wrong-typed lists are tested with strings and non-numeric elements.

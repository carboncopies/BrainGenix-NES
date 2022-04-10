[![Codacy Badge](https://app.codacy.com/project/badge/Grade/64207ebc26b34f24b1ad39ad43df315d)](https://www.codacy.com/gh/carboncopies/BrainGenix-NES/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=carboncopies/BrainGenix-NES&amp;utm_campaign=Badge_Grade)

# This project is currently in development

# What's BrainGenix-NES?

BrainGenix Neuron Emulation System (NES) aims to be a distributed biological neuron simulator designed for the emulation of human brains. The simulator uses Apache Zookeeper and other production-grade systems to ensure that it is both stable and fault-tolerant. It is designed with scalability in mind to run with one neuron or a hundred billion. NES will work in conjunction with other BrainGenix programs, including the management API.

## Key Features
 - NES runs with as many neurons as your simulation has, be it one or a billion.
 - Performance is also an issue with simulations, so NES supports almost every model. It's based on the popular Brian simulator, so any model that can be written as a differential equation is supported.
 - Based on Apache Zookeeper, NES uses the leader-follower system. Based on this system, even a fault with the leader node won't cause the system to crash.
 - NES has an extensive Simulation API that allows any other system to interact with the simulation that NES runs.
 - Management of the entire BrainGenix software suite can be acomplished via BrainGenix's unified CLI or GUI.
 - NES also interfaces with ERS and STS, the other parts of BrainGenix.
 
# Documentation
Currently the best source of information about NES can be found within our technical specifications document below:
https://docs.google.com/document/d/1tVIB7KQcSRKNU_df8oPVR3wyBHagjv2xvqaMy-oDg18/edit#

You can also view our Trello Boards here: https://trello.com/braingenixdepartment



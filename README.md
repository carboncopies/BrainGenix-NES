[![Codacy Badge](https://api.codacy.com/project/badge/Grade/df1c58e2d172468b9b39c113c52e9f13)](https://app.codacy.com/gh/carboncopies/BrainGenix?utm_source=github.com&utm_medium=referral&utm_content=carboncopies/BrainGenix&utm_campaign=Badge_Grade)
# What's BrainGenix?

BrainGenix is a distributed biological neuron simulator designed for the emulation of human minds. The simulator uses Apache Zookeeper and other production-grade systems to ensure that it's both stable and fault-tolerant. It's designed with scalability in mind to run with one neuron or a hundred billion.

## Key Features
 - BrainGenix runs with as many neurons as your simulation has, be it one or a billion.
 - Performance is also an issue with simulations, so BrainGenix supports almost every model. It's based on the popular Brian simulator, so any model that can be written as a differential equation is supported.
 - Based on Apache Zookeeper, BrainGenix uses the leader-follower system. Based on this system, even a fault with the leader node won't cause the system to crash.
 - BrainGenix has an extensive Addon interface that allows any other system to interact with the simulation that BrainGenix runs.
 - With whole brain emulation, security is a very high priority, so we've included many features to help protect the simulation from unauthorized access.

# Documentation
Currently, our documentation lives in Google Docs and is at the link below: 
https://docs.google.com/document/d/1R2Zk7qGkwjAa3g9k2tVrDdsyme5nByGAks-30Ec13M8/edit

You can also read the tech spec here:
https://docs.google.com/document/d/1tVIB7KQcSRKNU_df8oPVR3wyBHagjv2xvqaMy-oDg18/edit#

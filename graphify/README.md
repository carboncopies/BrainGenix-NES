# Graphify

This folder marks the repository's Graphify integration point. It is primarily here for AI agents so they know where to look for repository knowledge graph context and how that graph is generated.

Run `./Tools/Setup.sh` to install Graphify into the repo virtual environment, then run `./Tools/Tag.sh` to generate or update the knowledge graph before creating a release tag.

Generated Graphify output is written to `graphify-out/` and can be tracked by git when those artifacts should be shared with other agents.

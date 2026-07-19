# Graphify

This folder marks the repository's Graphify integration point. It is primarily here for AI agents so they know where to look for repository knowledge graph context and how that graph is generated.

Graphify generation is currently disabled in `./Tools/Tag.sh`. The tagger only creates the release tag and pushes it to origin.

Generated Graphify output is written to `graphify-out/` and can be tracked by git when those artifacts should be shared with other agents.

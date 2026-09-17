#pragma once

#include <string>
#include <system_error>

namespace BG {
namespace NES {
namespace Util {
namespace Storage {

void SetOutputBasePath(const std::string& OutputBasePath);
std::string GetOutputBasePath();

std::string SanitizeUsername(const std::string& Username);
std::string UserRoot(const std::string& Username);

// Resolve a CWD-relative handle (e.g. "Renders/...", "NetmorphOutput/...")
// under /var/lib/BrainGenix/NES/<username>/.
std::string Resolve(const std::string& Username, const std::string& RelativePath);

// Prefer Username, then scan sibling user directories under the output base.
// Used for unauthenticated Dataset/GetImage fetches where only the handle is known.
std::string FindExisting(const std::string& RelativePath, const std::string& PreferredUsername = "");

// Create <OutputBase>/<sanitized-username>/ with world rwx (0777) so any OS
// user can access shared outputs even when NES runs as another account.
bool EnsureUserRoot(const std::string& Username);

// Force world rwx (0777) on an already-existing path. For writers that create
// their own directories (e.g. the Igneous mesh pipeline) and would otherwise
// leave them at the process umask, which is not world-writable.
void ApplyWorldRwx(const std::string& Path);

// EnsureUserRoot, then create RelativePath under that user root (also 0777).
// Returns the absolute path on success, or empty string on mkdir failure.
std::string CreateDirectories(const std::string& Username, const std::string& RelativePath);
std::string CreateDirectories(const std::string& Username, const std::string& RelativePath, std::error_code& Error);

}; // namespace Storage
}; // namespace Util
}; // namespace NES
}; // namespace BG

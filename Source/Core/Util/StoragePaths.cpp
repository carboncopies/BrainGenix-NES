#include <Util/StoragePaths.h>

#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace BG {
namespace NES {
namespace Util {
namespace Storage {

namespace {
std::string OutputBasePath_ = "/var/lib/BrainGenix/NES";
constexpr std::filesystem::perms kWorldRwx =
    std::filesystem::perms::owner_all | std::filesystem::perms::group_all | std::filesystem::perms::others_all;

bool LooksAbsolute(const std::string& Path) {
    if (Path.empty()) {
        return false;
    }
    std::filesystem::path Parsed(Path);
    return Parsed.is_absolute();
}

std::string StripDotSlash(std::string Path) {
    while (Path.rfind("./", 0) == 0) {
        Path = Path.substr(2);
    }
    return Path;
}

// Shared lab storage: any OS user can read/write, independent of NES process owner.
void MakeWorldWritable(const std::filesystem::path& Path) {
    std::error_code Error;
    std::filesystem::permissions(Path, kWorldRwx, std::filesystem::perm_options::replace, Error);
    if (Error) {
        std::cerr << "[Storage] Warning: failed to set world rwx on '" << Path.string()
                  << "': " << Error.message() << "\n";
    }
#if defined(__APPLE__)
    // Drop any leftover per-user ACLs from the previous access model.
    std::string ClearAcl = "chmod -N '" + Path.string() + "' >/dev/null 2>&1";
    (void)std::system(ClearAcl.c_str());
#endif
}
} // namespace

void SetOutputBasePath(const std::string& OutputBasePath) {
    if (!OutputBasePath.empty()) {
        OutputBasePath_ = OutputBasePath;
    }
    std::error_code Error;
    std::filesystem::create_directories(OutputBasePath_, Error);
    MakeWorldWritable(OutputBasePath_);
}

void ApplyWorldRwx(const std::string& Path) {
    if (Path.empty()) {
        return;
    }

    // Only ever relax permissions inside the shared output root. Callers are
    // generic directory helpers that are also used for paths outside it, and
    // world-writing e.g. something under a user's home would be wrong.
    std::error_code Error;
    std::filesystem::path Canonical = std::filesystem::weakly_canonical(Path, Error);
    if (Error) {
        return;
    }
    std::filesystem::path Base = std::filesystem::weakly_canonical(OutputBasePath_, Error);
    if (Error) {
        return;
    }

    auto BaseIt = Base.begin();
    auto PathIt = Canonical.begin();
    for (; BaseIt != Base.end(); ++BaseIt, ++PathIt) {
        if (PathIt == Canonical.end() || *PathIt != *BaseIt) {
            return; // Not under the output base; leave permissions alone.
        }
    }

    // create_directories() also creates intermediate components, and those
    // inherit the umask too. Walk up to the output base so every level the
    // caller may have just created is world-writable, not only the leaf.
    std::filesystem::path Current = Canonical;
    while (!Current.empty() && Current != Base) {
        MakeWorldWritable(Current);
        if (!Current.has_parent_path() || Current.parent_path() == Current) {
            break;
        }
        Current = Current.parent_path();
    }
}

std::string GetOutputBasePath() {
    return OutputBasePath_;
}

std::string SanitizeUsername(const std::string& Username) {
    std::string Sanitized;
    for (char Character : Username) {
        unsigned char UnsignedCharacter = static_cast<unsigned char>(Character);
        if (std::isalnum(UnsignedCharacter) || Character == '_' || Character == '-' || Character == '.') {
            Sanitized += Character;
        }
    }
    return Sanitized.empty() ? "anonymous" : Sanitized;
}

std::string UserRoot(const std::string& Username) {
    return (std::filesystem::path(OutputBasePath_) / SanitizeUsername(Username)).string();
}

std::string Resolve(const std::string& Username, const std::string& RelativePath) {
    if (RelativePath.empty()) {
        return UserRoot(Username);
    }
    if (LooksAbsolute(RelativePath)) {
        return RelativePath;
    }
    return (std::filesystem::path(UserRoot(Username)) / StripDotSlash(RelativePath)).string();
}

std::string FindExisting(const std::string& RelativePath, const std::string& PreferredUsername) {
    if (RelativePath.empty()) {
        return "";
    }
    if (LooksAbsolute(RelativePath)) {
        return std::filesystem::exists(RelativePath) ? RelativePath : "";
    }

    std::string CleanRelative = StripDotSlash(RelativePath);
    if (!PreferredUsername.empty()) {
        std::string Candidate = Resolve(PreferredUsername, CleanRelative);
        if (std::filesystem::exists(Candidate)) {
            return Candidate;
        }
    }

    std::error_code Error;
    if (!std::filesystem::exists(OutputBasePath_, Error) || !std::filesystem::is_directory(OutputBasePath_, Error)) {
        // Fall back to legacy CWD-relative behavior during migration.
        if (std::filesystem::exists(CleanRelative)) {
            return CleanRelative;
        }
        return "";
    }

    for (const auto& Entry : std::filesystem::directory_iterator(OutputBasePath_, Error)) {
        if (Error || !Entry.is_directory()) {
            continue;
        }
        std::string Candidate = (Entry.path() / CleanRelative).string();
        if (std::filesystem::exists(Candidate)) {
            return Candidate;
        }
        // Also try common compressed/alternate suffixes used by GetImage.
        if (std::filesystem::exists(Candidate + ".gz")) {
            return Candidate;
        }
        if (std::filesystem::exists(Candidate + ".jpg")) {
            return Candidate;
        }
    }

    if (std::filesystem::exists(CleanRelative)) {
        return CleanRelative;
    }
    return "";
}

bool EnsureUserRoot(const std::string& Username) {
    std::string Root = UserRoot(Username);

    std::error_code Error;
    std::filesystem::create_directories(OutputBasePath_, Error);
    MakeWorldWritable(OutputBasePath_);

    Error.clear();
    std::filesystem::create_directories(Root, Error);
    if (Error && !std::filesystem::is_directory(Root)) {
        std::cerr << "[Storage] Warning: failed to create user root '" << Root
                  << "': " << Error.message() << "\n";
        return false;
    }
    MakeWorldWritable(Root);
    return true;
}

std::string CreateDirectories(const std::string& Username, const std::string& RelativePath, std::error_code& Error) {
    Error.clear();
    EnsureUserRoot(Username);

    std::string AbsolutePath = Resolve(Username, RelativePath);
    if (AbsolutePath.empty()) {
        Error = std::make_error_code(std::errc::invalid_argument);
        return "";
    }

    std::filesystem::create_directories(AbsolutePath, Error);
    if (Error && !std::filesystem::is_directory(AbsolutePath)) {
        return "";
    }
    Error.clear();

    // Ensure every newly created path component under the user root is world-writable.
    std::filesystem::path Current = AbsolutePath;
    std::filesystem::path Root = UserRoot(Username);
    while (!Current.empty()) {
        MakeWorldWritable(Current);
        if (Current == Root || Current == OutputBasePath_) {
            break;
        }
        if (!Current.has_parent_path() || Current.parent_path() == Current) {
            break;
        }
        Current = Current.parent_path();
    }

    return AbsolutePath;
}

std::string CreateDirectories(const std::string& Username, const std::string& RelativePath) {
    std::error_code Error;
    return CreateDirectories(Username, RelativePath, Error);
}

}; // namespace Storage
}; // namespace Util
}; // namespace NES
}; // namespace BG

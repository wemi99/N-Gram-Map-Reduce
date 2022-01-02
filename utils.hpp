#include <filesystem>
#include <functional>
#include <vector>

namespace utils {
std::vector<std::filesystem::path> find_all_files(
        const std::filesystem::path& dir, std::function<bool(const std::string&)> pred);


}  // namespace utils

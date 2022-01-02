#include "utils.hpp"

#include <list>

std::vector<std::filesystem::path> utils::find_all_files(
        const std::filesystem::path& dir, std::function<bool(const std::string&)> pred) {
    std::list<std::filesystem::path> files_to_sweep;
    // iterate recursively to find all files that satisfy pred
    for(auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if(std::filesystem::is_regular_file(entry)) {
            std::filesystem::path cur_file = entry.path();
            std::string type(cur_file.extension());
            if(pred(type)) {
	        files_to_sweep.push_back(std::move(cur_file));
            }
        }
    }
    return std::vector<std::filesystem::path>(std::make_move_iterator(files_to_sweep.begin()),
				 std::make_move_iterator(files_to_sweep.end()));
}

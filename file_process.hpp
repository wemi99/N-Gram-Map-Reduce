#pragma once
#include "utils.hpp"
// #include <experimental/filesystem>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

// namespace fs = std::experimental::filesystem;
namespace fs = std::filesystem;


namespace fp {
class fileProcess {
    // main storage structure for word frequencies
    std::map<std::string, uint64_t> freq;
    std::vector<std::vector<std::string>> files_string_vec;

    std::filesystem::path dir;
    uint32_t num_threads;

public:
    fileProcess(const std::string& dir, uint32_t num_threads);
    void process_file(fs::path& file, std::vector<std::vector<std::string>>& files_string_vec, int counter);
    std::map<std::string, uint64_t> local_freq;
    void display();
};
}  // namespace wc

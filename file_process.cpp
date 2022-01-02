#include "file_process.hpp"
#include "map.hpp"

#include <algorithm>
#include <cctype>
#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>
#include <thread>

#include "utils.hpp"

namespace fp{
fp::fileProcess::fileProcess(const std::string& dir, uint32_t num_threads)
        : dir(dir),
          num_threads(num_threads) {
              
}

void fp::fileProcess::display() {
    using pair_t = std::pair<std::string, uint64_t>;
    std::vector<pair_t> freq_vec(freq.size());
    uint32_t index = 0;
    for(auto [word, cnt] : freq) {
        freq_vec[index++] = {word, cnt};
    }
    std::sort(freq_vec.begin(), freq_vec.end(), [](const pair_t& p1, const pair_t& p2) {
        // decreasing order, of course
        return p1.second > p2.second || (p1.second == p2.second && p1.first < p2.first);
    });

    for(auto [word, cnt] : freq_vec) {
        std::cout << word << ": " << cnt << std::endl;
    }
}

void fp::fileProcess::process_file(fs::path& file, std::vector<std::vector<std::string>>& files_string_vec, int counter) {
    // read the entire file and update local_freq

    std::vector<std::string> file_string;

    std::ifstream fin(file);

    std::string line;
    std::stringstream dosString;

    std::stringstream buffer;
    buffer << fin.rdbuf();
    std::string contents = buffer.str();
    std::string new_contents = "";
    std::transform(contents.begin(), contents.end(), contents.begin(),
                                [](unsigned char c) -> unsigned char { return std::tolower(c); });

    // // splits on punctation
    std::regex rgx("[,.?:;!0123456789]+");
    std::sregex_token_iterator iter(contents.begin(), contents.end(), rgx, -1);
    std::sregex_token_iterator end;

    for(; iter != end; ++iter) {
        // splits on space
        std::regex rgx1("[ \n\t]+");
        std::string my_iter = (*iter);
        std::sregex_token_iterator iter1(my_iter.begin(), my_iter.end(), rgx1, -1);
        std::sregex_token_iterator end;
        for (; iter1 != end; ++iter1){

            if(*iter1 != "") {
                files_string_vec[counter].push_back(*iter1);
            }
        }
        files_string_vec[counter].push_back("!");
    }

};




}
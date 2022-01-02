#pragma once

#include "file_process.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <thread>
#include <unordered_map>
#include <bits/stdc++.h>
#include <algorithm>
#include <iterator>
#include <future>
#include <functional>
#include <mutex>
#include <map>
#include <set>

namespace mr{
class Map{

    public:

        std::vector<std::vector<std::string>> file_string_vec;
        int ngram_value;
        int total_thread_count;
        std::vector<std::vector<std::map<std::string,int>>>& thread_mailbox;

        Map(std::vector<std::vector<std::string>> file_string_vec, 
            int ngram_value, 
            int total_thread_count,
            std::vector<std::vector<std::map<std::string,int>>>& thread_mailbox);

        int compute_ngrams(std::vector<std::string> one_file_vec);

        std::vector<std::vector<std::string>> files_responsible_for(int thread_id);

        std::vector<std::string> create_local_ngrams(std::vector<std::vector<std::string>> files_responsible_for);

        int ngram_in_global_ngrams(std::vector<std::vector<std::string>> global_ngram_vector, std::vector<std::string> ngram);

        // create unordered map of ngram pairs
        std::map<std::string, int> create_ngram_count_map(int thread_id);

        std::string vec_to_string(std::vector<std::string> vec_string);

        void mapping();

        std::map<std::string,int> combine_thread_mailbox(std::vector<std::vector<std::map<std::string,int>>>& thread_mailbox, int thread_id);

};
}

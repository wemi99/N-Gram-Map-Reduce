#include "map.hpp"
#include "file_process.hpp"

#include <unordered_map>
#include <map>
#include <iostream>
#include <string.h>
#include <thread>
#include <filesystem>
#include <experimental/filesystem>

using namespace fp;
using namespace mr;

std::vector<std::vector<std::map<std::string,int>>> create_thread_mailbox(int workers){
    std::vector<std::vector<std::map<std::string,int>>> thread_mailbox;
    for (int i=0;i<workers;i++){
        std::vector<std::map<std::string,int>> temp_map_vec(workers);
        thread_mailbox.push_back(temp_map_vec);
    }
    return thread_mailbox;

}

int main(const int argc, const char** argv){
    // parsing arguments
    int ngram_value = -1;
    int workers_count = -1;
    for(int arg = 1; arg < argc; arg++) {
        if(argv[arg][0] != '-') {
            return -1;
        }
        switch(argv[arg][1]){
            case 't': {
                workers_count = atoi(argv[arg]+3);
                if (workers_count==0){
                    std::cout<<std::endl;
                    std::cout<<"No workers to process - worker value is 0 or invalid."<<std::endl;
                    std::cout<<std::endl;
                    return 0;
                }
                break;
            }
            case 'n': {
                ngram_value = atoi(argv[arg]+3);
                if (ngram_value==0){
                    std::cout<<std::endl;
                    std::cout<<"No ngrams to process - ngram value is 0 or invalid."<<std::endl;
                    std::cout<<std::endl;
                    return 0;
                }
                break;
            }
        }
    }

    if (ngram_value==-1 || workers_count==-1){
        std::cout << "error with input - try again" << std::endl;
        return -1;
    }

    fileProcess my_file_process("papers", workers_count);

    fs::path p_path = "papers";

    std::vector<fs::path> files_to_sweep = utils::find_all_files(p_path, [](const std::string& extension) {
        return extension == ".txt";
    });
    std::vector<std::vector<std::string>> files_string_vec(files_to_sweep.size());
    int counter =0;
    for (auto & fts:files_to_sweep){
        my_file_process.process_file(fts, files_string_vec, counter);
        counter ++;
    }

    typedef std::vector<std::vector<std::map<std::string,int>>> thread_mailbox_obj;
    thread_mailbox_obj thread_mailbox = create_thread_mailbox(workers_count);
    Map my_map(files_string_vec, ngram_value, workers_count, thread_mailbox);

    my_map.mapping();
    return 0;

}
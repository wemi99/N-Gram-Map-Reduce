#include "map.hpp"

namespace mr{
std::mutex mtx;
Map::Map(std::vector<std::vector<std::string>> file_string_vec,
         int ngram_value,
         int total_thread_count,
         std::vector<std::vector<std::map<std::string,int>>>& thread_mailbox) : thread_mailbox(thread_mailbox){

    this->file_string_vec = file_string_vec;
    this->ngram_value = ngram_value;
    this->total_thread_count = total_thread_count;

}

int Map::compute_ngrams(std::vector<std::string> one_file_vec){
    std::set<std::string> forbidden_set = {",", ".", "!", ";", "?",":"};

    int ngram_count = 0;
    for (int i=ngram_value-1;i<int(one_file_vec.size());i++){
        bool isNgram = false;
        for (int j=ngram_value-1;j>=0;j--){
            if (forbidden_set.find(one_file_vec[j])==forbidden_set.end()){
                isNgram = true;
            }
        }
        if (isNgram){
            ngram_count ++;
        }
    }
    return ngram_count;
}

std::vector<std::vector<std::string>> Map::files_responsible_for(int thread_id){
    std::vector<int> files_indices_thread_responsible = {};
    for (int i=0;i<int(this->file_string_vec.size());i++){
        if (i % total_thread_count == thread_id){
            files_indices_thread_responsible.push_back(i);
        }
    }
    std::vector<std::vector<std::string>> files_responsible_for;
    for (int i=0;i<int(files_indices_thread_responsible.size());i++){
        files_responsible_for.push_back(this->file_string_vec[i]);
    }
    return files_responsible_for;
}

// TO DO: remove once you can confirm you don't need these
std::string Map::vec_to_string(std::vector<std::string> vec_string){
    std::string string_from_vec = "";
    std::reverse(vec_string.begin(), vec_string.end());
    for (int i=0;i<int(vec_string.size());i++){
        if (i==(int(vec_string.size())-1)){
            string_from_vec = string_from_vec + vec_string[i];
            continue;
        };
        string_from_vec = string_from_vec + vec_string[i] + " ";
    }
    return string_from_vec;
}

std::vector<std::string> Map::create_local_ngrams(std::vector<std::vector<std::string>> files_responsible_for){
    std::set<std::string> forbidden_set = {":",",", ".", "!", ";", "?"};
    std::vector<std::string> return_vec = {};

    for (int i=0;i<int(files_responsible_for.size());i++){
        for (int j=this->ngram_value-1;j<int(files_responsible_for[i].size());j++){
            bool isValid = true;
            std::vector<std::string> temp_vec = {};
            for (int h=j;h>=j-ngram_value+1;h--){
                if ((forbidden_set.find(files_responsible_for[i][h])==forbidden_set.end())){
                    temp_vec.push_back(files_responsible_for[i][h]);
                    
                } else{
                    isValid = isValid && false;
                }
            }
            if (isValid){
                return_vec.push_back(vec_to_string(temp_vec));
            }
        }
    }
    std::cout<<std::endl;
    return return_vec;
}

std::map<std::string, int> Map::create_ngram_count_map(int thread_id){
    // create map
    std::map<std::string,int> ngram_count_map;

    // declare files responsible for
    std::vector<std::vector<std::string>> files_responsible_for_vec = mr::Map::files_responsible_for(thread_id);

    // local ngrams from files responsible for
    std::vector<std::string> local_ngrams = mr::Map::create_local_ngrams(files_responsible_for_vec);

    for (auto & ln:local_ngrams){
        if (ngram_count_map.find(ln)!=ngram_count_map.end()){
            ngram_count_map[ln] +=1;
        } else{
            ngram_count_map[ln] = 1;
        }
    }
    return ngram_count_map;
}

template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

std::multimap<int,std::string, std::greater<int>> flip_map(const std::map<std::string,int> &src)
{
    std::multimap<int,std::string, std::greater<int>> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
                   flip_pair<std::string,int>);
    return dst;
}

void print_top_five(std::vector<std::map<std::string,int>> ngram_count_map, int ngram_value){
    std::cout<<std::endl;
    std::cout<<"PRINTING THE FIVE MOST POPULAR "<<ngram_value<<"-GRAMS FOR EACH THREAD"<<std::endl;
    for (int n=0;n<int(ngram_count_map.size());n++){
        std::multimap<int,std::string, std::greater<int>> flipped = flip_map(ngram_count_map[n]);
        std::cout<<std::endl;

        std::cout<<"Thread "<<n<<"'s five most popular ngrams..." <<std::endl;
        int counter = 0;
        for (auto & [value,key]:flipped){
            if (counter<5){
                std::cout<<key<<": "<<value<<std::endl;
                counter ++;
            } else{
                break;
            }
        }
        std::cout<<std::endl;
    }
    
}

std::map<std::string,int> Map::combine_thread_mailbox(std::vector<std::vector<std::map<std::string,int>>>& thread_mailbox, int thread_id){
    std::map<std::string,int> combined_map;
    for (int i=0;i<int(thread_mailbox.size());i++){
        // update
        for (auto & [key,value]:thread_mailbox[i][thread_id]){
            combined_map[key] +=value;
        }
    }
    return combined_map;
}

void Map::mapping(){
    std::vector<std::thread> workers_vec(total_thread_count);
    std::vector<std::map<std::string,int>> thread_map_results(workers_vec.size());
    std::vector<std::map<std::string,int>> thread_map_reduce(workers_vec.size());
    std::vector<std::promise<std::map<std::string,int>>> my_promises(workers_vec.size());
    std::vector<std::future<std::map<std::string,int>>> my_futures(workers_vec.size());
    for(int j = 0; j < int(workers_vec.size()); j++) {
        my_futures[j] = my_promises[j].get_future();
    }

// *****************************


    const auto thread_shuffle_map = [&](int thread_id){
            // MAPPING
            std::map<std::string, int> my_map = mr::Map::create_ngram_count_map(thread_id);
            my_promises[thread_id].set_value(my_map);
            std::map<std::string,int> thread_map_get = my_futures[thread_id].get();
            thread_map_results[thread_id] = thread_map_get;

            // SHUFFLING
            for (auto & [key, value]:thread_map_results[thread_id]){
                std::hash<std::string> my_hash;                
                uint64_t hashed_ngram = my_hash(key);
                uint64_t receiving = hashed_ngram % total_thread_count;

                // send this ngram to the appropriate thread
                std::unique_lock<std::mutex> lock1(mtx);
                thread_mailbox[thread_id][receiving][key] = value;
            }

            // REDUCING
             for (int i=0;i<int(thread_mailbox.size());i++){
                // update
                for (auto & [key,value]:thread_mailbox[i][thread_id]){
                    std::unique_lock<std::mutex> lock2(mtx);
                    thread_map_reduce[thread_id][key] += value;
                }
            }
            };


    for (int th=0;th<int(workers_vec.size());th++){
        workers_vec[th] = std::thread(thread_shuffle_map, th);
    }

    auto wait =0;
    while (wait<int(workers_vec.size())){
        workers_vec[wait].join();
        wait++;
    }

    print_top_five(thread_map_reduce, this->ngram_value);
}
}

#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <math.h>
#include <time.h>
#include <vector>
#include <mutex>
#include <chrono>

#define MAX_THREADS 16
#define MAX_POINTS 2000000000
#define MIN_POINTS MAX_THREADS
#define CHUNK_SIZE 100000000

std::mutex sumMutex;

class BadArgException : std::exception{
    public:
        virtual const char* what() const noexcept{
            return "usage: monte [1...16] [16...2,000,000,000]";
        }
}badarg;

void monte(std::vector<std::pair<double,double>> *nums, int startIndex, int endIndex, int &sum, int threadNum){
    int localSum = 0;
    for(int i = startIndex; i < endIndex; i++){
        double result = sqrt(pow(nums->at(i).first,2) + pow(nums->at(i).second,2));
        if(result <= 1){
            localSum++;
        }
    }
    std::lock_guard<std::mutex> lock(sumMutex);
    sum += localSum;
}

void generateNums(std::vector<std::pair<double,double>> *nums,int points){
    srand(time(NULL));
    for(int i = 0; i < points; i++){
        double x = (double)rand()/RAND_MAX;
        double y = (double)rand()/RAND_MAX;
        nums->at(i) = std::make_pair(x,y);
    }
}

void generateNums(std::vector<std::pair<double,double>> *nums, int startIndex, int endIndex){
    srand(time(NULL)*endIndex);
    for(int i = startIndex; i < endIndex; i++){
        double x = (double)rand()/RAND_MAX;
        double y = (double)rand()/RAND_MAX;
        nums->at(i) = std::make_pair(x,y);
    }
}

void generateNumsP(std::vector<std::pair<double,double>> *nums, int threads, int points){
   
    int slice = points/threads;
    int rem = points%threads;
    int localThreads = threads;

    if(rem != 0){
        localThreads++;
    }

    int startIndex = 0;
    std::thread *localPool = new std::thread[localThreads]; 
    void (*pFunc)(std::vector<std::pair<double,double>>*,int,int) = generateNums;

    for(int i = 0; i < localThreads ; i++){
        int endIndex = startIndex + slice;
        if(i == localThreads-1 && rem != 0){
            endIndex = startIndex + rem;
        }
        localPool[i] = std::thread(pFunc, nums, startIndex, endIndex);
        startIndex += slice;
    }

    for(int i = 0; i < localThreads; i++){
        if(localPool[i].joinable()){
            localPool[i].join();
        }
    }

    delete[] localPool;
}

int main(int argc,char** argv){

    unsigned short threads;
    unsigned long points;

    try{
        if(argc != 3){
            throw badarg;
        }

        threads = std::stoi(argv[1]);
        points = std::stoi(argv[2]);

        if(threads > MAX_THREADS || points > MAX_POINTS || points < MIN_POINTS ){
            throw badarg;
        }
    }catch(BadArgException e){
        std::cerr << e.what() << std::endl;
        return 1;
    }

    int chunkCount = points/CHUNK_SIZE;
    int chunkRem = points%CHUNK_SIZE;

    if(chunkRem != 0 ){
        chunkCount++; 
    }
    unsigned int cores = std::thread::hardware_concurrency();

    std::cout << "\nCHUNK SIZE: " << CHUNK_SIZE << std::endl
            << "CHUNK COUNT: " << chunkCount << std::endl
            << "NUMBER OF THREADS: " << threads << std::endl
            << "NUMBER OF POINTS: " << points << std::endl
            << "NUMBER OF CORES: " << cores << std::endl << std::endl;
            
    int sum = 0;
    auto start = std::chrono::system_clock::now();

    for(int i = 0; i < chunkCount; i++){

        auto chunkStart = std::chrono::system_clock::now();
        int localPoints;
        if(i == chunkCount-1 && chunkRem){
            localPoints = chunkRem;
        }else{
            localPoints = CHUNK_SIZE;
        }
        
        std::vector<std::pair<double,double>> *nums = new std::vector<std::pair<double,double>>(localPoints);
        generateNumsP(nums, threads, localPoints);
        int rem = localPoints%threads;
        int slice = localPoints/threads;

        if(rem != 0){
            threads++;
        }

        std::thread *pool = new std::thread[threads];
        int startIndex = 0;

        for( int i = 0; i < threads; i++){
            int endIndex = startIndex+slice;
            if(i == threads-1 && rem != 0){
                endIndex = startIndex + rem;
            }
            pool[i] = std::thread(monte, std::ref(nums), startIndex,endIndex, std::ref(sum),i);
            startIndex += slice;
        }   

        for(int i = 0; i < threads; i++){
            if(pool[i].joinable()){
                pool[i].join();
            }
        }
        
        std::chrono::duration<double> localDur = (std::chrono::system_clock::now() - chunkStart);
        std::cout << "Sum after chunk #" << i << ": " << sum << std::endl
                    << "Time for Chunk Completion: " << localDur.count() << std::endl << std::endl;

        delete[] pool;
        delete nums;
    }

    std::chrono::duration<double> totalDur = (std::chrono::system_clock::now() - start);
    double ratio = (double)sum/points;
    double pi = ratio * 4;

    std::cout << "TOTAL INSIDE: " << sum << std::endl
     << "TOTAL POINTS: " << points << std::endl
     << "RATIO: " << ratio << std::endl
     << "PI APPROXIMATION: " << pi << std::endl
     << "TOTAL COMPLETION TIME: " << totalDur.count() << std::endl;

    return 0;
}   






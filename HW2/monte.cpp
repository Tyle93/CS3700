#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <math.h>
#include <time.h>
#include <vector>
#include <mutex>
#include <chrono>

#define MAX_THREADS 256
#define MAX_POINTS 2000000000
#define MIN_POINTS 10
#define CHUNK_SIZE 100000000

std::mutex iomutex;

class BadArgException : std::exception{
    public:
        virtual const char* what() const noexcept{
            return "usage: monte [1...256] [10...2,000,000,000]";
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
    std::lock_guard<std::mutex> lock(iomutex);
    sum += localSum;
    std::cout << "THREAD#" << threadNum << " LOCAL SUM: " << localSum << std::endl;
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
            endIndex -= (slice - rem);
        }
        localPool[i] = std::thread(pFunc, nums, startIndex, endIndex);
        startIndex += slice;
    }

    for(int i = 0; i < localThreads; i++){
        localPool[i].join();
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
    int sum = 0;
    
    for(int i = 0; i < chunkCount; i++){
        auto start = std::chrono::system_clock::now();
        int localPoints;
        if(i == chunkCount-1 && chunkRem){
            localPoints = chunkRem;
        }else{
            localPoints = CHUNK_SIZE;
        }
        std::thread *pool = new std::thread[threads];
        std::vector<std::pair<double,double>> *nums = new std::vector<std::pair<double,double>>(localPoints);
        generateNumsP(nums, threads, localPoints);

        int rem = localPoints%threads;
        int slice = localPoints/threads;
        if(rem != 0){
            threads++;
        }
        int startIndex = 0;

        for( int i = 0; i < threads; i++){
            int endIndex = startIndex+slice;
            if(i == threads-1 && rem != 0){
                endIndex -= (slice-rem);
            }
            pool[i] = std::thread(monte, std::ref(nums), startIndex,endIndex, std::ref(sum),i);
            startIndex += slice;
        }

        for(unsigned short i = 0; i < threads; i++){
            pool[i].join();
        }
        std::chrono::duration<double> dur = (std::chrono::system_clock::now() - start);
        std::cout << "Sum after chunk #" << i << ": " << sum << std::endl
                    << "Time for Chunk Completion: " << dur.count() << std::endl;

        delete[] pool;
        delete nums;
    }
    
    double ratio = (double)sum/points;
    double pi = ratio * 4;

    std::cout << "TOTAL INSIDE: " << sum << std::endl;
    std::cout << "TOTAL POINTS: " << points << std::endl;
    std::cout << "RATIO: " << ratio << std::endl;
    std::cout << "PI APPROXIMATION: " << pi << std::endl;

    return 0;
}






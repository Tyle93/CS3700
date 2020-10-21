#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <math.h>
#include <time.h>
#include <vector>
#include <mutex>

#define MAX_THREADS 256
#define MAX_POINTS 1000000000
#define MIN_POINTS 10

std::mutex tex;

class BadArgException : std::exception{
    public:
        virtual const char* what() const noexcept{
            return "usage: monte [1...256] [10...1,000,000,000]";
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
    std::lock_guard<std::mutex> lock(tex);
    sum += localSum;
    std::cout << "THREAD#" << threadNum << " LOCAL SUM: " << localSum << std::endl;
}
// Generate (X,Y) value pairs 
void generateNums(std::vector<std::pair<double,double>> *nums,int points){
    srand(time(NULL));
    for(int i = 0; i < points; i++){
        double x = (double)rand()/RAND_MAX;
        double y = (double)rand()/RAND_MAX;
        nums->at(i) = std::make_pair(x,y);
    }
}

int main(int argc,char** argv){
    int threads;
    int points;
    try{
        if(argc != 3){
            throw badarg;
        }
        threads = std::stoi(argv[1]);
        points = std::stoi(argv[2]);
        if(threads > MAX_THREADS || points > MAX_POINTS || points < MIN_POINTS){
            throw badarg;
        }
    }catch(BadArgException e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
    std::thread *pool = new std::thread[threads];
    std::vector<std::pair<double,double>> *nums = new std::vector<std::pair<double,double>>(points);
    generateNums(nums, points);
    int sum = 0;
    const int rem = points%threads;
    const int slice = points/threads;
    if(rem != 0){
        threads++;
    }

    int startIndex = 0;
    for(int i = 0; i < threads; i++){
        int endIndex = startIndex+slice;
        if(i == threads-1 && rem != 0){
            endIndex -= (slice-rem);
        }
        pool[i] = std::thread(monte, std::ref(nums), startIndex,endIndex, std::ref(sum),i);
        startIndex += slice;
    }

    for(int i = 0; i < threads; i++){
        pool[i].join();
    }
    
    double ratio = (double)sum/points;
    double pi = ratio * 4;

    std::cout << "TOTAL INSIDE: " << sum << std::endl;
    std::cout << "TOTAL POINTS: " << points << std::endl;
    std::cout << "RATIO: " << ratio << std::endl;
    std::cout << "PI APPROXIMATION: " << pi << std::endl;

    delete[] pool;
    delete nums;
    return 0;
}






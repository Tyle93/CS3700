#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <utility>
#include <vector>
#include <math.h>
#include <thread>

using namespace std;

constexpr long long SIZE= 20;   
mutex myMutex;

void sumUp(double& sum, const vector<double>& val, unsigned long long beg, unsigned long long end){
   double localSum = 0;
    for (auto it = beg; it < end; ++it){
        localSum += val[it];
    }
    lock_guard<mutex> myLock(myMutex);
    sum += localSum;
}

int main(){

  cout << endl;

  vector<double> values;
  values.reserve(SIZE);

  for ( long long i=0 ; i < SIZE ; i++)
     values.push_back((double)1/(i+1));
 
  double sum = 0;
  auto start = chrono::system_clock::now();

  const int threads = 5;
  thread t[threads];
  long long slice = SIZE/threads;
  long long rem = SIZE % threads;
  
  int startIdx=0;
  for (int i = 0; i < threads; ++i) {
    long long temp;
    if(i == threads-1){
        temp = rem;
    }else{
        temp = 0;
    }
    
    cout << "Thread[" << i << "] - slice ["
         << startIdx << ":" << (startIdx+slice-1+temp) << "]" << endl;
    t[i] = thread(sumUp, ref(sum), ref(values), startIdx, startIdx+slice+temp);
    startIdx += slice;
  }

  for (int i = 0; i < threads; ++i)
     t[i].join();

  chrono::duration<double> dur = chrono::system_clock::now() - start;
  cout << "Time for addition " << dur.count() << " seconds" << endl;
  cout << "Result: " << sum << endl;

  cout << endl;

}

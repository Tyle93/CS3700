#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <utility>
#include <vector>
#include <thread>

using namespace std;

constexpr long long SIZE= 1000000;   
mutex myMutex;

void sumUp(unsigned long long& sum, const vector<int>& val, 
   unsigned long long beg, unsigned long long end){
   long long localSum = 0;
    for (auto it= beg; it < end; ++it){
        localSum+= val[it];
    }
    lock_guard<mutex> myLock(myMutex);
    sum += localSum;
}

int main(){

  cout << endl;

  vector<int> randValues;
  randValues.reserve(SIZE);

  mt19937 engine (0);
  uniform_int_distribution<> uniformDist(1,10);
  for ( long long i=0 ; i< SIZE ; ++i)
     randValues.push_back(uniformDist(engine));
 
  unsigned long long sum= 0;
  auto start = chrono::system_clock::now();

  const int threads = 2;
  thread t[threads];
  long long slice = SIZE / threads;
  int startIdx=0;
  for (int i = 0; i < threads; ++i) {
    cout << "Thread[" << i << "] - slice ["
         << startIdx << ":" << startIdx+slice-1 << "]" << endl;
    t[i] = thread(sumUp, ref(sum), ref(randValues), startIdx, startIdx+slice-1);
    startIdx += slice;
  }

  for (int i = 0; i < threads; ++i)
     t[i].join();

  chrono::duration<double> dur= chrono::system_clock::now() - start;
  cout << "Time for addition " << dur.count() << " seconds" << endl;
  cout << "Result: " << sum << endl;

  cout << endl;

}
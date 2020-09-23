#include <math.h>
#include <utility>
#include <time.h>
#include <iostream>
#include <array>
#include <mpi.h>

const int COUNT = 250000;

std::array<std::pair<float,float>,COUNT>* generateCoords(int id){
    srand(id);
    std::array<std::pair<float,float>,COUNT> *coords = new std::array<std::pair<float,float>,COUNT>;
    for(int i = 0; i < COUNT; i++){
        float x = ((float)rand()) / ((float)RAND_MAX);
        float y = ((float)rand()) / ((float)RAND_MAX);
        coords->at(i) = std::make_pair(x,y);
    }
    return coords;
}

int main(int argc,char **argv){
    int id;
    int size;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    std::array<std::pair<float,float>,COUNT>* coordinates = generateCoords(id);
    int inside = 0;
    for(int i = 0; i < coordinates->size();i++){
        float h = sqrt(pow(coordinates->at(i).first,2) + pow(coordinates->at(i).second,2));
        if(h <= 1){
            inside++;
        }
    }
    std::cout << "Rank: " << id << " " << "Inside: " << inside << std::endl;
    int totalInside;
    MPI_Reduce(&inside,&totalInside, 1, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);    
    if(id == 0){

        float sum = (float)totalInside/(float)(COUNT*size);
        float pi = sum * 4;
        std::cout << "Rank: " << id << "\t" << "PI: " << pi << std::endl;
        delete coordinates;
    }
    MPI_Finalize(); 
    return 0;
}
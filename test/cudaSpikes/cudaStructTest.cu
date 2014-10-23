/*
    cudaStructTest
    testing/optimizing how to access/manipulate/return
    structures in cuda.
*/

#include <stdio.h>
#include <unistd.h>

#include "book.h"

#define N 10
/*#define BLOCKS 2
#define THREADS 5*/

typedef struct{
    int id;
    int age;
    int height;
} Person;


// Declare the Cuda kernels and any Cuda functions
__global__ void analyze_id(Person *people, int *statResults)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;

    if(id < N)
    {
        Person person = people[id];

        if(person.id < 6)
        {
            statResults[id] = 1;
        }
        else
        {
            statResults[id] = 0;
        }
    }
    
}

__global__ void analyze_age(Person *people, int *statResults)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;

    if(id < N)
    {
        Person person = people[id];

        if(person.age != 28)
        {
            statResults[id] = 1;
        }
        else
        {
            statResults[id] = 0;
        }
    }

}

__global__ void analyze_height(Person *people, int *statResults)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;

    if(id < N)
    {
        Person person = people[id];

        if(person.height != 6)
        {
            statResults[id] = 1;
        }
        else
        {
            statResults[id] = 0;
        }
    }
    
}


int main(void)
{
    int *cudaValues;
    cudaValues = cuda_setup(N);

    // CPU variables
    Person *people;
    int idStats[N];
    int ageStats[N];
    int heightStats[N];

    // GPU variables
    Person *dev_people;
    int *dev_idStats;
    int *dev_ageStats;
    int *dev_heightStats;

    // creates zero-copy memory for buffer (both CPU and GPU point to same memory). A pointer will be given to the GPU later...
    HANDLE_ERROR( cudaHostAlloc( (void **) &people, N * sizeof(Person), cudaHostAllocWriteCombined | cudaHostAllocMapped));

    // allocating GPU memory (GPU only memory)
    HANDLE_ERROR(cudaMalloc( (void **) &dev_idStats, N * sizeof(int) ));
    HANDLE_ERROR(cudaMalloc( (void **) &dev_ageStats, N * sizeof(int) ));
    HANDLE_ERROR(cudaMalloc( (void **) &dev_heightStats, N * sizeof(int) ));

    // fill people with some...
    Person person1  = {1, 25, 6};
    Person person2  = {2, 25, 5};
    Person person3  = {3, 25, 6};
    Person person4  = {4, 26, 5};
    Person person5  = {5, 26, 6};
    Person person6  = {6, 26, 5};
    Person person7  = {7, 27, 6};
    Person person8  = {8, 27, 5};
    Person person9  = {9, 27, 6};
    Person person10 = {10, 28, 5};

    people[0] = person1;
    people[1] = person2;
    people[2] = person3;
    people[3] = person4;
    people[4] = person5;
    people[5] = person6;
    people[6] = person7;
    people[7] = person8;
    people[8] = person9;
    people[9] = person10;

    // check data in people
    int x;
    /*for(x = 0; x < N; x++)
    {
        printf("id = %d\n", people[x].id);
        printf("age = %d\n", people[x].age);
        printf("height = %d\n", people[x].height);
        printf("\n");
    }*/

    // gives a pointer to the GPU to reference the zero-copy memory
    HANDLE_ERROR(cudaHostGetDevicePointer(&dev_people, people, 0));

    // calls to Cuda kernels
    analyze_id<<<BLOCKS, THREADS>>>(dev_people, dev_idStats);
    analyze_age<<<BLOCKS, THREADS>>>(dev_people, dev_ageStats);
    analyze_height<<<BLOCKS, THREADS>>>(dev_people, dev_heightStats);

    // Get the results from the GPU
    HANDLE_ERROR(cudaMemcpy(idStats, dev_idStats, N * sizeof(int), cudaMemcpyDeviceToHost));
    HANDLE_ERROR(cudaMemcpy(ageStats, dev_ageStats, N * sizeof(int), cudaMemcpyDeviceToHost));
    HANDLE_ERROR(cudaMemcpy(heightStats, dev_heightStats, N * sizeof(int), cudaMemcpyDeviceToHost));

    // make sure everyone is done
    HANDLE_ERROR(cudaThreadSynchronize());

    // cuda cleanup
    cudaFree(dev_people);
    cudaFree(dev_idStats);
    cudaFree(dev_ageStats);
    cudaFree(dev_heightStats);

    // total results
    int idTotal = 0;
    int ageTotal = 0;
    int heightTotal = 0;

    // id totals
    for(x = 0; x < N; x++)
    {
        idTotal += idStats[x];
    }

    // age totals
    for(x = 0; x < N; x++)
    {
        ageTotal += ageStats[x];
    }

    // height totals
    for(x = 0; x < N; x++)
    {
        heightTotal += heightStats[x];
    }

    printf("\n");
    printf("ID total: %d\n", idTotal);
    printf("Age total: %d\n", ageTotal);
    printf("Height total: %d\n", heightTotal);
    printf("\n");

    printf("End of cuda struct test\n");


    return 0;


}

// checks system for cuda compatible devices;
// makes sure the cuda devices have the necessary compute capabilities;
// and 
int * cuda_setup(int N)
{

}


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


typedef struct {
    int *array;
    int left;
    int right;
} Job;

// function prototypes
void merge(int *array, int left, int middle, int right);
long sequential_sort(int *array, int size);
void merge_sort(int *array, int left, int right);
void *thread_task(void *data);
long parallel_sort(int *array, int size, int numthreads);


void merge(int *array, int left, int middle, int right) {
    int *temp = malloc((right - left + 1) * sizeof(int));
    int idx = 0;
    int i = left, j = middle + 1;

    while (i <= middle && j <= right) {
        if (array[i] <= array[j]) {
            temp[idx++] = array[i++];
        } else {
            temp[idx++] = array[j++];
        }
    }

    while (i <= middle) {
        temp[idx++] = array[i++];
    }
    while (j <= right) {
        temp[idx++] = array[j++];
    }

    for (int k = 0; k < right - left + 1; k++) {
        array[left + k] = temp[k];
    }
    free(temp);
}

long sequential_sort(int *array, int size) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Perform the sorting
    merge_sort(array, 0, size - 1);

    clock_gettime(CLOCK_MONOTONIC, &end);
    long duration_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);

    return duration_ns;
}

void merge_sort(int *array, int left, int right) {
    if (left < right) {
        int middle = (left + right) / 2;
        merge_sort(array, left, middle);
        merge_sort(array, middle + 1, right);
        merge(array, left, middle, right);
    }
}

void *thread_task(void *data) {
    Job *job = (Job *)data;
    merge_sort(job->array, job->left, job->right);
    return NULL;
}

long parallel_sort(int *array, int size, int numthreads) {
    pthread_t *threads = malloc(numthreads * sizeof(pthread_t));
    Job *jobs = malloc(numthreads * sizeof(Job));
    int job_size = size / numthreads;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < numthreads; i++) {
        jobs[i].array = array;
        jobs[i].left = i * job_size;
        jobs[i].right = (i + 1) * job_size - 1;
        if (i == (numthreads - 1)) {
            jobs[i].right = size - 1;
        }
        pthread_create(&threads[i], NULL, thread_task, (void *)&jobs[i]);
    }

    for (int i = 0; i < numthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 1; i < numthreads; i++) {
        merge(array, 0, jobs[i - 1].right, jobs[i].right);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    long duration_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);

    free(threads);
    free(jobs);
    return duration_ns;
}

int main() {
    int size = 10000; // Increased size for more noticeable difference
    int numthreads = 4; // Use an appropriate number of threads for parallel_sort
    int *array = malloc(size * sizeof(int));
    int *array_copy = malloc(size * sizeof(int)); // Copy of the array for sequential sort
    srand(time(NULL));

    for (int i = 0; i < size; i++) {
        array[i] = rand() % 100;
    }
    memcpy(array_copy, array, size * sizeof(int)); // Copy the original array

    // Parallel sort
    long duration_ns_parallel = parallel_sort(array, size, numthreads);
    printf("Parallel sorting took %ld nanoseconds.\n", duration_ns_parallel);

    // Sequential sort
    long duration_ns_sequential = sequential_sort(array_copy, size);
    printf("Sequential sorting took %ld nanoseconds.\n", duration_ns_sequential);

    // Free memory
    free(array);
    free(array_copy);
    return 0;
}
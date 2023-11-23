#include <stdio.h>
#include <omp.h>
#define N 4

void PrivateX()
{
    omp_set_num_threads(N);
    int x = 44;
#pragma omp parallel for private(x) num_threads(N)
    for (int i = 0; i <= 10; i++) {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n\n", x);
}

void FirstPrivateX()
{
    omp_set_num_threads(N);
    int x = 44;
#pragma omp parallel for firstprivate(x) num_threads(N)
    for (int i = 0; i <= 10; i++) {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n\n", x);
}

void LastPrivateX()
{
    omp_set_num_threads(4);
    int x = 44;
#pragma omp parallel for lastprivate(x) num_threads(N)
    for (int i = 0; i <= 10; i++) {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n\n", x);
}

int main()
{
    PrivateX();
    FirstPrivateX();
    LastPrivateX();

    return 0;

}
#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#define N 1000

using Matrix = std::vector<std::vector<int>>;
using Time = std::chrono::milliseconds;

Time MultipleSerialMatrix(const Matrix& A, const Matrix& B)
{
    Matrix C(N, std::vector<int>(N));
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return duration;
}

Time MultipleParallelMatrix(const Matrix& A, const Matrix& B)
{
    Matrix C(N, std::vector<int>(N));
    auto start = std::chrono::high_resolution_clock::now();

#pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duraiton = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return duraiton;
}

int main()
{
    Matrix A(N, std::vector<int>(N));
    Matrix B(N, std::vector<int>(N));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 100;
            B[i][j] = rand() % 100;
        }
    }

    Time durationSerial = MultipleSerialMatrix(A, B);
    Time durationParallel = MultipleParallelMatrix(A, B);


    std::cout << "Time Serial: " << durationSerial.count() << " milliseconds" << std::endl;
    std::cout << "Time Parallel: " << durationParallel.count() << " milliseconds" << std::endl;
    return 0;
}
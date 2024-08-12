//==============================================================
// Copyright © 2020 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include <sycl/sycl.hpp>
#include <iomanip>
#include <iostream>
#include <limits>

#include "dpc_common.hpp"

using namespace std;
using namespace sycl;

// Matrix size constant.
constexpr int N = 1000;

/**
 * Initialize matrices A, B, C_serial, and C_parallel.
 */
void init_matrices(float* A, float* B, float* C_serial, float* C_parallel) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = 1.0f;
            B[i * N + j] = i + 1.0f;
            C_serial[i * N + j] = 0.0f;
            C_parallel[i * N + j] = 0.0f;
        }
    }
}

/**
 * Perform serial matrix multiplication.
 */
void matrix_mul_serial(float* A, float* B, float* C_serial) {
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            for (int j = 0; j < N; j++) {
                C_serial[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

/**
 * Perform parallel matrix multiplication using SYCL.
 */
void matrix_mul_parallel(queue& q, buffer<float, 2>& A_buf, buffer<float, 2>& B_buf, buffer<float, 2>& C_buf) {
    q.submit([&](auto& h) {
        accessor A(A_buf, h, read_only);
        accessor B(B_buf, h, read_only);
        accessor C(C_buf, h, write_only);

        int width_A = A_buf.get_range()[1];

        h.parallel_for(range(N, N), [=](auto index) {
            int row = index[0];
            int col = index[1];
            float sum = 0.0f;
            for (int i = 0; i < width_A; i++) {
                sum += A[row][i] * B[i][col];
            }
            C[index] = sum;
        });
    }).wait();  // Ensure completion before returning
}

/**
 * Check if two float values are approximately the same.
 */
bool value_same(float a, float b) {
    return fabs(a - b) < numeric_limits<float>::epsilon();
}

/**
 * Verify if the two matrices are the same.
 */
bool verify_results(float* C_parallel, float* C_serial) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (!value_same(C_parallel[i * N + j], C_serial[i * N + j])) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Main function to setup and run matrix multiplication.
 */
int main() {
    float (*A)[N] = new float[N][N];
    float (*B)[N] = new float[N][N];
    float (*C_serial)[N] = new float[N][N];
    float (*C_parallel)[N] = new float[N][N];

    init_matrices(*A, *B, *C_serial, *C_parallel);

    try {
        queue q(default_selector_v);

        device dev = q.get_device();
        cout << "Device: " << dev.get_info<info::device::name>() << "\n";
        cout << "Number of compute units: " << dev.get_info<info::device::max_compute_units>() << "\n";

        buffer<float, 2> A_buf(range(N, N));
        buffer<float, 2> B_buf(range(N, N));
        buffer<float, 2> C_buf(reinterpret_cast<float*>(C_parallel), range(N, N));

        q.submit([&](auto& h) {
            accessor A_acc(A_buf, h, write_only);
            h.parallel_for(range(N, N), [=](auto index) {
                A_acc[index] = 1.0f;
            });
        }).wait();

        q.submit([&](auto& h) {
            accessor B_acc(B_buf, h, write_only);
            h.parallel_for(range(N, N), [=](auto index) {
                B_acc[index] = index[0] + 1.0f;
            });
        }).wait();

        // Timing and execution of parallel matrix multiplication
        dpc_common::TimeInterval t_par;
        matrix_mul_parallel(q, A_buf, B_buf, C_buf);
        double parallelTime = t_par.Elapsed();

        // Timing and execution of serial matrix multiplication
        dpc_common::TimeInterval t_ser;
        matrix_mul_serial(*A, *B, *C_serial);
        double serialTime = t_ser.Elapsed();

        cout << std::setw(20) << "Serial time: " << serialTime << "s\n";
        cout << std::setw(20) << "Parallel time: " << parallelTime << "s\n";

        // Verify the results
        if (verify_results(*C_parallel, *C_serial)) {
            cout << "Both versions produced correct results.\n";
        } else {
            cout << "Results mismatch!\n";
        }

        delete[] A;
        delete[] B;
        delete[] C_serial;
        delete[] C_parallel;

        return 0;
    } catch (sycl::exception const& e) {
        cout << "An exception is caught while multiplying matrices.\n";
        terminate();
    }
}

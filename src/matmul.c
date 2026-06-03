#include <pthread.h>
#include <stdlib.h>
#include "matmul.h"

/* C = A * B, matrizes n x n em row-major (A[i*n + j]) */
void multiplicar(const double *A, const double *B, double *C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            double soma = 0.0;
            for (int k = 0; k < n; k++)
                soma += A[i*n + k] * B[k*n + j];
            C[i*n + j] = soma;
        }
}

static void *worker(void *arg) {
    thread_arg *ta = (thread_arg *)arg;
    const double *A = ta->A;
    const double *B = ta->B;
    double *C = ta->C;
    int n = ta->n;

    for (int i = ta->row_start; i < ta->row_end; i++)
        for (int j = 0; j < n; j++) {
            double soma = 0.0;
            for (int k = 0; k < n; k++)
                soma += A[i*n + k] * B[k*n + j];
            C[i*n + j] = soma;
        }
    return NULL;
}

void multiplicar_paralelo(const double *A, const double *B, double *C,
                          int n, int num_threads) {
    if (num_threads < 1) num_threads = 1;
    if (num_threads > n) num_threads = n;

    pthread_t *threads = malloc((size_t)num_threads * sizeof(pthread_t));
    thread_arg *args = malloc((size_t)num_threads * sizeof(thread_arg));

    int base = n / num_threads;
    int resto = n % num_threads;

    /* divide as linhas em blocos contiguos; as primeiras 'resto' threads
       ficam com uma linha a mais para balancear */
    int row = 0;
    for (int t = 0; t < num_threads; t++) {
        int linhas = base + (t < resto ? 1 : 0);
        args[t].A = A;
        args[t].B = B;
        args[t].C = C;
        args[t].n = n;
        args[t].row_start = row;
        args[t].row_end = row + linhas;
        row += linhas;
        pthread_create(&threads[t], NULL, worker, &args[t]);
    }

    for (int t = 0; t < num_threads; t++)
        pthread_join(threads[t], NULL);

    free(threads);
    free(args);
}

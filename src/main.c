#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "matmul.h"

#define N 1200       /* dimensao das matrizes (n x n) */
#define NUM_RUNS 6   /* execucoes por medicao; descarta-se a 1a (aquecimento) */

static double tempo_segundos(struct timespec t0, struct timespec t1) {
    return (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
}

static int comparar_double(const void *a, const void *b) {
    double da = *(const double *)a;
    double db = *(const double *)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

static double mediana(double *v, int n) {
    qsort(v, n, sizeof(double), comparar_double);
    if (n % 2 == 1) return v[n / 2];
    return 0.5 * (v[n / 2 - 1] + v[n / 2]);
}

static void preencher(double *A, double *B, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            A[i*n + j] = (double)((i + 2*j) % 7) * 0.5 + 0.1;
            B[i*n + j] = (double)((3*i + j) % 5) * 0.25 + 0.2;
        }
}

static double checksum(const double *C, int n) {
    double s = 0.0;
    for (int i = 0; i < n * n; i++)
        s += C[i];
    return s;
}

int main(int argc, char **argv) {
    int n = N;
    int num_threads;

    if (argc >= 2) {
        num_threads = atoi(argv[1]);
        if (num_threads < 1) num_threads = 1;
    } else {
        long nproc = sysconf(_SC_NPROCESSORS_ONLN);
        num_threads = (nproc > 0) ? (int)nproc : 4;
        fprintf(stderr, "Aviso: numero de threads nao informado; usando %d.\n",
                num_threads);
    }

    /* alocacao e preenchimento ficam fora do cronometro */
    double *A = malloc((size_t)n * n * sizeof(double));
    double *B = malloc((size_t)n * n * sizeof(double));
    double *C_seq = malloc((size_t)n * n * sizeof(double));
    double *C_par = malloc((size_t)n * n * sizeof(double));
    if (!A || !B || !C_seq || !C_par) {
        fprintf(stderr, "Erro: falha de alocacao de memoria.\n");
        free(A); free(B); free(C_seq); free(C_par);
        return 1;
    }
    preencher(A, B, n);

    struct timespec t0, t1;
    double tempos_seq[NUM_RUNS];
    double tempos_par[NUM_RUNS];

    for (int r = 0; r < NUM_RUNS; r++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        multiplicar(A, B, C_seq, n);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        tempos_seq[r] = tempo_segundos(t0, t1);
    }
    double t_seq = mediana(&tempos_seq[1], NUM_RUNS - 1);

    for (int r = 0; r < NUM_RUNS; r++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        multiplicar_paralelo(A, B, C_par, n, num_threads);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        tempos_par[r] = tempo_segundos(t0, t1);
    }
    double t_par = mediana(&tempos_par[1], NUM_RUNS - 1);

    /* compara o resultado paralelo com a referencia sequencial */
    double max_dif = 0.0;
    for (int i = 0; i < n * n; i++) {
        double d = C_seq[i] - C_par[i];
        if (d < 0) d = -d;
        if (d > max_dif) max_dif = d;
    }
    const char *check = (max_dif < 1e-6) ? "OK" : "FALHA";

    double speedup = t_seq / t_par;

    printf("==== Multiplicacao de matrizes (P1) ====\n");
    printf("n              : %d x %d\n", n, n);
    printf("threads        : %d\n", num_threads);
    printf("T_seq (s)      : %.6f\n", t_seq);
    printf("T_par (s)      : %.6f\n", t_par);
    printf("speedup        : %.4f\n", speedup);
    printf("checksum C_seq : %.6f\n", checksum(C_seq, n));
    printf("checksum C_par : %.6f\n", checksum(C_par, n));
    printf("max |dif|      : %.3e\n", max_dif);
    printf("verificacao    : %s\n", check);

    printf("RESULT threads=%d t_seq=%.6f t_par=%.6f speedup=%.4f check=%s\n",
           num_threads, t_seq, t_par, speedup, check);

    free(A); free(B); free(C_seq); free(C_par);
    return 0;
}

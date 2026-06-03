#ifndef MATMUL_H
#define MATMUL_H

/* Dados que cada thread recebe: a faixa de linhas [row_start, row_end)
   de C que ela deve calcular. */
typedef struct {
    const double *A;
    const double *B;
    double *C;
    int n;
    int row_start;
    int row_end;
} thread_arg;

void multiplicar(const double *A, const double *B, double *C, int n);
void multiplicar_paralelo(const double *A, const double *B, double *C,
                          int n, int num_threads);

#endif

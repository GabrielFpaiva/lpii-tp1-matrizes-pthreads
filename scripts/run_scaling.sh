#!/usr/bin/env bash
# Mede o tempo para 1, 2, 4 e 8 threads e gera results.csv + o grafico.
set -euo pipefail

cd "$(dirname "$0")/.."

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

CSV="scripts/results.csv"
echo "threads,tempo,speedup,eficiencia" > "$CSV"

t_seq_ref=""  # t_seq do run com T=1, usado como referencia do speedup.

for T in 1 2 4 8; do
    linha="$(./build/matmul "$T" | grep '^RESULT')"

    # Extrai os campos da linha RESULT.
    t_seq="$(printf '%s\n' "$linha" | sed -n 's/.*t_seq=\([0-9.eE+-]*\).*/\1/p')"
    t_par="$(printf '%s\n' "$linha" | sed -n 's/.*t_par=\([0-9.eE+-]*\).*/\1/p')"

    if [ -z "$t_seq_ref" ]; then
        t_seq_ref="$t_seq"
    fi

    # speedup = t_seq(T=1) / t_par ; eficiencia = speedup / T.
    speedup="$(awk -v a="$t_seq_ref" -v b="$t_par" 'BEGIN { printf "%.4f", a / b }')"
    efic="$(awk -v s="$speedup" -v t="$T" 'BEGIN { printf "%.4f", s / t }')"

    echo "$T,$t_par,$speedup,$efic" >> "$CSV"
    echo "T=$T  tempo=$t_par  speedup=$speedup  eficiencia=$efic"
done

echo "Resultados gravados em $CSV"

# Gera o grafico; se o python3 do PATH falhar, tenta o do sistema isolado.
if command -v python3 >/dev/null 2>&1; then
    if python3 scripts/plot.py; then
        :
    elif PYTHONNOUSERSITE=1 /usr/bin/python3 scripts/plot.py; then
        :
    else
        echo "Aviso: nao consegui gerar o grafico (matplotlib indisponivel)."
    fi
else
    echo "Aviso: python3 nao encontrado; pulei a geracao do grafico."
fi

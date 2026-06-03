#!/usr/bin/env python3
"""Le scripts/results.csv e plota speedup x numero de threads, com a reta
ideal (y = x) como referencia. Salva scripts/scaling.png.
Usa apenas a stdlib + matplotlib."""

import csv
import os

import matplotlib
matplotlib.use("Agg")  # backend sem display (gera arquivo)
import matplotlib.pyplot as plt

aqui = os.path.dirname(os.path.abspath(__file__))
csv_path = os.path.join(aqui, "results.csv")
png_path = os.path.join(aqui, "scaling.png")

threads = []
speedups = []
with open(csv_path, newline="") as f:
    leitor = csv.DictReader(f)
    for linha in leitor:
        threads.append(int(linha["threads"]))
        speedups.append(float(linha["speedup"]))

fig, ax = plt.subplots()
ax.plot(threads, speedups, marker="o", label="Speedup medido")
ax.plot(threads, threads, linestyle="--", color="gray", label="Ideal (y = x)")

ax.set_xlabel("Numero de threads")
ax.set_ylabel("Speedup")
ax.set_title("Escalabilidade — Multiplicacao de matrizes (P1)")
ax.set_xticks(threads)
ax.legend()
ax.grid(True)

fig.savefig(png_path, dpi=120, bbox_inches="tight")
print("Grafico salvo em", png_path)

import matplotlib.pyplot as plt

# Dados fictícios para demonstração
threads = [1, 2, 4, 8]
log_seq = [5000, 2500, 1500, 1200]  # Tempo sequencial (log transform)
log_par = [5000, 2600, 1400, 900]   # Tempo paralelo (log transform)
median_seq = [12000, 6000, 4000, 3000]
median_par = [12000, 6200, 3500, 2200]

# Plot de tempos
plt.figure(figsize=(12, 6))
#plt.plot(threads, log_seq, 'o-', label="Log (Sequencial)")
plt.plot(threads, log_par, 'o-', label="Log (Paralelo)")
#plt.plot(threads, median_seq, 'o-', label="Mediana (Sequencial)")
plt.plot(threads, median_par, 'o-', label="Mediana (Paralelo)")
plt.xlabel("Número de Threads")
plt.ylabel("Tempo de Execução (ms)")
plt.title("Comparação de Tempo de Execução")
plt.legend()
plt.grid(True)
plt.savefig("tempo_execucao.png")

# Plot de speedup
plt.figure(figsize=(12, 6))
plt.plot(threads, [log_seq[0] / t for t in log_par], 'o-', label="Log Transform")
plt.plot(threads, [median_seq[0] / t for t in median_par], 'o-', label="Median Filter")
plt.xlabel("Número de Threads")
plt.ylabel("Speedup")
plt.title("Speedup em Função do Número de Threads")
plt.legend()
plt.grid(True)
plt.savefig("speedup.png")
plt.show()

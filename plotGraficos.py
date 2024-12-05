import matplotlib.pyplot as plt

# Dados fictícios para demonstração
threads = [1, 2, 3, 4, 5, 6]
log_seq = [38.928955, 38.317871, 38.090820, 38.413086, 38.506836, 38.506836]  # Tempo sequencial (log transform)
log_par = [43.594971, 21.826172, 14.795898, 13.716797, 9.214844, 7.514160]   # Tempo paralelo (log transform)
median_seq = [1385.477051, 1367.830078, 1377.114014, 1447.312988, 1399.380127, 1383.339111]
median_par = [1479.303223, 705.625977, 569.734863, 359.823975, 338.871094, 290.034912]

# Gráfico 1: Comparação de tempos para a transformação logarítmica
plt.figure(figsize=(10, 5))
plt.plot(threads, log_seq, label='Sequencial', linestyle='--', color='gray')
plt.plot(threads, log_par, label='Paralelo', marker='o', linestyle='-', color='b')
plt.title('Comparação de Tempos - Transformação Logarítmica')
plt.xlabel('Número de Threads (paralelo)')
plt.ylabel('Tempo de Execução (ms)')
plt.xticks(threads)
plt.grid(True)
plt.legend()
plt.savefig('comparison_log_transform.png')  # Salvar o gráfico como imagem
plt.show()

# Gráfico 2: Comparação de tempos para o filtro de mediana
plt.figure(figsize=(10, 5))
plt.plot(threads, median_seq, label='Sequencial', linestyle='--', color='gray')
plt.plot(threads, median_par, label='Paralelo', marker='o', linestyle='-', color='r')
plt.title('Comparação de Tempos - Filtro de Mediana')
plt.xlabel('Número de Threads (paralelo)')
plt.ylabel('Tempo de Execução (ms)')
plt.xticks(threads)
plt.grid(True)
plt.legend()
plt.savefig('comparison_median_filter.png')  # Salvar o gráfico como imagem
plt.show()

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

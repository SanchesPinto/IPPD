#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#define WIDTH 3840 // Largura da imagem
#define HEIGHT 2160 // Altura da imagem

double computeExpensiveOperation(int x, int y, int iterations) {
    double sum = 0.0;
    int sign = 1;

    for (int i = 0; i < iterations; i++) {
        sum += sign * 1.0 / (2 * i + 1); // Termo da série de Leibniz
        sign = -sign;                   // Alterna o sinal
    }

    return sum * 4.0; // Multiplica por 4 para aproximar pi
}

// Função para aplicar o filtro de blur em uma imagem em tons de cinza
void applyBlur(unsigned char input[HEIGHT][WIDTH], unsigned char output[HEIGHT][WIDTH]) {
    int kernel[3][3] = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };
    int kernelSum = 9;
    int expensiveIterations = 10000; // Aumente conforme necessário para maior carga

    //#pragma omp parallel for collapse(2) schedule(static)
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            int sum = 0;

            // Percorrer o kernel
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += input[y + ky][x + kx] * kernel[ky + 1][kx + 1];
                }
            }

            // Aplica a função custosa no pixel
            //double expensiveResult = computeExpensiveOperation(x, y, expensiveIterations);
            //sum += (int)expensiveResult; // Adiciona o resultado ao somatório
            //#pragma omp critical
            {
                output[y][x] = (sum / kernelSum) % 256; // Normaliza e mantém no intervalo [0, 255]
            }
        }
    }
}


// Função para gerar uma imagem de exemplo (gradiente horizontal)
void generateExampleImage(unsigned char image[HEIGHT][WIDTH])
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            image[y][x] = (unsigned char)(x % 256); // Valores de 0 a 255
        }
    }
}

// Função para salvar a imagem em formato PGM (Portable GrayMap)
void savePGM(const char *filename, unsigned char image[HEIGHT][WIDTH])
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Erro ao salvar a imagem");
        exit(1);
    }
    fprintf(file, "P5\n%d %d\n255\n", WIDTH, HEIGHT); // Cabeçalho PGM
    fwrite(image, sizeof(unsigned char), WIDTH * HEIGHT, file);
    fclose(file);
}

int main()
{
    // Imagens de entrada e saída
    unsigned char input[HEIGHT][WIDTH];
    unsigned char output[HEIGHT][WIDTH] = {0};

    // Gerar uma imagem de exemplo
    generateExampleImage(input);
    savePGM("original.pgm", input);
    struct timeval start, stop;

    gettimeofday(&start, NULL);
    // Aplicar o filtro de blur
    applyBlur(input, output);
    gettimeofday(&stop, NULL);

    double tempo =
        (((double)(stop.tv_sec) * 1000.0 + (double)(stop.tv_usec / 1000.0)) -
         ((double)(start.tv_sec) * 1000.0 + (double)(start.tv_usec / 1000.0)));

    printf("Tempo: %f ms \n", tempo);

    // Salvar a imagem original e a imagem com blur
    savePGM("blurred.pgm", output);

    printf("Filtro de blur aplicado. Imagens salvas como 'original.pgm' e 'blurred.pgm'.\n");

    return 0;
}

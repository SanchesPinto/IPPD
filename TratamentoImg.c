#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#define WIDTH 200  // Largura da imagem
#define HEIGHT 100 // Altura da imagem

// Função para aplicar o filtro de blur em uma imagem em tons de cinza
void applyBlur(unsigned char input[HEIGHT][WIDTH], unsigned char output[HEIGHT][WIDTH])
{
    // Kernel de blur 3x3
    int kernel[3][3] = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}};
    int kernelSum = 9; // Soma dos elementos do kernel

    // Aplicar o kernel na imagem

    #pragma omp parallel for schedule(dynamic) collapse(2)
    {
    
        for (int y = 1; y < HEIGHT - 1; y++)
        {
            for (int x = 1; x < WIDTH - 1; x++)
            {
                int sum = 0;
                for (int ky = -1; ky <= 1; ky++)
                {
                    for (int kx = -1; kx <= 1; kx++)
                    {
                        printf("Thread numero: %d, Aplicando kernel na posicao %d %d\n", omp_get_thread_num(), y, x);
                        sum += input[y + ky][x + kx] * kernel[ky + 1][kx + 1];
                    }
                }
                output[y][x] = sum / kernelSum;
            }
        }
        printf("Numero de threads: %d\n", omp_get_num_threads());
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
    struct timeval start, end;

    gettimeofday(&start, NULL);
    // Aplicar o filtro de blur
    applyBlur(input, output);
    gettimeofday(&end, NULL);

    float time_spent = (((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))) / 1000.0f;
    printf("[INFO] Tempo total de %f mili-seconds \n", time_spent);

    // Salvar a imagem original e a imagem com blur
    savePGM("blurred.pgm", output);

    printf("Filtro de blur aplicado. Imagens salvas como 'original.pgm' e 'blurred.pgm'.\n");
    
    return 0;
}

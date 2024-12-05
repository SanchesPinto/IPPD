#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>

#define WIDTH 1000  // Largura da imagem
#define HEIGHT 5000 // Altura da imagem

// Função para aplicar a transformação logarítmica
void apply_log_transform(unsigned char (*input)[HEIGHT][WIDTH], unsigned char (*output)[HEIGHT][WIDTH], float c, int n)
{
#pragma omp parallel num_threads(n)
#pragma omp for collapse(2)
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float new_value = c * log(1 + (*input)[y][x]);
            if (new_value > 255)
                new_value = 255;
            if (new_value < 0)
                new_value = 0;
            (*output)[y][x] = (unsigned char)new_value;
        }
    }
}

void apply_log_transform_sequential(unsigned char (*input)[HEIGHT][WIDTH], unsigned char (*output)[HEIGHT][WIDTH], float c)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            float new_value = c * log(1 + (*input)[y][x]);
            if (new_value > 255)
                new_value = 255;
            if (new_value < 0)
                new_value = 0;
            (*output)[y][x] = (unsigned char)new_value;
        }
    }
}

// Função para aplicar o filtro de mediana 3x3
void apply_median_filter(unsigned char (*input)[HEIGHT][WIDTH], unsigned char (*output)[HEIGHT][WIDTH], int n)
{
    int window[25];

#pragma omp parallel num_threads(n)
#pragma omp for private(window) collapse(2)

    for (int y = 2; y < HEIGHT - 2; y++) // Ajuste os limites para a nova janela
    {
        for (int x = 2; x < WIDTH - 2; x++) // Ajuste os limites para a nova janela
        {
            int index = 0;

            // Preencher a janela 5x5
            for (int ky = -2; ky <= 2; ky++) // Aumenta o intervalo da janela
            {
                for (int kx = -2; kx <= 2; kx++) // Aumenta o intervalo da janela
                {
                    window[index++] = (*input)[y + ky][x + kx];
                }
            }

            // Ordenar a janela e pegar a mediana
            for (int i = 0; i < 24; i++) // Modifica os limites de ordenação para a nova janela
            {
                for (int j = i + 1; j < 25; j++)
                {
                    if (window[i] > window[j])
                    {
                        int temp = window[i];
                        window[i] = window[j];
                        window[j] = temp;
                    }
                }
            }

            (*output)[y][x] = window[12]; // Mediana (13º elemento após ordenação)
        }
    }
}

void apply_median_filter_sequential(unsigned char (*input)[HEIGHT][WIDTH], unsigned char (*output)[HEIGHT][WIDTH])
{
    int window[25];

    for (int y = 2; y < HEIGHT - 2; y++)
    {
        for (int x = 2; x < WIDTH - 2; x++)
        {
            int index = 0;

            // Preencher a janela 5x5
            for (int ky = -2; ky <= 2; ky++)
            {
                for (int kx = -2; kx <= 2; kx++)
                {
                    window[index++] = (*input)[y + ky][x + kx];
                }
            }

            // Ordenar a janela e pegar a mediana
            for (int i = 0; i < 24; i++)
            {
                for (int j = i + 1; j < 25; j++)
                {
                    if (window[i] > window[j])
                    {
                        int temp = window[i];
                        window[i] = window[j];
                        window[j] = temp;
                    }
                }
            }

            (*output)[y][x] = window[12];
        }
    }
}

// Função para gerar uma imagem de exemplo (gradiente horizontal)
void generateExampleImage(unsigned char (*image)[HEIGHT][WIDTH])
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            // Criar um gradiente horizontal de esquerda para a direita
            unsigned char gradient = (unsigned char)(x % 256);

            // Adicionar um padrão de círculos para gerar variação na imagem
            int centerX = WIDTH / 2;
            int centerY = HEIGHT / 2;
            int radius = 100;
            int distX = x - centerX;
            int distY = y - centerY;
            int distance = distX * distX + distY * distY;

            // Se o pixel estiver dentro do círculo, intensificar o valor
            if (distance < radius * radius)
            {
                gradient = (unsigned char)(gradient + 50); // Aumenta o brilho na área do círculo
            }

            // Definir o valor do pixel
            (*image)[y][x] = gradient;
        }
    }
}

// Função para salvar a imagem em formato PGM (Portable GrayMap)
void savePGM(const char *filename, unsigned char (*image)[HEIGHT][WIDTH])
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
    unsigned char input[HEIGHT][WIDTH];
    unsigned char output[HEIGHT][WIDTH];

    double tempoSeq, tempoPar;
    struct timeval start, end;
    float c = 5.0;

    // Gerar a imagem de exemplo
    generateExampleImage(&input);
    savePGM("original.pgm", &input);

    int TotalDeThreads = omp_get_max_threads();

    for (int i = 1; i <= TotalDeThreads; i++)
    {
        
        printf("\n====================\nUtilizando %d thread(s)\n", i);

        // Comparar tempos da transformação logarítmica
        gettimeofday(&start, NULL);
        apply_log_transform_sequential(&input, &output, c);
        gettimeofday(&end, NULL);
        tempoSeq = (((double)(end.tv_sec) * 1000.0 + (double)(end.tv_usec / 1000.0)) -
                    ((double)(start.tv_sec) * 1000.0 + (double)(start.tv_usec / 1000.0)));
        savePGM("log_transform_sequential.pgm", &output);
        printf("[LOG] Tempo sequencial (log transform): %f ms\n", tempoSeq);

        omp_set_num_threads(i);
        apply_log_transform(&input, &output, c, i);
        apply_log_transform(&input, &output, c, i);
        gettimeofday(&end, NULL);
        tempoPar = (((double)(end.tv_sec) * 1000.0 + (double)(end.tv_usec / 1000.0)) -
                    ((double)(start.tv_sec) * 1000.0 + (double)(start.tv_usec / 1000.0)));
        savePGM("log_transform_parallel.pgm", &output);
        printf("[LOG] Tempo paralelo (log transform): %f ms\n", tempoPar);

        // Comparar tempos do filtro de mediana
        gettimeofday(&start, NULL);
        apply_median_filter_sequential(&input, &output);
        gettimeofday(&end, NULL);
        tempoSeq = (((double)(end.tv_sec) * 1000.0 + (double)(end.tv_usec / 1000.0)) -
                    ((double)(start.tv_sec) * 1000.0 + (double)(start.tv_usec / 1000.0)));
        savePGM("median_filter_sequential.pgm", &output);
        printf("[MEDIAN] Tempo sequencial (median filter): %f ms\n", tempoSeq);

        omp_set_num_threads(i);
        apply_median_filter(&input, &output, i);
        apply_median_filter(&input, &output, i);
        gettimeofday(&end, NULL);
        tempoPar = (((double)(end.tv_sec) * 1000.0 + (double)(end.tv_usec / 1000.0)) -
                    ((double)(start.tv_sec) * 1000.0 + (double)(start.tv_usec / 1000.0)));
        savePGM("median_filter_parallel.pgm", &output);
        printf("[MEDIAN] Tempo paralelo (median filter): %f ms\n", tempoPar);
    }

    return 0;
}

/**********************************************************
 * Trabalho 2 - Analise de Algoritmos
 * Docente: Daniel Carlos Guimaraes Pedronette
 **********************************************************
 *

 * Integrantes: Cauan Gabriel de Souza <cauansouza@unesp.br>
 *              Rebeca Ruivo Reis  <rebeca.ruivo@unesp.br>
 *              Gabriel Sanches Pinto  <sanches.pinto@unesp.br>
 *              Hannah Caroline Cavalcanti Santos Andrade   <hannah.cavalcanti@unesp.br>
 ***********************************************************/

/********************** Libraries **************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <libgen.h>
#include <omp.h>

#define INPUT_FILENAME_A "input-A.in"
#define INPUT_FILENAME_B "input-B.in"
#define OUTPUT_FILENAME "output.out"
#define MAX_N_SIZE 2000
#define EXECUTION_TIMES 5
#define HASH_TABLE_SIZE 10000 // Tamanho da tabela hash

/******** Solução **********/
int v_output[MAX_N_SIZE];

// Cria a estrutura da tabela hash
typedef struct HashNode
{
    int key;
    struct HashNode *next;
} HashNode;

HashNode *hashTable[HASH_TABLE_SIZE];

int hashFunction(int key)
// Retorna a posicao da chave na tabela
{
    return key % HASH_TABLE_SIZE;
}

void insertHash(int key)
// Insere a chave dentro da tabela hash
{
    int index = hashFunction(key);

    
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    newNode->key = key;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
    
}

int searchHash(int key, int numThreads, int threadId)
// Busca a chave dentro da tabela hash
{
    int index = hashFunction(key);
 /*
    int chunkSize = MAX_N_SIZE / numThreads;
    int start = threadId * chunkSize;
    int end = (threadId == numThreads - 1) ? MAX_N_SIZE : start + chunkSize;
 
    for (int i = start; i < end; i++)
    {
        for (HashNode *current = hashTable[i]; current != NULL; current = current->next)
        {
        if (current->key == key)
        {
            return 1;
        }
        }
    }
*/

/*
    HashNode *current = hashTable[index];
    while (current != NULL)
    {
        if (current->key == key)
        {
            return 1;
        }
        current = current->next;
    }
*/
    return 0;
}

void solution(int **v_input_a, int *v_input_b)
// Solucao do problema
{
    // Inicializa o vetor com o valor 0
    memset(hashTable, 0, sizeof(hashTable));

    // Insere os elementos na tabela hash
    //#pragma omp parallel for schedule(static)
    for (int i = 0; i < MAX_N_SIZE; i++)
    {
        for (int j = 0; j < MAX_N_SIZE; j++)
        {
            insertHash(v_input_a[i][j]);
        }
    }
    

    // Testa se existem os valores de B na hash
    #pragma omp parallel
    {
        int numThreads = 0;
        numThreads = omp_get_num_threads();
        int threadId = omp_get_thread_num();

        #pragma omp for 
        for (int i = 0; i < MAX_N_SIZE; i++)
        {
            v_output[i] = searchHash(v_input_b[i], numThreads, threadId) ? 1 : 0;
        }
    }
}

/****** Auxiliar Functions ********/
int **read_a(char *fileName)
{
    FILE *input = fopen(fileName, "r");
    if (input == NULL)
    {
        printf("[ERR] File '%s' not found! (fix: change file directory)\n", fileName);
        exit(1);
    }

    int **v = (int **)malloc(sizeof(int *) * MAX_N_SIZE);
    for (int i = 0; i < MAX_N_SIZE; i++)
    {
        v[i] = (int *)malloc(MAX_N_SIZE * sizeof(int));
    }

    int num;
    char separator;

    for (int i = 0; i < MAX_N_SIZE; i++)
    {
        for (int j = 0; j < MAX_N_SIZE; j++)
        {
            fscanf(input, "%d%c", &num, &separator);
            v[i][j] = num;
        }
        fseek(input, 1, SEEK_CUR);
    }
    fclose(input);
    return v;
}

int *read_b(char *fileName)
{
    FILE *input = fopen(fileName, "r");
    if (input == NULL)
    {
        printf("[ERR] File '%s' not found! (fix: change file directory)\n", fileName);
        exit(1);
    }

    int *v = (int *)malloc(MAX_N_SIZE * sizeof(int));

    int num;
    char separator;

    for (int i = 0; i < MAX_N_SIZE; i++)
    {
        fscanf(input, "%d%c", &num, &separator);
        v[i] = num;
    }

    fclose(input);
    return v;
}

void write_output()
{
    FILE *output = fopen(OUTPUT_FILENAME, "w");
    for (int i = 0; i < MAX_N_SIZE; i++)
    {
        fprintf(output, "%d;", v_output[i]);
    }
    fclose(output);
    printf("[INFO] Arquivo de saida gerado em: '%s'\n", OUTPUT_FILENAME);
}

/******** Main **********/
int main(int argc, char *argv[])
{
    // Reading Files
    int **va = read_a(INPUT_FILENAME_A);
    int *vb = read_b(INPUT_FILENAME_B);

    // Writing output file
    solution(va, vb);
    write_output();

    // Measuring time
    printf("[INFO] Computando tempo de Execucao...\n");
    struct timeval start, end;

    gettimeofday(&start, NULL);
    for (int i = 0; i < EXECUTION_TIMES; i++)
    {
        solution(va, vb);
    }
    gettimeofday(&end, NULL);

    float time_spent = (((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))) / 1000.0f;
    printf("[INFO] Tempo total de %d execucoes:: %f mili-seconds \n", EXECUTION_TIMES, time_spent);

    // Free allocated memory
    for (int i = 0; i < MAX_N_SIZE; i++)
    {
        free(va[i]);
        // Free each linked list in the hash table
        HashNode *current = hashTable[i];
        while (current != NULL)
        {
            HashNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(va);
    free(vb);

    return 0;
}
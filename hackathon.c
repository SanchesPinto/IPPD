#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <libgen.h>
#include <omp.h>

#define INPUT_FILENAME_A  	"input-A.in"
#define INPUT_FILENAME_B  	"input-B.in"
#define OUTPUT_FILENAME 	"output.out"
#define MAX_N_SIZE 		20
#define EXECUTION_TIMES 	5
#define HASH_TABLE_SIZE    100 // Tamanho da tabela hash

/******** Solution **********/
int v_output[MAX_N_SIZE];

// Estrutura da tabela hash
typedef struct HashNode {
    int key;
    struct HashNode* next;
} HashNode;

HashNode* hashTable[HASH_TABLE_SIZE];

// Função de hash
int hashFunction(int key) {
    return key % HASH_TABLE_SIZE;
}

// Função para inserir na tabela hash
void insertHash(int key) {
    int index = hashFunction(key);
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->key = key;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

// Função para verificar se um elemento está na tabela hash
int searchHash(int key) {
    int index = hashFunction(key);
    HashNode* current = hashTable[index];
    while (current != NULL) {
        if (current->key == key) {
            return 1; // Encontrado
        }
        current = current->next;
    }
    return 0; // Não encontrado
}

// Função de solução modificada
void solution(int** v_input_a, int* v_input_b) {
    // Inicializa a tabela hash
    memset(hashTable, 0, sizeof(hashTable));

    // Insere todos os elementos de A na tabela hash
    for (int i = 0; i < MAX_N_SIZE; i++) {
        for (int j = 0; j < MAX_N_SIZE; j++) {
            insertHash(v_input_a[i][j]);
        }
    }

    // Verifica a presença de cada número de B na tabela hash
    #pragma omp parallel for
    for (int i = 0; i < MAX_N_SIZE; i++) {
        v_output[i] = searchHash(v_input_b[i]) ? 1 : 0;
    }
}


/****** Auxiliar Functions ********/
int** read_a(char *fileName){
    FILE *input  = fopen(fileName, "r");
    if(input == NULL){ 
        printf("[ERR] File '%s' not found! (fix: change file directory)", fileName); 
        exit(1);
    }
    int **v = (int**)malloc(sizeof(int*) * MAX_N_SIZE);
    for(int i = 0; i < MAX_N_SIZE; i++){
        v[i] = (int*)malloc(MAX_N_SIZE * sizeof(int));
    }

	int num;
	char separator;
    
    for(int i = 0; i < MAX_N_SIZE; i++){
        for(int j = 0; j < MAX_N_SIZE; j++){
          fscanf(input, "%d%c", &num, &separator);
          v[i][j] = num;

        }
        fseek(input, 1, SEEK_CUR);
    }
	fclose(input);
	return v;
}

int* read_b(char *fileName){
    FILE *input  = fopen(fileName, "r");	
    if(input == NULL){ 
        printf("[ERR] File '%s' not found! (fix: change file directory)", fileName); 
        exit(1);
    }
    int* v = (int*)malloc(MAX_N_SIZE * sizeof(int));

    int num;
	char separator;

    for(int i = 0; i < MAX_N_SIZE; i++){
        fscanf(input, "%d%c", &num, &separator);
        v[i] = num;
    }

	fclose(input);
	return v;
}

void write_output(){
    FILE *output = fopen(OUTPUT_FILENAME, "w");
    for(int i =0; i < MAX_N_SIZE; i++){
        fprintf(output, "%d;", v_output[i]);
    }
    fclose(output);
    printf("[INFO] Arquivo de saida gerado em: '%s'\n", OUTPUT_FILENAME);	
}

/******** Main **********/
int main(int argc, char *argv[]) {
	//Reading Files
	int** va = read_a(INPUT_FILENAME_A);
	int* vb = read_b(INPUT_FILENAME_B);

	//Writing output file
	solution(va, vb);
	write_output();		

	//Measuring time
	printf("[INFO] Computando tempo de Execucao...\n");		
	struct timeval start, end;
	
	gettimeofday(&start, NULL);
	for(int i=0; i<EXECUTION_TIMES; i++) {
		solution(va, vb);		
	}
	gettimeofday(&end, NULL);

	float time_spent = (((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))) / 1000.0f;
	printf("[INFO] Tempo total de %d execucoes:: %f mili-seconds \n", EXECUTION_TIMES, time_spent);	

	// Free allocated memory
	for (int i = 0; i < MAX_N_SIZE; i++) {
        free(va[i]);
        // Free each linked list in the hash table
        HashNode* current = hashTable[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }
	}
	free(va);
	free(vb);

	return 0;
}

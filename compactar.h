#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TAM 256

typedef struct node{
    unsigned char caracter;
    int frequencia;
    struct node *esq, *dir, *proximo;
} Node;

//head = FRONT 
typedef struct queue{
    Node *front;  
} Fila;


//==========DECLARAÇÕES DAS FUNÇÕES==============

FILE *Abrir(char nome_arquivo[]);

uint64_t calcular_tamanho_arquivo(FILE *arquivo);

unsigned char* ler_arquivo(FILE *arquivo);

Fila *criar_fila();

Node *criar_no(unsigned char cada_byte, int freq, Node *esq, Node *dir);

void inicializa_tabela_com_zero(unsigned int tab[]);

int preenche_tab_frequencia(char nome_arquivo[], unsigned int tab[]);

void inserir_ordenado(Fila *fila, Node *no);

void preencher_fila(unsigned int tab[], Fila *fila);

Node* remove_no_inicio(Fila *fila);

Node* montar_arvore(Fila* fila);

int altura_arvore(Node *raiz);

unsigned char** aloca_memoria_dicionario(int colunas);

void gerar_dicionario(unsigned char **dicionario, Node *raiz, char *caminho,int colunas);

uint64_t calcular_quantidade_bits(unsigned char **dicionario, char *nome_arquivo);
int calcular_lixo(uint64_t total_bits);

int calcular_tamanho_arvore(Node *raiz);

void percorrer_arvore_pre_ordem(Node *raiz, char *string, int *indice);

void fazer_cabecalho(unsigned char cabecalho[2], int lixo, int tamanho_arvore);

unsigned char ligar_bit(unsigned char c, int i);

void escrever_huffman(char *nome_arq_original, char *nome_arq_saida, unsigned char cabecalho[2], 
    char *array_arvore, int tam_arvore, unsigned char **dicionario);
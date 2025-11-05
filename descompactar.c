#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

long long tam_arvore = 0;
long long tam_lixo = 0;

// Estrutura do nó da Árvore de Huffman
typedef struct _snode {
	int conteudo; // O caractere (ou marcador '*')
	struct _snode *dir;
	struct _snode *esq;
} SNode;

// Cria um novo nó da árvore, alocando memória
SNode *SNode_create(int val, SNode *esq, SNode *dir){
	SNode *snode = (SNode*)calloc(1,sizeof(SNode));
	
	snode->conteudo = val;
	snode->esq =esq;
	snode->dir = dir;

	return snode;
}

// Libera recursivamente a memória da árvore
void destroy_arv(SNode *arv){
    if(arv == NULL){
        return;
    }
    
    destroy_arv(arv->esq);
    destroy_arv(arv->dir);
    
    free(arv);
}

// Isola e retorna o valor booleano (0 ou 1) de um bit específico em um byte
bool dir_esq(unsigned char byte, int indice){
    // Desloca o bit na posição 'indice' para o final (posição 0)
    int bit = byte >> indice;
    
    // Usa & 1 para verificar se o bit final é 1 (true) ou 0 (false)
    if(bit & 1){
        return true; 
    }
    
    return false; 
}

// Processa um byte de dados compactados, bit a bit
void bitAbit(unsigned int byteDaVez, SNode **aux_arv, SNode *arvore, FILE *arquivo_final, long *contadorDeBits){
    
    // Loop pelos 8 bits do byte (do mais significativo ao menos)
    for(int j = 7; j >= 0; j--){
         
         if(*contadorDeBits == 0){
             return; // Para se todos os bits válidos foram lidos
         }  
        
        // Navega para o filho DIREITO (bit 1)
        if(dir_esq(byteDaVez,j) == true && (*aux_arv)->dir != NULL){
            //o parenteses em (*aux_arv)-> dir significa: 
            //antes de apontar para a direita, como sou ponteiro duplo, mude a referencia para SNode* e depois aponte para a esq dele
            (*aux_arv) = (*aux_arv)->dir;
        }
        // Navega para o filho ESQUERDO (bit 0)
        else if(dir_esq(byteDaVez,j) == false && (*aux_arv)->esq != NULL){
            (*aux_arv) = (*aux_arv)->esq;
        }
        // Verifica se chegou a um nó folha (caractere decodificado)
        if((*aux_arv) != NULL && (*aux_arv)->esq == NULL && (*aux_arv)->dir == NULL){
            
            fputc((*aux_arv)->conteudo,arquivo_final); // Escreve o caractere decodificado
            *aux_arv = arvore; // Volta o ponteiro de navegação para a raiz
            
        }
        
        (*contadorDeBits)--; // Marca um bit como lido
    }
}

// Coordena a leitura dos dados e a decodificação da Árvore de Huffman
bool descompactar_arquivo(FILE *arquivo_recebido, SNode *arvore, FILE *arquivo_final){
    
    // Calcula o tamanho total, cabeçalho e bytes de dados válidos
    fseek(arquivo_recebido,0,SEEK_END);
    long tam_total = ftell(arquivo_recebido);
    
    //tamanho do cabeçalho = 2 bytes + tamanho da arvore serializada
    long cabecalho = 2 + tam_arvore;
    //os dados vao vir depois do cabeçalho
    long bytes_validos = tam_total - cabecalho;
    
    // Calcula o total de bits válidos para decodificação (subtrai o lixo)
    long total_bits = bytes_validos * 8;
    long bits_validos = total_bits - tam_lixo;
    
    long contadorDeBits = bits_validos;
    
    // Posiciona o ponteiro de leitura no início dos dados compactados
    fseek(arquivo_recebido,cabecalho,SEEK_SET);
    unsigned int byteDaVez;
    
    SNode *aux_arv = arvore;
    
    // Se não houver dados válidos, retorna erro
    if(bits_validos <= 0){
        return false;
    }
    
    // Loop principal: lê e processa cada byte do arquivo de dados
    while(bytes_validos > 0){
        
        byteDaVez = fgetc(arquivo_recebido); // Lê o byte
        
        // Processa os 8 bits do byte lido
        bitAbit(byteDaVez,&aux_arv,arvore,arquivo_final,&contadorDeBits);
        
        // Se todos os bits válidos foram decodificados, encerra
        if(contadorDeBits == 0){
            return true;
        }
        
        bytes_validos--;
    }
    return true;
}

// Cria um novo arquivo de saída com a extensão ".fim"
FILE *preparar_arquivo_final(char nome_arquivo[]){
    
    char nome_arquivo_final[1000];
    
    strcpy(nome_arquivo_final,nome_arquivo);
    
    strcat(nome_arquivo_final, ".fim");
    
    return fopen(nome_arquivo_final, "wb"); // Abre para escrita binária
}

// Função recursiva para reconstruir a árvore a partir do array serializado
SNode *criar_arvore(unsigned char **ponteiro_array){
    
    // Lê o caractere/marcador na posição atual do array
    //LEITURA
    unsigned char valor_array = **ponteiro_array;
    
    if(valor_array == '\\'){ // Encontrou marcador de nó folha
        //NAVEGAÇAO
        (*ponteiro_array)++;// Avança para o caractere real
        
        return (SNode_create(**ponteiro_array,NULL,NULL)); // Cria nó folha
    }
    else if(valor_array == '*'){ // Encontrou marcador de nó interno
        (*ponteiro_array)++; // Avança
        
        // Chamada recursiva para montar o ramo ESQUERDO
        SNode *node_esq = criar_arvore(ponteiro_array);
        
        (*ponteiro_array)++; // Avança para o início do ramo direito
        
        // Chamada recursiva para montar o ramo DIREITO
        SNode *node_dir = criar_arvore(ponteiro_array);
        
        return (SNode_create('*',node_esq,node_dir)); // Cria nó interno
    }
    
    else{
        return (SNode_create(valor_array,NULL,NULL)); // Trata caracteres sem '\'
    }
}

// Lê os bytes da árvore serializada do arquivo e inicia a reconstrução
SNode *gerarArvore(FILE *arquivo_recebido, unsigned int tam_arvore){
    
    // Aloca memória para o array que vai armazenar a serialização da árvore
    unsigned char *arrayRoot = (unsigned char*) calloc(tam_arvore,sizeof(unsigned char));
    
    fseek(arquivo_recebido,2,0);// Pula os 2 bytes do cabeçalho de lixo e tamanho
    
    unsigned int i=0;
    while(i < tam_arvore){
        
        arrayRoot[i] = fgetc(arquivo_recebido); // Lê o byte da árvore
        i++;
    }
    
    unsigned char *auxRoot = arrayRoot; // Guarda a referência inicial para liberar a memória
    
    SNode *arvoreConstruida = criar_arvore(&arrayRoot); // Constrói a árvore de forma recursiva
    
    free(auxRoot); // Libera o array temporário
    
    return arvoreConstruida;
}

// Lê e calcula o tamanho do lixo e o tamanho da árvore a partir do cabeçalho (2 bytes)
void tam_arvore_lixo(FILE *arquivo_recebido){
    rewind(arquivo_recebido); // Volta para o início do arquivo (reposiciona o ponteiro do arquivo para o início dele)
    
    // Lê o primeiro byte: contém tam_lixo (3 bits) e parte de tam_arvore (5 bits)
    unsigned char byte01 =  fgetc(arquivo_recebido);
    

    // Isole o tam_lixo (3 bits MSB) usando deslocamento de 5 posições
    tam_lixo = (unsigned int)(byte01 >> 5);
    
    // Lê o segundo byte: contém os 8 bits restantes de tam_arvore
     unsigned char byte02 =  fgetc(arquivo_recebido);
    
    // Mascara o byte01 para isolar apenas os 5 bits do tam_arvore
     byte01 = byte01 << 3; // Remove o lixo
     byte01 = byte01 >> 3; // Retorna os bits para as posições LSB
     
    // Combina os 5 bits de byte01 (parte alta) e os 8 bits de byte02 (parte baixa)
        tam_arvore = ((unsigned int)byte01 << 8) | byte02; //usa tabela verdade do OU pra unir os bits e formar o tamanho da arvore (13 bits)
}

// Pede o nome do arquivo e tenta abri-lo em modo binário de leitura
FILE *Abrir(char nome_arquivo[]){
    
    FILE *ARQ = NULL;
    
    printf("Qual o nome do arquivo?\n");
    
    fgets(nome_arquivo,1000,stdin);
    nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0';
    
    ARQ = fopen(nome_arquivo, "rb");
    if(ARQ == NULL){
       
        return ARQ;
    }
    if(ARQ != NULL){
        printf("SUCESSO NA ABERTURA DO ARQUIVO\n");
        return ARQ;
    }
    
}


/////FUNÇÃO PRINCIPAL/////////////////
void descompactar_huffman(){
    
    FILE *arquivo_recebido = NULL;
    FILE *arquivo_final = NULL;
    SNode *root = NULL;
    
    char nome_arquivo[1000];
    
    // 1. Tenta abrir o arquivo compactado
    arquivo_recebido = Abrir(nome_arquivo);

    if(arquivo_recebido == NULL){
         printf("Erro ao abrir arquivo!\n");
        return; // Encerra se não abrir
    }
    
    printf("Arquivo aberto com sucesso!\n");
    
    // 2. Lê e calcula o tamanho do lixo e da árvore a partir do cabeçalho
    tam_arvore_lixo(arquivo_recebido);
    
    // 3. Reconstrói a Árvore de Huffman
    root = gerarArvore(arquivo_recebido,tam_arvore);
    
    if(root == NULL && tam_arvore > 0){
        printf("ERRO\n"); // Falha na reconstrução de uma árvore esperada
    }
    else{
        // 4. Prepara o arquivo de saída
        arquivo_final = preparar_arquivo_final(nome_arquivo);
        
        if(arquivo_final == NULL){
            printf("Error ao preparar arquivo final\n");
            // Cleanup parcial e saída
            destroy_arv(root);
            fclose(arquivo_recebido);
            fclose(arquivo_final);
            return;
        }
        
        // 5. Decodifica os dados
        if(descompactar_arquivo(arquivo_recebido,root,arquivo_final)){
            printf("A descompactação foi sucesso!");
        }
        else{
            printf("Error ao Descompactar!!");
        }
    }
        
    // 6. Cleanup final: fecha arquivos e libera memória
    destroy_arv(root);
    fclose(arquivo_recebido);
    fclose(arquivo_final);
        
    return;
}

int main(){
    
    descompactar_huffman();
    
    return 0;
}


//==============ARQUIVOS==========
#include "compactar.h"

FILE *Abrir(char nome_arquivo[])
{
    
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
    
    return NULL;
}


uint64_t calcular_tamanho_arquivo(FILE *arquivo)
{
    //ftell vai informar onde esta o cursor
    //exemplo: se um arquivo tem 5000 bytes, as posiçoes vao de 0 a 4999
    //a posicao 5000 nesse caso seria EOF

    long int posicao_cursor = ftell(arquivo);

    //fseek move o cursor para o final do arquivo
    //parametro do meio é quantidade de bytes que vai deslocar
    fseek(arquivo,0,SEEK_END); //seek_end significa que a posicao de referencia é EOF, end of file 

    //le a posicao do cursor que esta no final do arquivo
    uint64_t tamanho_arquivo = ftell(arquivo);

    //restaura posiçao do cursor
    fseek(arquivo,posicao_cursor,SEEK_SET);

    return tamanho_arquivo;
}

unsigned char* ler_arquivo(FILE *arquivo)
{
    if(arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo");
        return NULL; // parar de executar
    }

    uint64_t tamanho_arquivo = calcular_tamanho_arquivo(arquivo);
    unsigned char* dados = calloc(tamanho_arquivo,1); // 1 = sizeof(unsigned char)
    if(dados == NULL)
    {
        printf("Erro em alocar memória para os dados do arquivo");
        return NULL; //parar de executar
    }

    //parametros do fread:
    //ponteiro para o bloco que vai armazenar dados, tamanho de cada byte, quantidade de bytes, arquivo de entrada
    //o tamanho é 1 pq unsigned char = 1 byte
    fread(dados,1,tamanho_arquivo,arquivo);

    return dados;
}


Fila *criar_fila(){
    Fila *F = (Fila *) calloc(1,sizeof(Fila));
    F->front = NULL;

    return F;
}

Node *criar_no(unsigned char cada_byte, int freq, Node *esq, Node *dir)
{
    Node *no = (Node*) calloc(1,sizeof(Node));

    //connexão no, arvore e tabela_frequencia
    no->caracter = cada_byte;
    no->frequencia = freq;
    
    no->esq = esq;
    no->dir = dir;
    //por padrão, o ultimo nó da lista é NULL
    no->proximo = NULL;

    return no;
}

//-------------------------Parte 1: tabela de frequencia ---------------------------------

void inicializa_tabela_com_zero(unsigned int tab[]){
    for(int i = 0; i < TAM; i++){
        tab[i] = 0;
    }
}

int preenche_tab_frequencia(char nome_arquivo[], unsigned int tab[]){
    int i = 0;
    int cada_byte = 0; //byte lido
    
    FILE *documento = fopen(nome_arquivo, "rb");
    
    //para verificar se não teve error ao abrir arquivo;
    if(documento == NULL){
        printf("ERRO");
        return 0;
    }
    // FGETC RETORNA O VALOR DO BYTE LIDO
    while((cada_byte = fgetc(documento)) != EOF){
        tab[cada_byte]++;
        i++;
    }
    
    //fecha o arquivo apos leitura
    fclose(documento);
    
    return 1;//deu certo
}

//-------------------------Parte 2: Inserção de forma ordenada ---------------------------------


void inserir_ordenado(Fila *fila, Node *no){
    //a fila esta vazia? 
    if(fila->front == NULL){
        fila->front = no;
    }
    //tem frequencia menor que o inicio da fila
    else if(no->frequencia < fila->front->frequencia){
        no->proximo = fila->front;
        fila->front = no;
    }
    //inserção no meio
    else{
        Node *aux = fila->front;
        while(aux->proximo != NULL && aux->proximo->frequencia <= no->frequencia)
        {
            aux = aux->proximo;
        }
        no->proximo = aux->proximo;
        aux->proximo = no;
    }
}

void preencher_fila(unsigned int tab[], Fila *fila){
    //percorrer toda a tabela
    Node *no_novo;
    for(int i = 0; i < TAM; i++){
       if(tab[i] > 0){
           no_novo = malloc(sizeof(Node));
           if(no_novo){//alocado com sucesso 
               no_novo->caracter = i;
               no_novo->frequencia = tab[i];
               no_novo->dir = NULL;
               no_novo->esq = NULL;
               no_novo->proximo = NULL;
               
               inserir_ordenado(fila, no_novo);
           }
           else{
               printf("\tERRO AO ALOCAR MEMORIA EM preencher_fila!\n");
               break;//sai do loop
           }
       }
    }
}

//-------------------------Parte 3: Montar a árvore de Huffman ---------------------------------

Node* remove_no_inicio(Fila *fila){
    Node *aux = NULL;
    
    if(fila->front){ 
        aux = fila->front; //armazenar o ponteiro para o nó que vai ser removido
        fila->front = aux->proximo; //atualiza o topo da fila
        aux->proximo = NULL; 
    }
    
    return aux;
}

Node* montar_arvore(Fila* fila){
    Node *primeiro, *segundo, *novo;
    while(fila->front->proximo != NULL){
        primeiro = remove_no_inicio(fila);
        segundo = remove_no_inicio(fila);
        novo = calloc(1,sizeof(Node));
        
        if(novo){
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->proximo = NULL;
            
            inserir_ordenado(fila,novo);
        }
        else{
            printf("\nERRO AO ALOCAR MEMORIA EM montar_arvore");
            break;
        }
    }
    
    return fila->front;
}

//-------------------------Parte 4: Montar o dicionário ---------------------------------

//retorna o numero de arestas do caminho mais longo da raiz até uma folha
int altura_arvore(Node *raiz) //maior caminho da raiz ate a folha mais distante
{
    int caminho_esq = 0;
    int caminho_dir = 0;

    if(raiz == NULL) //(caso base) arvore varia ou no que nao existe
    {
        return 0; //filho nao existe -> altura = 0
    }
    else
    {
        if(raiz->esq != NULL) // para contabilizar apenas flhos de nos que existem
        {
            // somar 1 pra contabilizar o tamanho da raiz/no atual, que é 1
            caminho_esq = altura_arvore(raiz->esq) + 1;
        }

        if(raiz->dir != NULL)
        {
            // somar 1 pra contabilizar o tamanho da raiz/no atual, que é 1
            caminho_dir = altura_arvore(raiz->dir) + 1;
        }

        if(caminho_esq > caminho_dir)
        {
            return caminho_esq;
        }
        else
        {
            return caminho_dir;
        }
    }
}

// PERCORRER ARVORE PARA OBTER O CODIGO DE HUFFMAN (0s e 1s, esq e dir)
//percorrer da raiz ate cada no, esquerda é '0' e direita é '1'
//ate atingir a folha
unsigned char** aloca_memoria_dicionario(int capacidade_codigos)
{
    int i;
    unsigned char **dicionario;

    dicionario = calloc(TAM,sizeof(unsigned char*)); 
    if(dicionario == NULL)
    {
        printf("Erro em alocar memória para o dicionario");
        return NULL;
    }

    for(i=0;i<TAM;i++)
    {
        //calloc aloca memoria e retorna o ponteiro, porém LIMPA aquela regiao de memoria
        dicionario[i] = calloc(capacidade_codigos, sizeof(unsigned char)); 
    }
    return dicionario;
}

//caminhando na arvore e concatenando o 0 ou 1 pra gerar o codigo
void gerar_dicionario(unsigned char **dicionario, Node *raiz, char *caminho,int capacidade_codigos)
{
    //ARMAZENAR CAMINHO ATE AS FOLHAS
    //precisam ter no minimo a quantidade de colunas da matriz
    //armazenar endereço retornado por calloc em um ponteiro
    // *esquerda e *direita apontam para um array de quantidade de colunas caracteres
    char *esquerda = calloc(capacidade_codigos,sizeof(char)); //sizeofchar = 1
    char *direita = calloc(capacidade_codigos,sizeof(char));
    //antes: unsigned char direita[colunas];

    if(raiz->esq == NULL && raiz->dir == NULL) //se estamos em uma folha
    {
        //strcpy(destino,origem) copiar string para dicionário;
        strcpy((char*)dicionario[raiz->caracter], (char*)caminho);    
    }
    else
    {
        //se nao chegou na folha (é um no intermediario)
        //esse 3 parametro garante que nunca vai adicionar mais do que isso
        strncpy(esquerda, caminho, capacidade_codigos);
        strncpy(direita, caminho, capacidade_codigos);
        strncat(esquerda,"0", 2);
        strncat(direita, "1", 2);

        gerar_dicionario(dicionario,raiz->esq,esquerda,capacidade_codigos);
        gerar_dicionario(dicionario,raiz->dir,direita,capacidade_codigos);

        //liberar memoria alocada em calloc
        free(esquerda);
        free(direita);

    }
}

//-------------------------Parte 5: Compactar ---------------------------------
//DIVIDIDO EM VARIAS ETAPAS/FUNÇÕES


//calcular quantos bytes o arquivo codificado ocupará depois de comprimido
//para cada byte do arquivo, pega o codigo do byte, conta quantos bits tem e soma no total
uint64_t calcular_quantidade_bits(unsigned char **dicionario, char *nome_arquivo)
{
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao reabrir arquivo em calcular_quantidade_bits\n");
        return 0;
    }

    unsigned char *codigo;
    uint64_t total = 0;
    int cada_byte;

    while ((cada_byte = fgetc(arquivo)) != EOF)
    {
        codigo = dicionario[cada_byte]; 
        total += strlen((char*)codigo);
    }

    fclose(arquivo);
    return total;
}



//calcular lixo (que serve para completar bits)
int calcular_lixo(uint64_t total_bits)
{
    int quant_lixo = 8 - (total_bits % 8);

    return quant_lixo;
}


//calcular a quantidade de caracteres da arvore para poder construir a string
int calcular_tamanho_arvore(Node *raiz)
{
    int tamanho_arvore = 0;

    if(raiz != NULL)
    {
        //se é folha e tem o caractere de escape
        //nao chama recursivamente pois é o fim da arvore
        //se raiz->caracter for o asterisco literal (char asterisco) ou seja, de escape \*
        //if(raiz->caracter == '*' && raiz->esq == NULL && raiz->dir == NULL)
        if(raiz->esq == NULL && raiz->dir == NULL && (raiz->caracter == '*' || raiz->caracter == '\\'))
        {
            tamanho_arvore += 2; //conta dois bytes: \ e * (\*)
        }
        //é folha mas nao tem caractere de escape
        //else if(raiz->caracter != '*' && raiz->esq == NULL && raiz->dir == NULL)
        else if(raiz->esq == NULL && raiz->dir == NULL)
        {
            tamanho_arvore += 1; 
        }
        //no com filhos (* sem ser de escape)
        //se tiver um filho so (qualquer lado) ou dois filhos
        else if(raiz->esq != NULL || raiz->dir != NULL)
        {
            tamanho_arvore += 1; 
            if(raiz->esq != NULL)
            {
                tamanho_arvore += calcular_tamanho_arvore(raiz->esq);
            }
            if(raiz->dir != NULL)
            {
                tamanho_arvore += calcular_tamanho_arvore(raiz->dir); 

            }
        }
       
    }

    else
    {
        printf("Erro ao calcular tamanho da árvore");
        return 0; //arvore vazia
    }

    return tamanho_arvore;
}

/*
transformar arvore em string seguindo as regras:
-> percorrer por pré-ordem
-> utilizar "*" para nó com filhos
-> se for uma folha:
    -> se o byte for o asterisco literal, escreve \*
    -> se o byte for qualquer coisa menos o asterisco literal, escreve o byte

-> utilizar "\" como caractere de escape (caso o arquivo contenha o * de forma literal)

*/
void percorrer_arvore_pre_ordem(Node *raiz, char *string, int *indice) 
{
    if(raiz != NULL)
    {
        //achar folhas (nós sem filhos)
        if(raiz->esq == NULL && raiz->dir == NULL)
        {
            //caso seja folha e asterisco literal OU barra literal
            if(raiz->caracter == '*' || raiz->caracter == '\\')
            {
                // Escreve o char de escape '\'
                string[*indice] = '\\';
                (*indice)++;
                // Escreve o char literal (* ou \)
                string[*indice] = raiz->caracter;
                (*indice)++;
            }
            else
            {
                // Escreve o char literal (qualquer um, inclusive \0)
                string[*indice] = raiz->caracter;
                (*indice)++;
            }
        }
        //caso seja nó com filhos
        else
        {
           // Escreve o char de nó interno '*'
           string[*indice] = '*';
           (*indice)++;
        }
        
        // Chamadas recursivas com o índice
        percorrer_arvore_pre_ordem(raiz->esq, string, indice);
        percorrer_arvore_pre_ordem(raiz->dir, string, indice);
    }
}

void fazer_cabecalho(unsigned char cabecalho[2], int lixo, int tamanho_arvore)
{
    cabecalho[0] = 0;
    cabecalho[1] = 0;

    //pegar os bits do lixo e deslocar 5 pra esquerda
    //ou seja, sobram 5 pra guardar o começo do tamanho
    //o resto do tamanho ficará no outro byte
    unsigned char bits_lixo = (unsigned char)(lixo << 5);

    //desloca 8 pra direita, pois se livra dos 8 últimos do segundo byte
    unsigned char tamanho_arvore_comeco = (unsigned char)(tamanho_arvore >> 8);

    //agora precisa juntar a parte do lixo com o começo do tamanho
    //se pelo menos um dos dois tiver 1, volta 1, pra poder "concatenar"
    cabecalho[0] = bits_lixo | tamanho_arvore_comeco;

    //fazer o segundo byte que tem so informação do tamanho, o que ficou de fora
    //usa & como "e" da logica mesmo, o 255 é tipo o byte todo "ativado" 11111111
    //o & so retorna 1 se ambas forem 1, aí onde for 1 do byte do tamanho, volta 1, onde for 0, volta  0
    cabecalho[1] = (unsigned char)(tamanho_arvore & 255);
}





// b = byte inteiro que quer manipular
// i = posiçao do bit que queremos ligar
//ativar um bit especifico sem afetar os outros
unsigned char ligar_bit(unsigned char b, int i) 
{
    // 1 << i cria a máscara. Ex: i=2 -> 00000100

    unsigned char mascara = 1 << i; //desloca o bit 1 pra esquerda "i" posições
    return mascara | b; // usar o ou para ligar naquela posicao
}



        /*
            Código Huffman: "101"

            Bit '1': 7 -    0 = 7 → liga bit 7 → 10000000
            Bit '0': não    liga bit 6 → 10000000
            Bit '1': 7 -    2 = 5 → liga bit 5 → 10100000

            Resultado: byte_rascunho = 10100000
        */


void escrever_huffman(char *nome_arq_original, char *nome_arq_saida, unsigned char cabecalho[2], 
    char *array_arvore, int tam_arvore, unsigned char **dicionario)
{
    FILE* novo_saida = fopen(nome_arq_saida, "wb");  //arquivo de saida ja compactado

    FILE* entrada = fopen(nome_arq_original, "rb"); //arquivo que vai ser compactado

    
    fwrite(cabecalho, sizeof(unsigned char), 2, novo_saida); //escreve o cabeçalho no novo arquivo

    //escreve os bytes da arvore (arvore em formato de string)
     fwrite(array_arvore, sizeof(unsigned char), tam_arvore, novo_saida);

    fflush(novo_saida); // força escrita

    int cada_byte_do_original; 
    unsigned char byte_rascunho = 0;
    int contabit= 0;   

    //vamos empacotar os bits dentro do nosso byte de rascunho
    while((cada_byte_do_original = fgetc(entrada)) != EOF)
    {
        //para cada byte do original, pega o codigo do huffman (dicionario, ex: 101)
        //para cada caractere do codigo que esta no dicionario ('0' ou '1'), liga ou desliga no byte_rascunho
        unsigned char *codigo = dicionario[cada_byte_do_original];
        for(uint64_t i = 0; i < strlen((char*)codigo); i++)
        {
            if(codigo[i] == '1')
            {
                // 7 - contabit é qual posiçao ligar: quando contabit = 0, liga o bit 7-0 = 7 (primeiro) e assim vai
                // preencher do bit mais importante pro menos importante
                byte_rascunho = ligar_bit(byte_rascunho, 7 - contabit); 
            }   
            contabit++; //contabit diz quantos bits ja colocamos no byte_rascunho

            if(contabit == 8) //quando atinge 8, significa que byte_rascunho esta completo
            {
                fputc(byte_rascunho, novo_saida);
                byte_rascunho = 0;
                contabit = 0;
            }

        }
    }

    //caso tenha sobrado bits que nao completaram 1 byte
    if (contabit > 0) 
    {
        fputc(byte_rascunho, novo_saida);
    }

    fclose(entrada);
    fclose(novo_saida);
}

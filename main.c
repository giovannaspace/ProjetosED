//========== HEADER ============
#include "compactar.h"


int main()
{
    FILE *arquivo_recebido = NULL;
    char nome_arquivo[1000];

    //tirei: unsigned char nome_arquivo[] = "arquivo_teste.jpeg";
    /* ANTES ERA ASSIM:
    arquivo_recebido = Abrir(nome_arquivo);
    //...
    unsigned char* dados_arquivo = ler_arquivo(arquivo_recebido); // !! PROBLEMA DE MEMÓRIA !!
    printf("Arquivo aberto com sucesso!\n");
    //...
    preenche_tab_frequencia(nome_arquivo,tabela_frequencia);
    //...
    uint64_t total_bits = calcular_quantidade_bits(dicionario,dados_arquivo,arquivo_recebido); // Passava o buffer da RAM
    //...
    escrever_huffman(nome_arquivo,"teste_compactado.huff",cabecalho,string_arvore,tamanho_arvore,dicionario);
*/
    
    
    arquivo_recebido = Abrir(nome_arquivo);

    if(arquivo_recebido == NULL) 
    {
        printf("Erro ao abrir arquivo!\n");
        //return;
    }

    printf("Arquivo aberto com sucesso!\n");
    fclose(arquivo_recebido);

    unsigned int tabela_frequencia[TAM];
    unsigned char cabecalho[2];

//-------------------------Parte 1: Tabela de frequencia ---------------------------------
    inicializa_tabela_com_zero(tabela_frequencia);
    preenche_tab_frequencia(nome_arquivo,tabela_frequencia);
    //imprime_tab_frequencia(tabela_frequencia);

//-------------------------Parte 2: Fila ordenada ---------------------------------
    Fila* fila = criar_fila();
    preencher_fila(tabela_frequencia,fila);

//-------------------------Parte 3: Árvore de huffman ---------------------------------
    Node* arvore = montar_arvore(fila);

//-------------------------Parte 4: Montar dicionário ---------------------------------
    int  tamanho_maximo_codigo = altura_arvore(arvore) + 1; //conta mais 1 pra contabiliar o espaço do \0 da string que fica no dicionario
    unsigned char **dicionario;
    dicionario = aloca_memoria_dicionario(tamanho_maximo_codigo);
    gerar_dicionario(dicionario,arvore,"", tamanho_maximo_codigo);
    //imprime_dicionario(dicionario);

//-------------------------Parte 5: Compactar ---------------------------------

    uint64_t total_bits = calcular_quantidade_bits(dicionario, nome_arquivo);

    int tamanho_arvore = calcular_tamanho_arvore(arvore);

    int quant_lixo = calcular_lixo(total_bits);

    //nao precisa do (char*) antes de calloc pois em C, ja converte automaticamente pra char* 
    //aloca espaço de tamanho_arvore para fazer a string da arvore
    char *string_arvore = calloc(tamanho_arvore,sizeof(char));

    //MUDOU A CHAMADA DA FUNCAO, PRA PASSAR ESSE INDICE !!!!!!!

    int indice_arvore = 0; 
    percorrer_arvore_pre_ordem(arvore, string_arvore, &indice_arvore); 

    fazer_cabecalho(cabecalho,quant_lixo,tamanho_arvore);

    printf("Gerando arquivo compactado...\n");
    escrever_huffman(nome_arquivo,"teste_compactado.huff",cabecalho,string_arvore,tamanho_arvore,dicionario);
    printf("Arquivo criado com sucesso!\n");
    printf("Compactação concluída.\n");

    return  0;
}
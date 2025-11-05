#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define NUM_TESTES 100

typedef struct _snode {
	int conteudo; 
	struct _snode *dir;
	struct _snode *esq;
} SNode;

typedef struct _arvore {
	SNode *raiz; //raiz da arvore
} Arvore;

//////////////// LISTA ENCADEADA ///////////////////////////
///////////////////////////////////////////////////////////
typedef struct _snodex {
    int num; //elemento da lista
	struct _snodex *next; //endereC'o do prox. nC3
} node;

typedef struct _list {
	node *begin; //Ponteiro para o primeiro nC3 da lista
}List;

List *List_create() {
	List *L = (List *) calloc(1,sizeof(List));
	L->begin = NULL;

	return L;
}

node *Node_create(int val) {
	node *snode = (node*) calloc(1,sizeof(node));
	snode->num = val;
	snode->next = NULL;

	return snode;
}

//inserir elmento na cabeC'a da lista
void List_add_cabeca(List *L, int val) {
	node *p = Node_create(val);
	
	p->next = L->begin;
	L->begin = p;
}

int busca_lista(List *L, int valor){
    
    node *aux = L->begin;
    long int contador_lista = 0;
    
    while(aux != NULL){
        contador_lista++;
        
        if(aux->num == valor){
            
            //printf("Achei na Lista\n");
            return contador_lista;
        }
        aux = aux->next;
    }
    
    //printf("O número não está na lista\n");
    return contador_lista;
}

//////////////// ÁRVORE BINÁRIA ///////////////////////////
///////////////////////////////////////////////////////////

SNode *SNode_create(int val, SNode *esq, SNode *dir){
	SNode *snode = (SNode*) calloc(1,sizeof(SNode));
	
	snode->conteudo = val;
	snode->esq =esq;
	snode->dir = dir;

	return snode;
}

Arvore *Arvore_create() {
	Arvore *A = (Arvore *) calloc(1,sizeof(Arvore));
	
	A->raiz = NULL;

	return A;
}

long int buscae(SNode *raiz, int chave){
    if(raiz == NULL){
        //printf("O número não está na Árvore\n");
        return 0;
    }
    else{
        if(raiz->conteudo == chave){
            
            //printf("Achei na Árvore\n");
            return 1;
        }
        else{
            if(chave < raiz->conteudo){
                //return 1 = uma comparação
                return 1 + buscae(raiz->esq, chave);
            }
            else{
                
                return 1 + buscae(raiz->dir, chave);
            }
        }
    }
}

SNode *inserir_node(SNode *raiz,long int valor){
    
    if(raiz == NULL){
        SNode *snode = SNode_create(valor,NULL,NULL);
        return snode;
    }
    else{
        if(valor < raiz->conteudo){
            raiz->esq = inserir_node(raiz->esq,valor);
        }
        else{
            raiz->dir = inserir_node(raiz->dir,valor);
        }
        
        return raiz;
    }
}


int main(){
    
    Arvore *arvore = Arvore_create();
    List *lista = List_create();
    
    int array_num_inserido[1000];
    int array_testes_arvore[NUM_TESTES];
    int array_testes_lista[NUM_TESTES];
    
    int num = 0;
    
    // inserir números aleátorios até 1000 (100 numeros aleatorios entre 0 e 999)
    // pode inserir números repetidos
    for(int f = 0; f < 1000; f++){
        
        num = rand() % 1000;
        
        array_num_inserido[f] = num;
        arvore->raiz = inserir_node(arvore->raiz,num);
        List_add_cabeca(lista,num);
    }
    
    //Busca nas estruturas lista e arvore binária
    // como tem numeros repetidos, ele vai parar no primeiro num
    // que tem repetição. 
    int i = 0;
    int s = 0;
    while(i < NUM_TESTES){ //roda 100 testes de busca
        
        num = rand() % 1000;
        
        //antes dessa implementação havia um problema, chamava muitos numeros que nao estavam inseridos
        //agora com esse if, garante que metade dos testes busque um numero que esta na estrutura de certeza 
        //e a outra metade busque um numero aleatorio que pode ou nao estar
        if(s % 2 == 0){
            num = array_num_inserido[num];
        }
        
        //numero de comparações(custo) é salvo nos arrays
        array_testes_arvore[i] = buscae(arvore->raiz,num);
        array_testes_lista[i] = busca_lista(lista,num);
        
        s++; // incrementa pra deixar de ser par, depois voltar, e assim vai
        i++;
    }
    
    //salvar em arquivo para mandar para o raiz
    FILE *arquivo_R = fopen("arvore_lista_R.csv", "w");
    int m = 1;
    
    fprintf(arquivo_R, "NUMEROS SORTEADOS, COMPARACAO LISTA, COMPARACAO ARVORE\n");
    
    while(m <= NUM_TESTES){
        
      fprintf(arquivo_R, "%d,%d,%d\n", m, array_testes_lista[m - 1],
                                          array_testes_arvore[m - 1]);
        
        m++;
    }

    List_destroy(&lista);
    destroy_arv(arvore->raiz);
    
    fclose(arquivo_R);
    
    return 0;
}

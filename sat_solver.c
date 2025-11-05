#include <stdio.h>
#include <stdlib.h>

int **formula; // array de arrays -> ponteiro pra ponteiro de inteiros
int num_variaveis;
int num_clausulas; //variaveis pra guardar info sobre o arquivo dimacs
int* valor_da_variavel;

int valorAbsoluto(int numero) {
    if (numero < 0) {
        return -numero; // ou numero * -1
    }
    return numero;
}


void leitura_e_obter_formula(const char* nome_arquivo){
FILE* pont_arq; //FILE é padrao do C. esse ponteiro serve para controlar o arquivo
char buffer_da_linha[256]; // tipo um espaço de rascunho pra nossa funcao guardar uma linha de texto do nosso arquivo, uma por vez
pont_arq = fopen(nome_arquivo, "r"); //fopen tambem padrao do C, pra sincronizar entre o algoritmo e o arquivo 
//ela recebe o nome do arquivo, ai usamos o "r" de read, pra ativar o modo de leitura, sem modificar. vai ate o s.o e pede pra acessar o arquivo
//se der certo, ela cria uma estrutura FILE com as informações e devolve um ponteiro pra isso;
if(pont_arq == NULL){
    printf("Erro, não conseguimos abrir o arquivo");
    exit(1);
}

while(fgets(buffer_da_linha, sizeof(buffer_da_linha), pont_arq) != NULL)
{
    // o fgets vai ler os caracteres do arquivo para o qual o pont_arq aponta e copia para o buffer

    if(buffer_da_linha[0] == 'p'){ // procura pela linha que começa com p, é nela que tem o num de variaveis e de clausulas
    // quando acha, usa o sscanf pra pegar os valores da string
    if(sscanf(buffer_da_linha, "p cnf %d %d", &num_variaveis, &num_clausulas) == 2){
        printf("\n  Temos: %d variaveis e %d clausulas\n", num_variaveis, num_clausulas);
        break; // se o sscanf obter exatamente 2 "valores", então deu certo, o arquivo ta certinho
    }

    }
}

formula = (int **)malloc(num_clausulas * sizeof(int *)); //alocar ponteiros (linhas da matriz) que estao apontando para o vazio
if(formula == NULL){
    printf("Erro ao alocar memoria das clausulas");
    exit(1);
}
int literal_lido; int contador_clausulas = 0;
int buffer_da_claus_aux[num_variaveis + 1]; // nosso buffer temporario para verificar as clausulas. o + 1
int contador_literal = 0;

while(fscanf(pont_arq, "%d", &literal_lido) == 1){
    if(literal_lido != 0){
        buffer_da_claus_aux[contador_literal] = literal_lido;
        contador_literal ++;
    }else{
        //a
        formula[contador_clausulas] = (int*)malloc((contador_literal + 1) * sizeof(int)); //acessa a "linha" alocada antes e preenche com o array clausula no indice da ordem que ela aparece
        //quando encontra 0, é o fim daquela clasula, aí aqui aloca memoria para a clausula e mais 1 pra guardar o 0 do final
        if(formula[contador_clausulas] == NULL){
            printf("Erro na alocação de memoria da clausula"); exit(1);
        }
        for(int i = 0; i < contador_literal; i++){
            formula[contador_clausulas][i] = buffer_da_claus_aux[i];
        //copiando os literais do buuffer auxiliar para o local que devem estar 
        }
        formula[contador_clausulas][contador_literal] = 0; // o 0 no final
        contador_clausulas ++;
        contador_literal = 0; // reinicia para os literais da proxima clausula
        if(contador_clausulas >= num_clausulas){
            break;
        }
    }
}
fclose(pont_arq);
}

int analisar_formula(){
    //essa funcao serve para checar se, com as atribuições atuais, a formula é satisfeita
    for(int i = 0; i < num_clausulas; i++){
        int clausula_true = 0;
        
        for(int j = 0; formula[i][j] != 0; j++){ //percorre por cada um dos literais da clausula atual
            int literal = formula[i][j];
            int variavel = valorAbsoluto(literal);
            // se o literal for positivo, e a variavel dele está como verdade, valor_variavel[3] == 1
            // se for negativo e a variavel dele esta falso, o valor_variavel[3] == -1. Ambos sao true
            if((literal > 0 && valor_da_variavel[variavel] == 1) || (literal < 0 && valor_da_variavel[variavel] == -1)){
                clausula_true = 1; break;
            }
        }
        if(clausula_true == 0){
            return 0;
        }
    }
   return 1;
}

//RECURSÃO
//função que tenta todas as combinações possíveis de valores para as variáveis
//nivel_var = qual a variavel da vez (começa com 1,ou seja, variavel 1, depois chama ela +1, ou seja, variavel 2)
int buscar_solucao(int nivel_var){
    //ponto de parada da recursão: quando a variavel a ser avaliada ultrapassar o max de variaveis
    if(nivel_var >  num_variaveis){
        if(analisar_formula() == 1){
            return 1;
        }else{
            return 0;
        }
    }
    valor_da_variavel[nivel_var] = 1; //testar pra true
    if(buscar_solucao(nivel_var + 1)){
        return 1;
    }

    valor_da_variavel[nivel_var] = -1; //testar pra falso
    if(buscar_solucao(nivel_var + 1)){
        return 1;
    }
    valor_da_variavel[nivel_var] = 0;
    return 0;

}


void resolucao(){
if(buscar_solucao(1)){
    printf("\n Sat: \n");
    for(int i = 1; i <= num_variaveis; i++){
        if(valor_da_variavel[i] == 1){
            printf("%d ", i);
        }else{

            printf("%d ", i*(-1));;
        }
    }
    printf("0\n");
}else{
    printf("\n Unsat\n");
}
}


int main( int contador_arg, char *vetor_arg[]){
    // -> contador_arg é pra saber quantos 'pedaços' foram passados na linha de comando.
    // por ex: quando compila: '/programa_sat exemplo_de_teste.cnf, foram 2 pedaços. a variavel guardaria 2.
    // -> vetor_arg guardará quais sao os pedaços, array de string, por isso tem as posições [0] [1]...
    if(contador_arg != 2){
        printf("O correto seria: %s, <seu_arquivo>", vetor_arg[0]);
        return 1;
    } // A quantidade de "pedaços" precisa ser dois, do contrario, ta errado a entrada do arquivo

    leitura_e_obter_formula(vetor_arg[1]);
    valor_da_variavel = (int*)malloc((num_variaveis + 1) * sizeof(int));
    //memoria para o array de estados das variaveis (falso ou true) + 1 por causa do dimacs
    printf("\n | Processando seu arquivo...\n");
    for(int i = 0;  i <= num_variaveis; i++){
        valor_da_variavel[i] = 0; //pra deixar sem valor no inicio, sem falso nem true
    }
    resolucao();
    return 0;

}

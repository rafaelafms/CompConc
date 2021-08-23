//Encontrar o numero de elementos pares, numero de elementos impares e numero de elementos primos de um vetor aleatorio

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include<time.h>
#include"timer.h"

long int dim; //dimensao do vetor de entrada
int nthreads; //numero de threads 
long int verificaGlobal=0; //verifica se todos os elementos dos vetores ja foram processados dentro da tarefa
pthread_mutex_t espera; //controla o uso do verificaGlobal
int *vetor; //vetor de valores a serem comparados

//fluxo executado pelas threads
void * tarefa(void * arg){
    int *parLocal; //variavel local de par, impar e primo
    int primoLocal=0; //auxilia na identificacao dos primos
    long int verificaLocal=0; //controla internamente o while

    //guarda o numero de pares, impares e primos do bloco atual parLocal = par, parLocal+1 = impar, parLocal+2 = primo
    parLocal = (int*) malloc(sizeof(int)*3);
    if (parLocal == NULL){
        fprintf(stderr, "ERRO-- malloc\n"); exit(1);
    }
    
    //primeira execucao
    pthread_mutex_lock(&espera);
    verificaLocal = verificaGlobal;
    verificaGlobal++;
    pthread_mutex_unlock(&espera);

    //verifica os elementos impares, pares e primos de determinado bloco
    while(verificaLocal < dim){
        if(vetor[verificaLocal]%2 == 0){ //verificacao par
            *parLocal +=1; 
            if(vetor[verificaLocal]==2) //verificacao 2 
                *(parLocal+2) +=1;
        }
        else{
            *(parLocal+1) +=1; //impar local
            for(int j=2; j<=sqrt(vetor[verificaLocal]); j++){ //verificacao primo
                if(vetor[verificaLocal]%j==0)
                    primoLocal = 2;
            }
            if(primoLocal!=2)
                *(parLocal+2) +=1;
            primoLocal=0;
        }

        //controle do total de execucoes
        pthread_mutex_lock(&espera);
        verificaLocal = verificaGlobal;
        verificaGlobal++;
        pthread_mutex_unlock(&espera);
    }

    //retorna os numeros de pares, impares e primos locais
    pthread_exit((void *) parLocal);
}

//fluxo principal
int main(int argc, char *argv[]){
    
    int parSeq=0, imparSeq=0, primoSeq=0; //elementos a serem retornados pela forma sequencial
    int parConc=0, imparConc=0, primoConc=0; //elementos a serem retornados pela forma concorrente
    int *retorno; //valor de retorno das threads
    pthread_t *tid; //identificadores das threads 
    
    //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
    if(argc < 3){
        printf("Digite: %s <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1; 
    }
    dim = atoi(argv[1]); 
    nthreads = atoi(argv[2]);

    //aloca o vetor de entrada
    vetor = (int*) malloc(sizeof(int) * dim); 
    if(vetor == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }
    
    //preenche o vetor de entrada com valores inteiros aleatorios
    srand(time(NULL));
    for(long int i=0; i<dim; i++)
        vetor[i] = rand(); 

    //verificacao impar, par e primo sequencial dos elementos 
    int primo=0;
    for(int i=0; i<dim; i++){
        if(vetor[i]%2 == 0){
            parSeq +=1;
            if(vetor[i]==2)
                primoSeq += 1;
        }
        else{
            imparSeq +=1;
   
            //verificacao primo
            for(int j=2; j<=sqrt(vetor[i]); j++){
                if(vetor[i]%j==0)
                    primo = 2;
            }
            if(primo!=2)
                primoSeq +=1;
            primo=0;
        }
    }

    //verificacao impar, par e primo concorrente
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads); //aloca espaco para as threads
    if(tid == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }
   
    //criar as threads
    for(long int i=0; i<nthreads; i++){
        if(pthread_create(tid+i, NULL, tarefa, NULL)){
            printf("ERRO--pthread_create\n");
            return 3;
        }
    }
        
    //aguardar o termino e retorno das threads
    for(long int i=0; i<nthreads; i++){
        if(pthread_join(*(tid+i), (void**) &retorno)){ 
            printf("ERRO--pthread_join\n");
            return 3;
        }
        parConc += *retorno;
        imparConc += *(retorno+1);
        primoConc += *(retorno+2);
        free(retorno); //libera o parLocal da funcao executada pela thread
    } 

    //verificar corretude
    if(parSeq == parConc && imparSeq == imparConc && primoSeq == primoConc){
        printf("--Execucao bem sucedida\n");
        printf("Pares: %d\n", parSeq);
        printf("Impares: %d\n", imparSeq);
        printf("Primos: %d\n", primoSeq);
    }
    else{
        printf("--Problema na execucao\n");
        printf("Pares Seq: %d\n", parSeq);
        printf("Impares Seq: %d\n", imparSeq);
        printf("Primos Seq: %d\n", primoSeq);
        printf("Pares Conc: %d\n", parConc);
        printf("Impares Conc: %d\n", imparConc);
        printf("Primos Conc: %d\n", primoConc);
    }

    //libera as areas de memoria alocadas
    free(vetor);
    free(tid);

    return 0;
}

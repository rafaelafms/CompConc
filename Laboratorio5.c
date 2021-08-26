#include<pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>

/* Variaveis globais */
int bloqueadas = 0; //contador de threads bloqueadas
int nthreads, dim; //numero de threads e tamanho do vetor (iguais)
int *vetor; //vetor com valores aleatorios de 1 a 9
pthread_mutex_t x_mutex; //variavel lock
pthread_cond_t x_cond; //variavel de condicao

/* Funcao barreira */
void barreira(int nthreads, long int id){
    pthread_mutex_lock(&x_mutex); //inicio da secao critica
    if(bloqueadas == (nthreads-1)){
        //ultima thread a chegar na barreira
        printf("Desbloqueia: %ld\n", id); //escreve em tela a thread que desbloqueara todas as outras
        pthread_cond_broadcast(&x_cond);
        bloqueadas=0;
    } else {
        printf("Bloqueia: %ld\n", id); //escreve em tela a thread que sera bloqueada
        bloqueadas++;
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

/* Funcao que sera executada pelas threads */
void *tarefa (void *arg){
    long int id = (long int) arg; 
    int somaLocal = 0;

    srand(time(NULL));
    for(int i=0; i<dim; i++){
        for(int j=0; j<dim; j++){
            somaLocal += vetor[j]; //soma todos os elementos do vetor na iteracao i
        }
        printf("Thread %ld soma na iteracao %d\n", id, i);
        barreira(nthreads,id); //aguarda todas as outras threads terminatem sua soma
        vetor[id] = rand()%10; //cada thread gera um novo valor aleatorio e o coloca na posicao do seu id no vetor
        printf("Thread %ld muda o valor na iteracao %d\n", id, i);
        barreira(nthreads,id); //aguarda todas as threads trocarem os valores de seus id's
    }
    vetor[id] = somaLocal; //coloca no vetor o somatorio total acumulado
    printf("Thread %ld soma = %d\n", id, somaLocal); //imprime a soma final de cada thread

    pthread_exit(NULL);
} 

/* Funcao principal */
int main(int argc, char *argv[]){
    pthread_t *tid;
    
    if(argc < 2){
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1; 
    }
    nthreads = atoi(argv[1]); 
    dim = atoi(argv[1]); 

    //aloca o vetor de entrada
    vetor = (int*) malloc(sizeof(int) * dim); 
    if(vetor == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }
    
    //preenche o vetor de n posicoes com valores aleatorios de 0 a 9
    srand(time(NULL));
    for(long int i=0; i<dim; i++){
        vetor[i] = rand() % 10; 
    }

    //aloca espaco para as n threads
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads); 
    if(tid == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }

    //criacao das n threads
    for(long int i=0; i<nthreads; i++){
        if(pthread_create(tid+i, NULL, tarefa, (void *) i)){
            printf("ERRO--pthread_create\n");
            return 3;
        }
    }

    //aguardar o termino das threads
    for(long int i=0; i<nthreads; i++){
        if(pthread_join(*(tid+i), NULL)){ 
            printf("ERRO--pthread_join\n");
            return 3;
        }
    }

    //compara as somas
    for(int i=1; i<dim; i++){
        if(vetor[i-1] != vetor[i]){
            printf("Valores diferentes %d %d\n", vetor[i], vetor[i-1]);
            return -1;
        }
    }
    printf("Todos os valores são iguais = %d\n", vetor[0]);

    free(vetor);
    free(tid);
   
    return 0;
}

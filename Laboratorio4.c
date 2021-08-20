/* Atividade 4 lab 4
Imprimir threads 1,2,3,4 nas ordens 2134 ou 2314 */

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#define NTHREADS 4

/* Variaveis globais */
int b=0, ad=0; //b = numero de vezes que escrevi a thread 2 ad = numero de vezes que escrevi a thread 1 e 4
pthread_mutex_t x_mutex; //controle do lock
pthread_cond_t b_cond,ad_cond; //controle das variaveis de condicoes (b para a thread a e d)(ad para a thread c)

/* Thread A = 1*/
void *A (void *t){

    pthread_mutex_lock(&x_mutex);
    if(b==0){ //bloqueia caso a thread b ("Seja bem-vindo") ainda nao tenha sido executada
        pthread_cond_wait(&b_cond,&x_mutex);
    }
    printf("Fique a vontade.\n"); //escrita da thread a 
    ad++; //controle para que a thread c ("Volte sempre") possa executar
    if(ad==2){ //caso a thread a e d ja tenham executado, libero a c ("Volte sempre")
        pthread_cond_signal(&ad_cond);
    }
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}


/* Thread B = 2*/
void *B (void *t){

    printf("Seja bem-vindo!\n"); //primeira a executar, ja escreve na chegada

    pthread_mutex_lock(&x_mutex); 
    b++; //controle para que as threads a e d ("Fique a vontade" e "Sente-se por favor") possam executar
    pthread_cond_broadcast(&b_cond); //libero a e d, caso estejam bloqueadas
    pthread_mutex_unlock(&x_mutex);

    pthread_exit(NULL);
}

/* Thread C = 3*/
void *C (void *t){

    pthread_mutex_lock(&x_mutex);
    if(ad<2){ //bloqueio c caso a e d ("Fique a vontade" e "Sente-se por favor") ainda nao tenham executado
        pthread_cond_wait(&ad_cond,&x_mutex);
    }
    printf("Volte sempre!\n"); //escrita da thread c
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}

/* Thread D = 4 */
void *D (void *t){

    pthread_mutex_lock(&x_mutex);
    if(b==0){ //bloqueia caso a thread b ("Seja bem-vindo") ainda nao tenha sido executada
        pthread_cond_wait(&b_cond,&x_mutex);
    }
    printf("Sente-se por favor.\n"); //ecrita da thread d
    ad++; //controle para que a thread c ("Volte sempre") possa executar
    if(ad==2){ //caso a thread a e d ja tenham executado, libero a c ("Volte sempre")
        pthread_cond_signal(&ad_cond);
    }
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}

/* Funcao Principal */
int main(int argc, char *argv[]){
    int i;
    pthread_t threads[NTHREADS];

    /* Inicaliza o mutex (lock de exclusao mutua) e as variaveis de condicao */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init(&b_cond, NULL);
    pthread_cond_init(&ad_cond, NULL);

    /* Cria as threads */
    pthread_create(&threads[0], NULL, A, NULL);
    pthread_create(&threads[1], NULL, B, NULL);
    pthread_create(&threads[2], NULL, C, NULL);
    pthread_create(&threads[3], NULL, D, NULL);

    /* Espera todas as threads completarem */
    for(i=0; i<NTHREADS; i++){
        pthread_join(threads[i], NULL);
    }

    /* Desaloca as variaveis e termina */
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&b_cond);
    pthread_cond_destroy(&ad_cond);

    return 0;
}

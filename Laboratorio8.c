/* Codigo: Laboratorio 8 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4

// Variaveis globais
int intermediarias = 0;      //variavel para o controle das frases intermediarias
sem_t condt2, condt3;     //semaforos para sincronizar a ordem de execucao das threads

//funcao executada pela thread 2
void *t1 (void *arg) {
  printf("Seja bem-vindo!\n");
  sem_post(&condt2); //permite que t2 e t3 executem
  pthread_exit(NULL);
}

//funcao executada pela thread 4
void *t2 (void *arg) {
  sem_wait(&condt2); //espera t1 executar
  printf("Sente-se por favor.\n");
  intermediarias++;
  sem_post(&condt2); //permite que t3 execute
  if(intermediarias == 2) //libera t4 caso t2 e t3 ja tenham executado
    sem_post(&condt3);
  pthread_exit(NULL);
}

//funcao executada pela thread 1
void *t3 (void *threadid) {
  sem_wait(&condt2); //espera t1 executar
  printf("Fique a vontade\n");
  intermediarias++;
  sem_post(&condt2); //permite que t2 execute
  if(intermediarias == 2) //libera t4 caso t2 e t3 ja tenham executado
    sem_post(&condt3);
  pthread_exit(NULL);
}

//funcao executada pela thread 3
void *t4 (void *threadid) {
  sem_wait(&condt3); //espera t2 e t3 executarem
  printf("Volte sempre!\n");
  pthread_exit(NULL);
}

//funcao principal
int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];
  int *id[4], t;

  for (t=0; t<NTHREADS; t++) {
    if ((id[t] = malloc(sizeof(int))) == NULL) {
       pthread_exit(NULL); return 1;
    }
    *id[t] = t+1;
  }

  //inicia os semaforos
  sem_init(&condt2, 0, 0);
  sem_init(&condt3, 0, 0);

  //cria as tres threads
  if (pthread_create(&tid[0], NULL, t3, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); } //criacao da thread 1
  if (pthread_create(&tid[1], NULL, t1, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); } //criacao da thread 2
  if (pthread_create(&tid[2], NULL, t4, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); } //criacao da thread 3
  if (pthread_create(&tid[3], NULL, t2, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); } //criacao da thread 4

  //--espera todas as threads terminarem
  for (t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
    free(id[t]);
  } 
  pthread_exit(NULL);
}

#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool>
#include <unistd.h>
#include <SDL.h>
#include "fractal.h"
#include <semaphore.h>
#include <pthread.h>
#define NTHREAD 3// a changer avec le nombre max de thread qu'on peut avoir!!!!!

TEST;

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
sem_t empty1;
sem_t full1;
sem_t empty2;
sem_t full2;
pthread_mutex_init (&mutex1, NULL);
pthread_mutex_init (&mutex2,NULL);
sem_init (&empty1,0,N);// N a discuter
sem_init (&full1,0,0);
sem_init (&empty2,0,N);//N a discuter
sem_init (&full2,0,0);
int le_plus_grand=INT_MIN;

//typedef struct tab{
////  struct fractal *tableau_fract;
//} tab_t;


struct fractal *tab1[20];//20? peut etre a changer!!!
//tab1->value=0;//place dans le tableau a laquelle il faut placer la prochaine fractal dont il faut calculer la valeur des pixels.
struct fractal *tab2[20];
//tab2->value=0;//place dans le tableau a laquelle il faut placer la prochaine fractal dont la valeur des pixels est deja calculee.







//producteur
void producer (void){
  while(true){//a changer en while(le fichier a lire contient des trucs)
    int r=open(filename,O_RDONLY);//attention, il faut surement mettre le filename en argument
    if(r<0){
      return -1;
    }
    char caracter;
    int ret=read(r,&caracter,sizeof(char));
    if(ret<0){
      return (EXIT_FAILURE);
    }
    while(caracter==' '){
      for(int w=0;caracter!='\n';w++){//pas certain du tt!! (\n est 2 caractère nn?)
        ret=read(r,&caracter,sizeof(char));
        if(ret<0){
          return (EXIT_FAILURE);
        }
        if(ret==0){
          //c'est la fin du fichier
        }
      }
      ret=read(r,&caracter,sizeof(char));
    }
    while(caracter=='#'){
      for(int w=0;caracter!='\n';w++){//pas certain du tt!! (\n est 2 caractère nn?)
      ret=read(r,&caracter,sizeof(char));
      if(ret<0){
        return (EXIT_FAILURE);
      }
      if(ret==0){
        //c'est la fin du fichier
      }
    }
    ret=read(r,&caracter,sizeof(char));
    }
    char nom[65];
    while(caracter!=' '){
      strcpy(nom, &caracter);//A TESTER!!!!
      ret=read(r,&caracter,sizeof(char));
      if(ret<0){
        return (EXIT_FAILURE);
      }
    }
    ret=read(r,&caracter,sizeof(char));// a faire car il faut passer au caractère suivant
    if(ret<0){
      return (EXIT_FAILURE);
    }
    char la_largeur[200];//a discuter j'ai mis 200 en ne sachant pas la longeure que pouvait avoir la longeur et la la_largeur mais on parle de 32 bits
    while(caracter!=' '){
      strcpy(la_largeur, &caracter);
      ret=read(r,&caracter,sizeof(char));
      if(ret<0){
        return (EXIT_FAILURE);
      }
    }
    int la_largeur2=atoi(la_largeur);
    ret=read(r,&caracter,sizeof(char));// a faire car il faut passer au caractère suivant
    if(ret<0){
      return (EXIT_FAILURE);
    }
    char la_longeur[200];
    while(caracter!=' '){
      strcpy(la_longeur, &caracter);
      ret=read(r,&caracter,sizeof(char));
      if(ret<0){
        return (EXIT_FAILURE);
      }
    }
    int la_longeur2=atoi(la_longeur);
    ret=read(r,&caracter,sizeof(char));
    if(ret<0){
      return (EXIT_FAILURE);
    }
    char reel[200];
    while(caracter!=' '){
      strcpy(reel,&caracter);
      ret=read(r,&caracter,sizeof(char));
      if(ret<0){
        return (EXIT_FAILURE);
      }
    }
    float reel2=atof(reel);//transformer un char en float:atof
    ret=read(r,&caracter,sizeof(char));
    if(ret<0){
      return (EXIT_FAILURE);
    }
    char imaginaire[200];
    while(caracter!=' '){
      strcpy(imaginaire,&caracter);
      ret=read(r,&caracter,sizeof(char));
      if(ret<0){
        return (EXIT_FAILURE);
      }
    }
    float imaginaire2=atof(imaginaire);



    //fin de la lecture d'une fractal.









    struct fractal *fractal_a_creer;
    int place;
    fractal_a_creer=fractal_new(nom,la_largeur2,la_longeur2,(double)reel2,(double)imaginaire2);
    sem_wait(&empty1);
    pthread_mutex_lock (&mutex1);
    if(sem_getvalue(&full1,&place)!=0){
      return (EXIT_FAILURE);
    }


    tab1[place]=fractal_a_creer;
    //tab1->value=tab1->value+1;
    pthread_mutex_unlock(&mutex1);
    sem_post(&full1);
  }
}


void proconsumer(void){
  struct fractal *fractal_a_calculer;
  struct fractal *fractal_a_free;
  int place;
  while(true){//a changer en while(le producer a fini && le tableau est vide)
    sem_wait(&full1);
    pthread_mutex_lock(&mutex1);
    if(sem_getvalue(&full1,&place)!=0){
      return (EXIT_FAILURE);
    }
    fractal_a_free=tab1[place];
    fractal_a_calculer=fractal_new(fractal_a_free->name,fractal_a_free->width,fractal_a_free->height,fractal_a_free->a,fractal_a_free->b);
    free(fractal_a_free);
    //tab1->value=tab1->value-1;  //prend une fractal + decremente la valeur du tableau et remplace l'endoit du tableau dont on a pri la fract par NULL
    pthread_mutex_unlock(&mutex1);
    sem_post (&empty1);
    for(int largeur=0;largeur<*(fractal_a_calculer->height);largeur++){
      for(int longueur=0;longeur<*(fractal_a_calculer->width);longeur++){
        fractal_compute_value (fractal_a_calculer,longeur,largeur);//ATTENTION REVOIE UN INT
      }
    }//fin du calcul des fractal.
    sem_wait(&empty2);
    pthread_mutex_lock(&mutex2);
    if(sem_getvalue(&full2,&place)!=0){
      return (EXIT_FAILURE);
    }
    tab2[place]=fractal_a_calculer;
    //tab2->value=tab2->value+1;
    pthread_mutex_unlock(&mutex2);
    sem_post(&full2);

  }
}
void consumer (void){
  struct fractal *fractal_moyenne;
  struct fractal *fractal_a_free;
  struct fractal *fractal_a_conserver;
  int *b;
  int place;
  while(true){//a changer en while(le proconsummer a fini && le tableau est vide)
    sem_wait(&full2);
    pthread_mutex_lock(&mutex2);
    if(sem_getvalue(&full2,&place)!=0){
      return (EXIT_FAILURE);
    }
    fractal_a_free=tab2[place];
    fractal_moyenne=fractal_new(fractal_a_free->name,fractal_a_free->width,fractal_a_free->height,fractal_a_free->a,fractal_a_free->b);
    b=fractal_a_free->value;//obligé de faire ca car fractal_new ne prend pas en compte le tableau value (ici, hyper important)
    fractal_moyenne->value=b;
    free(fractal_a_free);
    //tab2->value=tab2->value-1;//prend une fractal dont il faut calculer la moyenne +decremente et remplace l'endroit du tableau dont on a pris la fract par NULL
    pthread_mutex_unlock(&mutex2);
    sem_post(&empty2);
    b=fractal_moyenne->value;
    int max_tableau=fractal_moyenne->width * fractal_moyenne->height;
    int a_comparer;

    for(int i=0;i<max_tableau;i++){
      a_comparer=a_comparer+*(b+i);
    }
    a_comparer=a_comparer/(max_tableau);//a cheker si on mettrait pas plutot un double et une division plus précise.
    if(a_comparer>le_plus_grand){
      le_plus_grand=a_comparer;
      free(fractal_a_conserver);
      fractal_a_conserver=fractal_moyenne;
    }
    else{
      free(fractal_moyenne);
    }

  }

}

int main (int argc, char *argv[]){
  int prod=NTHREAD/3;//a voir car pour moi il faudra peut etre mettre plus de thread 'proco' car ils ont bcp de travail donc seront plus lent.
  pthread_t thread_pro[prod];//producteur qui lit les fractal et les met dans un tableau
  pthread_t thread_proco[prod];//producteur et consommateur en mm tps (consomme les fractal et calcule leur valeur et les met dans deuxieme tableau)
  pthread_t thread_co[prod];//consommateur final qui compare les valeur calculee
  int err;
  int i=0;
  while (i<prod) {//permet de créer un producteur puis un consommateur et pas tt les prod puis tt les conso.
    err=pthred_create(&(thread_pro[i]),NULL,producer,NULL);
    if(err!=0){
      error(err,"pthred_create");
    }
    err=pthred_create(&(thread_proco[i]),NULL,proconsumer,NULL);
    if(err!=0){
      error(err,"pthread_create");
    }
    err=pthread_create(&(thread_co[i]),NULL,consumer,NULL);
    if(err!=0){
      error(err,"pthread_create");
    }

    i++;
  }


}

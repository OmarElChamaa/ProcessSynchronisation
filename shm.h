// Fichier shm.h à rédiger
#include "asem.h" 
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define NOMSHM "vaccinodromeOmar"
#define PRISCHARGE -2 
#define SIEGELIB -1

#define CHECK(call) do {if (call == -1){perror(#call); \
    exit(EXIT_FAILURE);}} while (0)

void raler(char *msg);

typedef struct box{
    int nbBox ;
    int nMedecin ;
    char nom[10];
}box;




typedef struct vaccinodrome{
    int m ; // medecin dans le vacc 
    int mMax ; //capacite max medecin
    int n ; //n sieges 
    int t ; //temps vaccination
    int patientsEnAttente ;
    int ouvert ; //ouvert si 1 ferme si 0 
    int sommePatients ;
    // asem_t ouverture ; 
    // asem_t arriveeM ;
    asem_t placeLibre ;
    asem_t patientAttend ;
    asem_t ecritureVax ; 
    asem_t ecritureSalle ;
    asem_t fermeture ; 
} vaccinodrome ; 



typedef struct chaise{
    int numPatient ; 
    char nomPatient[10];
    asem_t finVaccination ; 
    int medecin ;
}chaise;
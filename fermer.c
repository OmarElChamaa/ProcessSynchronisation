#include "shm.h"


// Fichier fermer.c à rédiger
int main (int argc, char * argv[]) {

    if(ainit(argv[0])==-1){
        raler("ainit \n");
    }
    // il faut fermer signal  
    if(argc>1){
        
        adebug(DBG_SYNC,"usage: %d arg \n",argc-1);
        exit(EXIT_FAILURE);
    }

    int fd  = shm_open(NOMSHM,O_RDWR,0666);
    if(fd<0){
        adebug(DBG_SYNC,"fd \n");
        exit(EXIT_FAILURE);
    }
    
    struct stat buf;
    CHECK(fstat(fd, &buf));
    printf("size is %ld \n",buf.st_size);
    vaccinodrome * vax;

    if ((vax = mmap(NULL, buf.st_size, PROT_WRITE | PROT_READ, 
        MAP_SHARED, fd, 0)) == MAP_FAILED){
            
        adebug(DBG_SYNC,"map \n");
        exit(EXIT_FAILURE);
    }


    chaise * tabSalleAttente ; 
    tabSalleAttente = (chaise *)(vax+vax->mMax+1);

    
    
    vax->ouvert = 0 ;

    for(int i=0;i<vax->m;i++){ 
        CHECK(asem_post(&vax->patientAttend)); 
    }
    printf("vax m is %d",vax->m);
    for(int i=0;i<vax->m;i++){ // pour s'assurer que tout les medecins ont quitte avant de fermer 
        CHECK(asem_wait(&vax->fermeture));  
    }
    printf("Tous les medecins ont quittes \n");

    //on commence la fermeture pas besoin de synchro comme c'est le dernier prog qui tourne
    for(int i= 0 ; i < vax->n ; i++ ){
        CHECK(asem_destroy(&tabSalleAttente[i].finVaccination));
    }
    CHECK(asem_destroy(&vax->ecritureVax));
    CHECK(asem_destroy(&vax->fermeture));
    // CHECK(asem_destroy(&vax->ouverture));
    CHECK(asem_destroy(&vax->patientAttend));
    CHECK(asem_destroy(&vax->placeLibre));
    


    CHECK(munmap(vax,buf.st_size));

    CHECK(shm_unlink(NOMSHM));
    


    return 0 ;
}

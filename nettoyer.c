// Fichier nettoyer.c à rédiger
#include "shm.h"

int main (int argc, char *argv []) {
    
    (void) argv ;
    if(ainit(argv[0])==-1){
        raler("ainit \n");
    }
    if(argc>1){
        
        adebug(DBG_SYNC,"usage: %d arg \n",argc-1);
        exit(EXIT_FAILURE);
    }
    
    int fd=shm_open(NOMSHM,O_RDWR,0);
    
    if(fd<0){
        adebug(DBG_SYNC,"pas de fd rien a nettoyer\n");
        exit(EXIT_SUCCESS);
    }

    struct stat buf;
    CHECK(fstat(fd, &buf));
    vaccinodrome * vax;
    

    printf("size map is %ld \n",buf.st_size);
    if(buf.st_size == 0){
        adebug(DBG_SYNC,"echec nettoyer \n");
        exit(EXIT_SUCCESS);
    }else{
            if ((vax = mmap(NULL, buf.st_size, PROT_WRITE | PROT_READ, 
            MAP_SHARED, fd, 0)) == MAP_FAILED){
                
            adebug(DBG_SYNC,"map \n");
            exit(EXIT_FAILURE);
        }
    }
    chaise * tabSalleAttente ; 
    tabSalleAttente = (chaise *)(vax+vax->mMax+1);

    for(int i= 0 ; i < vax->n ; i++ ){
        CHECK(asem_destroy(&tabSalleAttente[i%vax->n].finVaccination));
    }

    CHECK(asem_destroy(&vax->ecritureVax));
    CHECK(asem_destroy(&vax->fermeture));
   // CHECK(asem_destroy(&vax->ouverture));
    CHECK(asem_destroy(&vax->patientAttend));
    CHECK(asem_destroy(&vax->placeLibre));
    
    CHECK(munmap(vax,buf.st_size));
    CHECK(shm_unlink(NOMSHM));

}

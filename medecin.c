// Fichier medecin.c à rédiger
#include "shm.h"
int main (int argc, char *argv []) {
    

    if(ainit(argv[0])==-1){
        raler("ainit \n");
    }

    if(argc>1){
        
        adebug(DBG_SYNC,"usage: %d arg \n",argc-1);
        exit(EXIT_FAILURE);
    }

    int fd  = shm_open(NOMSHM, O_RDWR,0666);
    if(fd<0){
        adebug(DBG_SYNC,"fd \n");
    }
    
    struct stat buf;
    CHECK(fstat(fd, &buf));
    
    vaccinodrome * vax;

    if ((vax = mmap(NULL, buf.st_size, PROT_WRITE | PROT_READ, 
        MAP_SHARED, fd, 0)) == MAP_FAILED){
            
        adebug(DBG_SYNC,"map \n");
        exit(1);
    }

    box * tabBox ;

    tabBox = (box  * ) (vax + 1);

    //zone crit !!!!!!!!!!!!!!!!!!!!!!
    CHECK(asem_wait(&vax->ecritureVax));
    if(vax->m==vax->mMax){
        CHECK(asem_post(&vax->ecritureVax));
        CHECK(asem_post(&vax->placeLibre));
        exit(EXIT_FAILURE);
    }
    int nChaises = vax->n ; 
    int nMedecin = vax->m;
    vax->m++;
    tabBox[nMedecin].nMedecin = nMedecin;
    printf("Le medecin %d prend le box %d \n",nMedecin,nMedecin);

    if (vax->ouvert==0){
        printf("Vaccinodrome ferme, je rentre \n");
        CHECK(munmap(vax,buf.st_size));
        exit(EXIT_SUCCESS);
    }

    CHECK(asem_post(&vax->ecritureVax));

    chaise * tabSalleAttente ; 
    tabSalleAttente = (chaise *)(vax+vax->mMax+1);

    //zone crit !!!!!!!!!!!!!!!!!!!!!!
    //check si patient a vacciner 
    
    
    for(int i = 0 ; i<nChaises ;){
        if(i==0){
            CHECK(asem_wait(&vax->patientAttend));
        }
        if(vax->ouvert==0 ){
            printf("Le vaccinodrome ferme, je finit de vacciner \n");
            if(vax->patientsEnAttente == 0 ){
                printf("il ne reste plus de patients, je rentre \n");
                CHECK(asem_post(&vax->ecritureSalle));   
                CHECK(asem_post(&vax->fermeture));
                CHECK(munmap(vax,buf.st_size));
                exit(EXIT_SUCCESS);
            }
        }
        
        CHECK(asem_wait(&vax->ecritureSalle));
        
        if(tabSalleAttente[i].numPatient > -1 && 
            tabSalleAttente[i].medecin <0){ // pour eviter que 2 medecins traitent le meme patient 
            char * nomTmp; //pour pouvoir liberer rapidement la place
            nomTmp=tabSalleAttente[i].nomPatient;
            tabSalleAttente[i].medecin = nMedecin;
            
            strcpy(tabBox[nMedecin].nom,nomTmp);
            
            printf("medecin %d vaccine %s\n",nMedecin,nomTmp);

                
            CHECK(asem_post(&vax->ecritureSalle));   
            usleep(vax->t*1000);
            
            //annonce la fin de vaccination du patient 
            CHECK(asem_post(&tabSalleAttente[i].finVaccination));
            i=0;
        }else{
            CHECK(asem_post(&vax->ecritureSalle)); 
            i=(i+1)%(nChaises);    
        }

        if(vax->ouvert==0 ){
            printf("Le vaccinodrome ferme, je finit de vacciner \n et je rentre chez moi \n");
            if(vax->patientsEnAttente == 0 ){
                printf("il ne reste plus de patients, je rentre \n");
                CHECK(asem_post(&vax->ecritureSalle));   
                CHECK(asem_post(&vax->fermeture));
                CHECK(munmap(vax,buf.st_size));
                exit(EXIT_SUCCESS);
            }
        }
        
    }
    CHECK(asem_post(&vax->fermeture));
    CHECK(munmap(vax,buf.st_size));
    exit(EXIT_SUCCESS);
}

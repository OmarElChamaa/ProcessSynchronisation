// Fichier patient.c à rédiger
#include "shm.h"

int main (int argc, char *argv []) {
    

    if(ainit(argv[0])==-1){
        raler("ainit \n");
    }

    if(argc!=2){
        
        adebug(DBG_SYNC,"usage: %d arg \n",argc-1);
        exit(EXIT_FAILURE);
    }

    char * nomPatient = argv[1];
    if(strlen(nomPatient)>10){
        adebug(DBG_SYNC,"usage: nom = %d octets ! \n",strlen(nomPatient));
        exit(EXIT_FAILURE);
    }
    if(strlen(nomPatient)==0){
        adebug(DBG_SYNC,"usage: nom vide ! \n");
        exit(EXIT_FAILURE);
    }

    int fd ; 


    fd  = shm_open(NOMSHM,O_RDWR,0666);
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


    CHECK(asem_wait(&vax->placeLibre));
     
    
    if(vax->ouvert==0){
        printf("Vaccinodrome ferme, je rentre chez moi \n");
        CHECK(asem_post(&vax->placeLibre));
        CHECK(munmap(vax,buf.st_size));
        exit(EXIT_FAILURE);
    }
    
   
    if((vax->n - vax->patientsEnAttente)>0){//place libre verification malgres la semaphore
        chaise * tabSalleAttente ; 
        tabSalleAttente = (chaise *)(vax+vax->mMax+1);

        // box * tabBox ;

        // tabBox = (box  * ) (vax + 1);

        //trouve une chaise de libre
        for (int i = 0; i < vax->n; i++)
        {   
            CHECK(asem_wait(&vax->ecritureSalle));
            if(tabSalleAttente[i].numPatient  == -1){//place vide !

                strcpy(tabSalleAttente[i].nomPatient,nomPatient);
                
                tabSalleAttente[i].numPatient = vax->sommePatients + 1 ;
                
                printf("patient %s siege %d\n",nomPatient,i);
                vax->sommePatients++ ;
                vax->patientsEnAttente++ ;

                CHECK(asem_post(&vax->ecritureSalle));
                //signale un patient en attente 
                CHECK(asem_post(&vax->patientAttend));

                //attend la fin de sa vaccination
                CHECK(asem_wait(&tabSalleAttente[i].finVaccination));
                
                printf("patient %s medecin %d\n",nomPatient,tabSalleAttente[i].medecin);

                CHECK(asem_wait(&vax->ecritureSalle));
                

                //libere son siege ;
                vax->patientsEnAttente -- ;
                tabSalleAttente[i].medecin = -1 ;
                tabSalleAttente[i].numPatient = -1 ;
                strcpy(tabSalleAttente[i].nomPatient,""); 

                CHECK(asem_post(&vax->ecritureSalle));
                

                //signale une place libre
                CHECK(asem_post(&vax->placeLibre));
               
                CHECK(munmap(vax,buf.st_size));
                exit(EXIT_SUCCESS);
            }else{
                CHECK(asem_post(&vax->ecritureSalle));
                
            }
        }
        
    }
    
}

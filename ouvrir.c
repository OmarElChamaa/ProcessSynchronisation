#include "shm.h"



void init_Salle(chaise * c,int nbrSieges){
     for(int i= nbrSieges ; i > -1  ; i-- ){
         
        asem_t finVaccination ;
        CHECK(asem_init(&finVaccination,"finVaccination",1,0));

        c[i%nbrSieges].numPatient = -1;
        c[i%nbrSieges].finVaccination= finVaccination;
    }
}

// vaccinodrome * init_asem_vaccinodrome(vaccinodrome * vax){

//     asem_t  ouverture ; //ouverture vaccinodrome
//     CHECK(asem_init(&ouverture,"ouverture", 1, 0));

//     asem_t arriveeM ; // arrivee medecin
//     CHECK(asem_init(&arriveeM,"arriveeM",1,0));

//     asem_t  places ; //si une place est libre dans le vaccinodrome
//     CHECK(asem_init(&places,"places", 1, 1));

//     asem_t patientAttend ;
//     CHECK(asem_init(&patientAttend,"patientAttend",1,0));


//     asem_t ecritureVax ;
//     CHECK(asem_init(&ecritureVax,"ecritureVax",1,1));

//     asem_t ecritureSalle ;
//     CHECK(asem_init(&ecritureSalle,"ecritureSalle",1,1));

//     asem_t fermeture ;
//     CHECK(asem_init(&fermeture,"fermeture",1,0));  

//     vax->ouverture = ouverture;
//     vax->arriveeM = arriveeM;
//     vax->placeLibre = places ;
//     vax->patientAttend = patientAttend ;
//     vax->ecritureVax = ecritureVax ;
//     vax->ecritureSalle = ecritureSalle ;
//     vax->fermeture = fermeture ; 
//     return vax ;
// }





int main (int argc, char *argv []) {
    

    if(ainit(argv[0])==-1){
        raler("ainit \n");
    }

    if(argc!=4){
        printf("usage: %d args\n",argc-1);
        exit(EXIT_FAILURE);
    }


    

    int nbrSieges = atoi(argv [1]) ; 
    if(nbrSieges<0 || nbrSieges == 0){
        adebug(DBG_SYNC,"usage: n=%d \n",nbrSieges);
        exit(EXIT_FAILURE);
    }
    

    int mMax=atoi(argv[2]);
    if(mMax<0 || mMax == 0){
        adebug(DBG_SYNC,"usage: m= %d\n",mMax);
        exit(EXIT_FAILURE);
    }
    

    int t = atoi(argv[3]);
    if(t<0){
        adebug(DBG_SYNC,"usage: t= %d\n",t);
        exit(EXIT_FAILURE);
    }

    int fd ; 

    fd  = shm_open(NOMSHM,O_CREAT| O_RDWR,0666);
    
    struct stat buf;
    CHECK(fstat(fd, &buf));

    if(buf.st_size!=0){
        adebug(DBG_SYNC,"segment memoire non vide, je n'ouvre pas \n");
        exit(EXIT_FAILURE);
    }

    if(fd<0){
        adebug(DBG_SYNC,"fd \n");
        exit(EXIT_FAILURE);
    }

    // asem_t  ouverture ; //ouverture vaccinodrome
    // CHECK(asem_init(&ouverture,"ouverture", 1, 0));
    
    // asem_t arriveeM ; // arrivee medecin
    // CHECK(asem_init(&arriveeM,"arriveeM",1,0));

    asem_t  places ; //si une place est libre dans le vaccinodrome
    CHECK(asem_init(&places,"places", 1, nbrSieges));

    asem_t patientAttend ;
    CHECK(asem_init(&patientAttend,"patientAttend",1,0));


    asem_t ecritureVax ;
    CHECK(asem_init(&ecritureVax,"ecritureVax",1,1));

    asem_t ecritureSalle ;
    CHECK(asem_init(&ecritureSalle,"ecritureSalle",1,1));

    asem_t fermeture ;
    CHECK(asem_init(&fermeture,"fermeture",1,0));

    vaccinodrome * vax; 

    
    
    size_t size = sizeof(vax); 
    

    size_t sizeTotal = size + mMax * sizeof(box) + 
    nbrSieges * sizeof(chaise) ; 

    if(ftruncate(fd,sizeTotal)==-1){
       adebug(DBG_SYNC,"truncate \n");
       exit(EXIT_FAILURE);
    }
    

    if ((vax = mmap(NULL, sizeTotal, PROT_WRITE | PROT_READ, 
        MAP_SHARED, fd, 0)) == MAP_FAILED){
            
        adebug(DBG_SYNC,"map \n");
        CHECK(shm_unlink(NOMSHM));
        exit(EXIT_FAILURE);
    }

    vax->m=0;
    vax->mMax=mMax ; 
    vax->n=nbrSieges;
    vax->t=t;
    vax->ouvert = 1 ;
    vax->patientsEnAttente=0;
    vax->sommePatients=0;

    //vax->ouverture = ouverture;
    //vax->arriveeM = arriveeM;
    vax->placeLibre = places ;
    vax->patientAttend = patientAttend ;
    vax->ecritureVax = ecritureVax ;
    vax->ecritureSalle = ecritureSalle ;
    vax->fermeture = fermeture ; 
    

    box * tabBox ;

    tabBox = (box  * )(vax + 1);
    
    //init des box
    for(int i = 0 ; i<mMax ; i++){
        tabBox[i].nbBox = i ;
        tabBox[i].nMedecin = -1;
    }

    chaise * tabSalleAttente ; 
    tabSalleAttente = (chaise *)(vax+vax->mMax+1);

    
    for(int i= nbrSieges ; i >= 0  ; i-- ){
        asem_t finVaccination ;
        CHECK(asem_init(&finVaccination,"finVaccination",1,0));
        printf("i est %d \n",i);

        tabSalleAttente[i].numPatient = -1;
        tabSalleAttente[i].medecin=-1;
        tabSalleAttente[i].finVaccination= finVaccination;
    }

   // CHECK(asem_post(&vax->ouverture));
    //CHECK(munmap(vax,buf.st_size));
    
    return 0 ;
}

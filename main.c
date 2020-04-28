#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define MS 1000

/* Food*/
typedef struct {
    char name[50];
    int prepTime;
} Food;

typedef struct{
    pthread_mutex_t timemux;
    int currentId;
}CurrentClient_mux;

/*Client*/
typedef struct {
    int id;
    Food choice;
    int tolerance;
    int served;
    int orderReady;
    CurrentClient_mux *currentClient;
} Client;

typedef struct {
    pthread_mutex_t atender , cobrar;
} SplitMutex;

/*Local*/
typedef struct {
    Food *menu;
    Client *clients;
    int cantClientes;
    pthread_t *client_threads;
    SplitMutex *smutex; 
    CurrentClient_mux *currentClient;
}FoodPlace;



/*Threads*/
void *streetThread(void *);
void *clientThread(void *);
void *cashierThread(void *);

/*Menu Functions*/
Food *menuSetup();
Food pickFood(Food * menu);
int getMaxWaitTime(Food *menu);

void atender(SplitMutex *smutex, Client *cliente);
void cobrar(SplitMutex *smutex, Client *cliente);



int main(){
    srand(time(NULL));

    /*Crear local*/
    FoodPlace mercadoChino;
    mercadoChino.clients = calloc(50, sizeof(Client));
    mercadoChino.menu = menuSetup();
    mercadoChino.smutex = calloc(1, sizeof(SplitMutex));
    mercadoChino.client_threads = calloc(50,sizeof(pthread_t));
    mercadoChino.currentClient = calloc(1,sizeof(pthread_mutex_t));

    pthread_mutex_init(&mercadoChino.smutex->atender, NULL);
    pthread_mutex_init(&mercadoChino.smutex->cobrar, NULL);
    pthread_mutex_unlock(&mercadoChino.smutex->cobrar);
    pthread_mutex_lock(&mercadoChino.smutex->cobrar);
    pthread_mutex_init(&mercadoChino.currentClient->timemux, NULL);
    pthread_mutex_unlock(&mercadoChino.currentClient->timemux);

    /*Crear hilo calle*/
    pthread_t street;
    pthread_create(&street, NULL, streetThread, (void *)&mercadoChino);
    /*Crear hilo cajero*/
    pthread_t cashier;
    pthread_create(&cashier, NULL, cashierThread, (void *)&mercadoChino);
    // pthread_join(mercadoChino.client_threads[19],NULL);
    pthread_join(street,NULL);
    pthread_join(cashier,NULL);

    return 0;
}
/*Devuelve un arreglo del tipo comida con el menu*/
Food* menuSetup(){

    Food *menu = calloc(10, sizeof(Food));

    sprintf(menu[0].name,"Pizza");
    menu[0].prepTime = 50;

    sprintf(menu[1].name,"Lomito");
    menu[1].prepTime = 80;

    sprintf(menu[2].name,"Empanadas");
    menu[2].prepTime = 65;

    sprintf(menu[3].name,"Ensalada");
    menu[3].prepTime = 20;

    sprintf(menu[4].name,"Milanesa");
    menu[4].prepTime = 45;

    sprintf(menu[5].name,"Sushi");
    menu[5].prepTime = 75;

    sprintf(menu[6].name,"Chop Suey");
    menu[6].prepTime = 100;

    sprintf(menu[7].name,"Pollo");
    menu[7].prepTime = 60;

    sprintf(menu[8].name,"Matambre");
    menu[8].prepTime = 100;

    sprintf(menu[9].name,"Murcielago");
    menu[9].prepTime = 80;

    return menu;
}

/*Selecciona una comida del menu*/
Food pickFood(Food *menu){
    return menu[rand()%10];
}

/*Devuelve las toletancias de espera*/
int getMaxWaitTime(Food *menu){

    int min = menu[0].prepTime;

    for(int i = 0; i < 10; i++)
        if(menu[i].prepTime < min)
            min = menu[i].prepTime;

    return min * 4;
}
/*Dispara el hilo calle que genera los hilos cliente*/
void *streetThread(void *arg){

    FoodPlace *mercadoChino = (FoodPlace *)arg;
    
    int cant_max = 20;
    int tolerance = getMaxWaitTime(mercadoChino->menu);
    mercadoChino->cantClientes = 0;

    for(int i = 0; i < cant_max; i++){
        usleep((rand()%3 + 2) * MS);

        Client *client = calloc(1,sizeof(Client));
        client->id = i;
        client->choice = pickFood(mercadoChino->menu);
        client->tolerance = tolerance;
        client->served = 0;
        client->orderReady = 0;

        
        client->currentClient = mercadoChino->currentClient;
        pthread_create(&mercadoChino->client_threads[i], NULL, clientThread, (void *)client);//lleva el tiempo y se habilita o de  sabilita
        mercadoChino->clients[i] = *client;
        mercadoChino->cantClientes = i;   
    }
 

    // for(int j = 0; j < cant_max; j++){
    //     pthread_join(mercadoChino->client_threads[j], NULL);
    // }
    return NULL;
    
}
/*Hilo cliente tiene que esperar la tolerancia o ser atendido*/
void *clientThread(void *arg){
    Client *client = (Client *)arg;

    printf("Client %d in queue\n",client->id+1);
    fflush(stdout);
    

    struct timespec wait;
    clock_gettime(CLOCK_REALTIME, &wait);
    wait.tv_nsec+= client->tolerance * 100000;
    //Si esta libre devuelve 0
    int respuesta;
    respuesta = pthread_mutex_timedlock(&client->currentClient->timemux,&wait);
    if(respuesta == 0){
        printf("Client %d served\n",client->id+1);
        client->currentClient->currentId = client->id;
    }else{
        printf("Client %d left without food\n", client->id+1);
    }

    pthread_exit(NULL);

    return NULL;
}
/*Encargado de tomar clientes y cocinarles*/
void *cashierThread(void *arg){
    // printf("Cajero\n");
    FoodPlace *mercadoChino = (FoodPlace *)arg;
    usleep(100000000);
    while(1){
        pthread_mutex_unlock(&mercadoChino->currentClient->timemux);
        atender(mercadoChino->smutex, &mercadoChino->clients[mercadoChino->currentClient->currentId]);
        cobrar(mercadoChino->smutex, &mercadoChino->clients[mercadoChino->currentClient->currentId]);
        
    }
    return NULL;
}

void atender(SplitMutex *smutex, Client *cliente){
    pthread_mutex_lock(&smutex->cobrar);
    printf("Client %d por atender\n",cliente->id+1);
    int tiempo;
    tiempo = cliente->choice.prepTime;
    cliente->served = 1;
    while(!tiempo){
        usleep(1 * MS);
        tiempo--;
    }
    pthread_mutex_unlock(&smutex->atender); 
}
void cobrar(SplitMutex *smutex, Client *cliente){
    pthread_mutex_lock(&smutex->atender);
    cliente->orderReady = 1;
    printf("Client %d pedido terminado\n",cliente->id+1);
    pthread_mutex_unlock(&smutex->cobrar);
}


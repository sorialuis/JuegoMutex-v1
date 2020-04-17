#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define MS 1000

// Food
typedef struct {
    char name[50];
    int prepTime;
} Food;

// Client
typedef struct {
    int id;
    Food choice;
    int tolerance;
    int served;
    int timeout;
//    pthread_mutex_t mutex_Cashier;
} Client;

// Cashier
typedef struct {
    Client *current;
    pthread_t serveThread;
    pthread_t cookThread;

} Cashier;

//Local

typedef struct {
    Cashier cajero;
    pthread_t *clients;
    Food *menu;
    int *queue;
}FoodPlace;


// Threads

void *streetThread(void *);
void *clientThread(void *);
void *cashierThread(void *);

//Menu Functions
Food *menuSetup();
Food pickFood(Food * menu);
int getMaxWaitTime(Food *menu);

int main(){
    srand(time(NULL));

    int cantidad_cajeros;
    cantidad_cajeros = 1;

    //Crear local
    FoodPlace mercadoChino;
    mercadoChino.queue = calloc(200, sizeof(int));
    mercadoChino.clients = calloc(50, sizeof(pthread_t));
    mercadoChino.menu = menuSetup(); 


    //Crear hilo calle
    pthread_t street;
    pthread_create(&street, NULL, streetThread, (void *)&mercadoChino);

    pthread_t cashier;

    pthread_create(&cashier, NULL, cashierThread, (void *)&mercadoChino);
    pthread_join(cashier,NULL);

    return 0;
}

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

    sprintf(menu[9].name,"Choripan");
    menu[9].prepTime = 80;

    return menu;
}

Food pickFood(Food *menu){
    return menu[rand()%10];
}

int getMaxWaitTime(Food *menu){

    int min = menu[0].prepTime;

    for(int i = 0; i < 10; i++)
        if(menu[i].prepTime < min)
            min = menu[i].prepTime;

    return min * 4 * MS;
}

void *streetThread(void *arg){

    FoodPlace *mercadoChino = (FoodPlace *)arg;

    int cant_max = 20;
    int tolerance = getMaxWaitTime(mercadoChino->menu);

    for(int i = 0; i < cant_max; i++){

        usleep((rand()%300 + 200) * MS);

        Client *client = calloc(1,sizeof(Client));
        client->id = i;
        client->choice = pickFood(mercadoChino->menu);
        client->tolerance = tolerance;
        client->served = 0;

        // pthread_mutex_t mutex_queue;
        pthread_t *client_thread = calloc(1,sizeof(pthread_t));
        pthread_create(client_thread, NULL, clientThread, (void *)client);//lleva el tiempo y se habilita o desabilita
        mercadoChino->clients[i] = *client_thread;

    }
}

void *clientThread(void *arg){
    Client *client = (Client *)arg;

    printf("Client %d in queue\n",client->id+1);
    fflush(stdout);

}

void *cashierThread(void *arg){
    FoodPlace *mercadoChino = (FoodPlace *)arg;

}
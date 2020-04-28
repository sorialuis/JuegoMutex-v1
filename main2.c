#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MS 1000
#define CLIENTS 20


typedef struct {
    char name[50];
    int prepTime;
}Food;

typedef struct {
    pthread_mutex_t mtxCook;
    pthread_mutex_t mtxCheckout;

}Cashier;

typedef struct {
    int id;
    int tolerance;
    Food *order;
    pthread_mutex_t *mtxQueue;
}Client;

typedef struct {
    Food *menu;
    Cashier cashier;

    pthread_mutex_t mtxQueue;
}FoodPlace;


/* Thread functions */
void *streetThread(void *);
void *clientThread(void *);
void *cashierThread(void *);

/* Cashier Functions */
void cook(Client *client);
void checkout(Client *client);

/* Menu Functions */
Food *menuSetup();
Food *pickFood(Food * menu);
int getMaxWaitTime(Food *menu);

int main(){
    srand(time(0));

    FoodPlace *mercadoChino = (FoodPlace *)calloc(1,sizeof(FoodPlace));

    mercadoChino->menu = menuSetup();

    pthread_mutex_init(&mercadoChino->mtxQueue,NULL);
    pthread_mutex_lock(&mercadoChino->mtxQueue);


    pthread_mutex_init(&mercadoChino->cashier.mtxCook,NULL);
    pthread_mutex_init(&mercadoChino->cashier.mtxCheckout,NULL);
    pthread_mutex_lock(&mercadoChino->cashier.mtxCook);
    pthread_mutex_lock(&mercadoChino->cashier.mtxCheckout);

    pthread_t sThread;
    pthread_t cThread;
    pthread_create(&sThread,NULL,streetThread,(void *)mercadoChino);
    pthread_create(&cThread,NULL,cashierThread,(void *)mercadoChino);
    pthread_join(sThread,NULL);


    free(mercadoChino);
    return 0;
}

void *streetThread(void *arg){
    FoodPlace *mercadoChino = (FoodPlace *)arg;
    int tolerance = getMaxWaitTime(mercadoChino->menu);

    pthread_t clientsThreads[CLIENTS];
    Client clients[CLIENTS];

    for (int i = 0; i < CLIENTS; i++){
        clients[i].id = i;
        clients[i].tolerance = tolerance;
        clients[i].order = pickFood(mercadoChino->menu);
        clients[i].mtxQueue = &mercadoChino->mtxQueue;

        sleep(rand()%(3)+1);
        pthread_create(&clientsThreads[i],NULL,clientThread,&clients[i]);
    }

    for(int j = 0; j < CLIENTS; j++)
        pthread_join(clientsThreads[j],NULL);

    pthread_exit(NULL);
}

void *clientThread(void *arg){
    Client *client = (Client *)arg;
    printf("New Client: %d - Menu: %s - Tolerance: %d\r\n",client->id,client->order->name,client->tolerance);

    struct timespec wait;
    clock_gettime(CLOCK_REALTIME, &wait);
    wait.tv_sec += client->tolerance;

    int errCode = pthread_mutex_timedlock(client->mtxQueue, &wait);

    if(!errCode){
        printf("Cliente atendido: %d\r\n",client->id);
        printf("Client %d prepTime: %d\r\n",client->id,client->order->prepTime);

        pthread_mutex_lock(client->mtxFree);

        printf("Client %d Served\r\n",client->id);

    } else{
        printf("Cliente Morido: %d\r\n",client->id);
    }


    pthread_exit(NULL);
}

void *cashierThread(void *arg){
    FoodPlace *mercadoChino = (FoodPlace *)arg;



    pthread_exit(NULL);
}

void cook(Client *client){
    pthread_mutex_lock(&client->cashier->mtxCook);

    sleep(client->order->prepTime);

    pthread_mutex_unlock(&client->cashier->mtxCheckout);
}

void checkout(Client *client){
//    pthread_mutex_lock(&client->cashier->mtxCheckout);
//
//    pthread_mutex_unlock(&client->cashier->mtxCook);
//    pthread_mutex_unlock(&client->cashier->mtxFree);
}

Food *menuSetup(){

    Food *menu = calloc(10, sizeof(Food));

    sprintf(menu[0].name,"Pizza");
    menu[0].prepTime = 2;

    sprintf(menu[1].name,"Lomito");
    menu[1].prepTime = 2;

    sprintf(menu[2].name,"Empanadas");
    menu[2].prepTime = 5;

    sprintf(menu[3].name,"Ensalada");
    menu[3].prepTime = 4;

    sprintf(menu[4].name,"Milanesa");
    menu[4].prepTime = 3;

    sprintf(menu[5].name,"Sushi");
    menu[5].prepTime = 6;

    sprintf(menu[6].name,"Chop Suey");
    menu[6].prepTime = 3;

    sprintf(menu[7].name,"Pollo");
    menu[7].prepTime = 4;

    sprintf(menu[8].name,"Matambre");
    menu[8].prepTime = 3;

    sprintf(menu[9].name,"Choripan");
    menu[9].prepTime = 2;

    return menu;
}
Food *pickFood(Food *menu){
    return &menu[rand()%10];
}
int getMaxWaitTime(Food *menu){

    int min = menu[0].prepTime;

    for(int i = 0; i < 10; i++)
        if(menu[i].prepTime < min)
            min = menu[i].prepTime;
    return min * 4;
}
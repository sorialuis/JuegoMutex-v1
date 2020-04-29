#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MS 1000
#define CLIENTS 10

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
    Client *clients;
    Food *currentOrder;
    int open;
    pthread_mutex_t mtxQueue;
    pthread_mutex_t mtxServed;
}FoodPlace;

typedef struct {
    Client *client;
    FoodPlace *mercadoChino;
}ClientOrder;

/* Thread functions */
void *streetThread(void *);
void *clientThread(void *);
void *cashierThread(void *);
void *cookThread(void *);
void *checkoutThread(void *);

/* Menu Functions */
Food *menuSetup();
Food *pickFood(Food * menu);
int getMaxWaitTime(Food *menu);

int main(){
    srand(time(0));

    FoodPlace *mercadoChino = (FoodPlace *)calloc(1,sizeof(FoodPlace));

    mercadoChino->menu = menuSetup();
    mercadoChino->open = 1;

    pthread_mutex_init(&mercadoChino->mtxQueue,NULL);
    pthread_mutex_init(&mercadoChino->mtxServed,NULL);
    pthread_mutex_init(&mercadoChino->cashier.mtxCook,NULL);
    pthread_mutex_init(&mercadoChino->cashier.mtxCheckout,NULL);

    pthread_mutex_unlock(&mercadoChino->mtxQueue);
    pthread_mutex_lock(&mercadoChino->mtxServed);
    pthread_mutex_lock(&mercadoChino->cashier.mtxCook);
    pthread_mutex_lock(&mercadoChino->cashier.mtxCheckout);

    pthread_t sThread;
    pthread_t cThread;
    pthread_create(&sThread,NULL,streetThread,(void *)mercadoChino);
    pthread_create(&cThread,NULL,cashierThread,(void *)mercadoChino);
    pthread_join(sThread,NULL);

    pthread_mutex_destroy(&mercadoChino->mtxQueue);
    pthread_mutex_destroy(&mercadoChino->mtxServed);
    pthread_mutex_destroy(&mercadoChino->cashier.mtxCook);
    pthread_mutex_destroy(&mercadoChino->cashier.mtxCheckout);

    free(mercadoChino);
    return 0;
}

void *streetThread(void *arg){
    FoodPlace *mercadoChino = (FoodPlace *)arg;
    int tolerance = getMaxWaitTime(mercadoChino->menu);

    pthread_t clientsThreads[CLIENTS];
    Client clients[CLIENTS];
    ClientOrder orders[CLIENTS];

    for (int i = 0; i < CLIENTS; i++){
        clients[i].id = i;
        clients[i].tolerance = tolerance;
        clients[i].order = pickFood(mercadoChino->menu);
        clients[i].mtxQueue = &mercadoChino->mtxQueue;
        orders[i].client = &clients[i];
        orders[i].mercadoChino = mercadoChino;

        sleep(rand()%(3)+1);
        pthread_create(&clientsThreads[i],NULL,clientThread,&orders[i]);
    }

    for(int j = 0; j < CLIENTS; j++)
        pthread_join(clientsThreads[j],NULL);

    mercadoChino->open = 0;

    pthread_exit(NULL);
}

void *clientThread(void *arg){
    ClientOrder *order = (ClientOrder *)arg;
    FoodPlace *mercadoChino = order->mercadoChino;
    Client *client = order->client;

    printf("Nuevo cliente en cola: %d\r\n",client->id);

    struct timespec wait;
    clock_gettime(CLOCK_REALTIME, &wait);
    wait.tv_sec += client->tolerance;

    int errCode = pthread_mutex_timedlock(client->mtxQueue, &wait);

    if(!errCode){
        printf("Cliente: %d ordena %s\r\n",client->id,client->order->name);
        /* Ingreso el pedido del cliente */
        mercadoChino->currentOrder = client->order;
        /* El cajero empieza a cocinar */
        pthread_mutex_unlock(&mercadoChino->cashier.mtxCook);
        /* El Cliente Espera que le cobren */
        pthread_mutex_lock(&mercadoChino->mtxServed);
        printf("El Cliente %d se retira con su orden\r\n",client->id);
        /* Libero el lugar en la cola para que pase el siguiente */
        pthread_mutex_unlock(&mercadoChino->mtxQueue);

    } else{
        printf("El Cliente %d se canso de esperar\r\n",client->id);
    }

    pthread_exit(NULL);
}

void *cashierThread(void *arg){
    FoodPlace *mercadoChino = (FoodPlace *)arg;

    while (mercadoChino->open){
        pthread_t cook,checkout;

        pthread_create(&cook,NULL,cookThread,mercadoChino);
        pthread_create(&checkout,NULL,checkoutThread,mercadoChino);

        pthread_join(checkout,NULL);
    }

    pthread_exit(NULL);
}

void *cookThread(void *arg){
    FoodPlace *mercadoChino = (FoodPlace *) arg;

    pthread_mutex_lock(&mercadoChino->cashier.mtxCook);
    printf("Cocinando %s\r\n",mercadoChino->currentOrder->name);
    sleep(mercadoChino->currentOrder->prepTime);
    printf("Pedido Cocinado\r\n");

    pthread_mutex_unlock(&mercadoChino->cashier.mtxCheckout);

    pthread_exit(NULL);
}

void *checkoutThread(void *arg){
    FoodPlace *mercadoChino = (FoodPlace *) arg;

    pthread_mutex_lock(&mercadoChino->cashier.mtxCheckout);
    printf("Cobrando: %s\r\n",mercadoChino->currentOrder->name);

    pthread_mutex_unlock(&mercadoChino->mtxServed);

    pthread_exit(NULL);
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// Food
typedef struct {
    char name[50];
    int prepTime;
} Food;

// Cashier

typedef struct {

} Cashier;

// Client
typedef struct {
    int id;
    Food choice;
    int tolerance;
    int served;
    pthread_mutex_t timeout;
} Client;

//Local

typedef struct {
    Cashier *cajero;
    Client *clients;
    Food *menu;
}FoodPlace;


// Threads

void *newClient(void *);
void *cliente(void *);
void *cajero(void *);

//Menu Functions
Food *menuSetup();
Food pickFood(Food * menu);
int getMaxWaitTime(Food *menu);

int main(){
    srand(time(NULL));

    int cantidad_cajeros;
    cantidad_cajeros = 1;

    Food *menu = menuSetup();

    printf("%s\n",pickFood(menu).name);
    printf("%d\n",getMaxWaitTime(menu));

    return 0;
}

void onNewClient(FoodPlace *local,Client *newClient){
    local->menu

}

Food* menuSetup(){

    Food *menu = calloc(10, sizeof(Food));

    sprintf(menu[0].name,"Pizza");
    menu[0].prepTime = 500;

    sprintf(menu[1].name,"Lomito");
    menu[1].prepTime = 800;

    sprintf(menu[2].name,"Empanadas");
    menu[2].prepTime = 650;

    sprintf(menu[3].name,"Ensalada");
    menu[3].prepTime = 200;

    sprintf(menu[4].name,"Milanesa");
    menu[4].prepTime = 450;

    sprintf(menu[5].name,"Sushi");
    menu[5].prepTime = 750;

    sprintf(menu[6].name,"Chop Suey");
    menu[6].prepTime = 1000;

    sprintf(menu[7].name,"Pollo");
    menu[7].prepTime = 600;

    sprintf(menu[8].name,"Matambre");
    menu[8].prepTime = 1000;

    sprintf(menu[9].name,"Choripan");
    menu[9].prepTime = 800;

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

    return min * 4;
}

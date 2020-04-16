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

// Street

// Cashier

// Client


// Threads

void *calle(void *);
void *cliente(void *);
void *cajero(void *);
Food *menuSetup();

int main(){
  int cantidad_cajeros;
  cantidad_cajeros = 1;
  
  Food *menu = menuSetup();
  
  return 0;
}


Food* menuSetup(){

    Food *menu = calloc(10, sizeof(Food));

    menu[0].name = "Pizza";
    menu[0].prepTime = 500;
  
    menu[1].name = "Lomito";
    menu[1].prepTime = 800;
  
    menu[2].name = "Empanadas";
    menu[2].prepTime = 650;
  
    menu[3].name = "Ensalada";
    menu[3].prepTime = 300;
  
    menu[4].name = "Milanesa";
    menu[4].prepTime = 450;
  
    menu[5].name = "Sushi";
    menu[5].prepTime = 750;
  
    menu[6].name = "Chop Suey";
    menu[6].prepTime = 1000;
  
    menu[7].name = "Pollo";
    menu[7].prepTime = 600;
  
    menu[8].name = "Matambre";
    menu[8].prepTime = 1000;
  
    menu[9].name = "Choripan";
    menu[9].prepTime = 800;

    return menu;
}
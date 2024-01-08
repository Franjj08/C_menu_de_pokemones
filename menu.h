#ifndef MENU_H_
#define MENU_H_


#include "menu.c"

//Pre: argv, cajas_de_pokemones y lista_pokemones no sean NULL
//Pos: cargan cajas_de_pokemones y lista_pokemones con datos correspondientes 
void inicia_menu(int argc, char* argv[], hash_t* cajas_de_pokemones, hash_t* lista_pokemones, bool* corriendo);

//Pre: cajas_de_pokemones, lista_pokemones, corriendo no sean NULL
//Pre: ejecuta el menu con distintas opciones y devuelve leido si se funciona.
int ejecuta_opcion(hash_t* cajas_de_pokemones, hash_t* lista_pokemones, bool* corriendo);


//Pre:-
//Pos:Imprime las opciones por la pantalla
void mostrar_menu();


#endif // MENU_H_
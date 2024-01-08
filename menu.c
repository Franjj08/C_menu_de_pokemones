#include "src/cajas.h"
#include "src/pokemon.h"
#include "src/abb.h"
#include "src/hash.h"
#include "src/lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define MAX_LINEA 20
#define MAX_NOMBRE 50
#define CANTIDAD_MINIMA 2
#define MOSTRAR_INVENTARIO 'I'
#define CARGAR_OTRA_CAJA 'C'
#define COMBINAR_CAJAS 'M'
#define MOSTRAR_CAJA 'D'
#define BUSCAR_CAJA 'B'
#define SALIR 'Q'


typedef struct datos {
        char nombre[MAX_NOMBRE];
        int cantidad;
}datos_t;

//Pre:-
//Pos:Imprime las opciones por la pantalla
void mostrar_menu()
{
        printf("\nI: Mostrar inventario\n");
        printf("C: Cargar otra caja\n");
        printf("M: Combinar cajas\n");
        printf("D: Mostrar caja\n");
        printf("B: Buscar caja\n");
        printf("Q: Salir:\n");
}

//Pre: a y b no sean NULL
//Pos: Devuelve 0 si son datos repetidos
bool datos_repetidos(void*a, void*b)
{
        datos_t* cajita1 = a;
        datos_t* cajita2 = b;
        return strcmp(cajita1->nombre, cajita2->nombre);


}

//Pre: los parametros no sean NULL
//Pos: Inserta una lista de datos en lista_pokemones
void cargar_lista_en_hash(datos_t* datos, lista_t* pokes, 
hash_t* lista_pokemones, char* nombre_actual, int cantidad_caja, char* nombre)
{
        strcpy(datos->nombre, nombre_actual);
        datos->cantidad = cantidad_caja;
        lista_insertar(pokes, datos);
        hash_insertar(lista_pokemones, nombre,pokes, NULL );
}

//Pre:-
//Pos: Libera dato
void destructor_dato(void* a)
{
        datos_t* dato = a;
        free(dato);
}

//Pre: caja, lista_pokemones y nombre_actual no sean NULL
//Pos: carga lista_pokemones, las calves son nombres de pokemones y valor es datos
void carga_lista_pok(caja_t* caja, hash_t* lista_pokemones, char* nombre_actual)
{
        int cantidad_caja = caja_cantidad(caja);
        for (int i  = 0; i< cantidad_caja; i++) {
                pokemon_t* pok = caja_obtener_pokemon(caja, i);
                char nombre[MAX_NOMBRE];
                strcpy(nombre, pokemon_nombre(pok));

                if(hash_contiene(lista_pokemones, nombre)){


                        lista_t* pokes = hash_obtener(lista_pokemones, nombre);
                        datos_t* datos = malloc(sizeof(datos_t));
                        if(!datos)
                                return;
                        strcpy(datos->nombre, nombre_actual);
                        datos->cantidad = cantidad_caja;
                        lista_insertar(pokes, datos);
                        size_t posicion_repetida = lista_con_cada_elemento(pokes, 
                                                        datos_repetidos,datos);
                        if(posicion_repetida >= 1)
                                return;
                        hash_insertar(lista_pokemones, nombre,pokes, NULL );
                }
                else {
                        lista_t* pokes = lista_crear();
                        if(!pokes)
                                return;
                        datos_t* datos = malloc(sizeof(datos_t));
                        if(!datos){
                                lista_destruir_todo(pokes, destructor_dato);
                                return;
                        }

                        cargar_lista_en_hash(datos, pokes, lista_pokemones, 
                                nombre_actual, cantidad_caja, nombre);
                }        
        }
}

//Pre: argv, cajas_de_pokemones y lista_pokemones no sean NULL
//Pos: cargan cajas_de_pokemones y lista_pokemones con datos correspondientes
void inicia_menu(int argc, char* argv[], hash_t* cajas_de_pokemones, 
                          hash_t* lista_pokemones, bool* corriendo)
{
        if (argc < CANTIDAD_MINIMA) {
                printf("\nNecesita por lo menos una caja\n");
                *corriendo = false;
                return;
        }
        for (int i = 1; i< argc; i++) {
                caja_t* caja = caja_cargar_archivo(argv[i]);
                if(!caja){
                        printf("caja %i no se puede cargar\n", i );
                }
                else {
                        printf("caja %i se puede cargar\n", i );
                        hash_insertar(cajas_de_pokemones, argv[i], caja, NULL);

                        carga_lista_pok(caja,lista_pokemones, argv[i]);
                }
                
        }

        if (hash_cantidad(cajas_de_pokemones) <= 0 ) {
                printf("No se puede cargar ninguna caja\n");
                return;
        }
        
        if (hash_cantidad(cajas_de_pokemones)== argc-1) {
                printf("Se puede cargar todas las cajas\n");
        }
}

//Pre:-
//Pos: devuelve true si es una opcion valida
bool es_opcion_valida(char letra)
{
        
        return(letra == MOSTRAR_INVENTARIO ||letra == CARGAR_OTRA_CAJA 
                || letra == COMBINAR_CAJAS  || letra == MOSTRAR_CAJA
                || letra == BUSCAR_CAJA || letra == SALIR);
}

//Pre: clave y valor no sean NULL
//Pos: Muestra nombre y la cantidad de una caja mientras devuelve true
bool mostrar_datos(const char *clave, void *valor, void *aux)
{
        caja_t* caja = valor;
        int cantidad = caja_cantidad(caja);
        printf("\nnombre de caja: %s, cantidad: %i \n", clave, cantidad);
        
        return true;
}

//Pre: pokemon no sea NULL
//Pos: Muestra datos de un pokemon mientras devuelve true
void mostrar_datos_pokemones( pokemon_t* pokemon)
{       
        char nombre[MAX_NOMBRE];
        strcpy(nombre,pokemon_nombre(pokemon));
	int nivel = pokemon_nivel(pokemon);
	int ataque = pokemon_ataque(pokemon);
	int defensa = pokemon_defensa(pokemon);
	printf("%s;%i;%i;%i\n", nombre, nivel, ataque, defensa);
}
//Pre: -
//Pos: destruir una caja
void destructor_caja(void* a){
        caja_t* caja = a;
        caja_destruir(caja);
}
//Pre: -
//Pos: destruir una lista
void destructor_lista(void* a){
        lista_t* lista = a;
        lista_destruir_todo(lista, destructor_dato);
}
//Pre: a y b no sean NULL
//Pos: Imprime los datos por la pantalla
bool imprimir_datos(void* a, void* b)
{
        datos_t* datos = a;
        printf("nombre de caja: %s ; cantidad: %i \n", datos->nombre, 
                                                datos->cantidad);

        return true;
}
//Pre: Los parametros no sean NULL
//Pos: devuelve true mientras encontra la clave necesita y ejecuta una funcion de lista
bool busca_pokemon(const char *clave, void *valor, void *aux)
{
        char* nombre = aux;
        lista_t* pok = valor;
        if (strcmp(clave,nombre) ==0) {
                lista_con_cada_elemento(pok, imprimir_datos,
			       NULL);
        }
        return true;
}

//Pre: cajas_de_pokemones, lista_pokemones, corriendo no sean NULL
//Pre: ejecuta el menu con distintas opciones y devuelve leido si se funciona.
int ejecuta_opcion(hash_t* cajas_de_pokemones, hash_t* lista_pokemones, 
                                                        bool* corriendo)
{
        char opcion[MAX_NOMBRE];
        printf("Ingrese una opcion valida:\n");
        int leido = scanf(" %s", opcion);
        while (!es_opcion_valida((char)toupper(opcion[0]))) {
                printf("Es una opcion invalida\n");
                mostrar_menu();
                printf("Ingrese una opcion valida:\n");
                leido = scanf(" %s", opcion);
        }

        switch((char)toupper(opcion[0])) {
                case MOSTRAR_INVENTARIO:
                {
                        hash_con_cada_clave(cajas_de_pokemones,
			mostrar_datos,
			NULL);
                }
                break;

                case CARGAR_OTRA_CAJA:
                {
                        printf("Ingrese un nombre valido de caja:");
                        char nombre[MAX_NOMBRE];
                        leido = scanf(" %s", nombre);
                        if (hash_contiene(cajas_de_pokemones, nombre) == true) {
                                printf("Se repite el nombre, no se puede cargar esa caja");
                                return leido;
                        }

                        caja_t* caja = caja_cargar_archivo(nombre);
                        if (!caja) {
                                printf("un nombre invalido, no se puede cargar\n");
                                return leido;
                        }

                        carga_lista_pok(caja, lista_pokemones, nombre);
                        hash_insertar(cajas_de_pokemones, nombre, caja, NULL);
                        printf("Se cargo caja: %s\n", nombre);
                }                      
                break;

                case COMBINAR_CAJAS:
                {
                        printf("Ingrese un nombre valido de caja1:");
                        char nombre1[MAX_NOMBRE];
                        char nombre2[MAX_NOMBRE];
                        char nombre3[MAX_NOMBRE];

                        leido = scanf(" %s", nombre1);
                        if (!hash_contiene(cajas_de_pokemones, nombre1)) {
                                printf("no exsite esa caja1\n");
                                return leido;
                        }

                        printf("Ingrese un nombre valido de caja2:");
                        leido = scanf(" %s", nombre2);
                        if (!hash_contiene(cajas_de_pokemones, nombre2)) {
                                printf("no exsite esa caja2\n");
                                return leido;
                        }

                        printf("Ingrese un nombre valido de archivo:");
                        leido = scanf(" %s", nombre3);
                        if (hash_contiene(cajas_de_pokemones, nombre3)) {
                                printf("Se repite el nombre");
                                return leido;
                        }

                        caja_t* caja1 = hash_obtener(cajas_de_pokemones, nombre1);
                        if (!caja1) {
                                printf("No existe esa caja1\n");
                                return leido;
                        }

                        caja_t* caja2 = hash_obtener(cajas_de_pokemones,nombre2);
                        if (!caja2) {
                                printf("No existe esa caja2\n");
                                caja_destruir(caja1);
                                return leido;
                        }

                        caja_t* caja3 = caja_combinar(caja1, caja2);
                        if (!caja3) {
                                printf("No se puede combinar las dos cajas\n");
                                caja_destruir(caja1);
                                caja_destruir(caja2);
                                return leido;
                        }

                        caja_guardar_archivo(caja3, nombre3);
                        carga_lista_pok(caja3, lista_pokemones, nombre3);
                        hash_insertar(cajas_de_pokemones, nombre3, caja3, NULL);
                        printf("exito\n");
                }
                break;
                
                case MOSTRAR_CAJA:
                {
                        printf("Ingrese un nombre valido de caja:");
                        char nombre[MAX_NOMBRE];
                        leido = scanf(" %s", nombre);
                        if (!hash_contiene(cajas_de_pokemones, nombre)) {
                                printf("No existe esa caja");
                                return leido;
                        }
                        caja_t* caja = hash_obtener(cajas_de_pokemones, nombre);
                        if (!caja) {
                                printf("No existe esa caja");
                                return leido;
                        }

                        printf("nombre de la caja: %s\n", nombre);
                        caja_recorrer(caja, mostrar_datos_pokemones);
                }
                break;

                case BUSCAR_CAJA:
                {
                        printf("Ingrese un nombre valido de pokemon:");
                        char nombre[MAX_NOMBRE];
                        leido = scanf(" %s", nombre);
                        if (!hash_contiene(lista_pokemones, nombre)) {
                                printf("No existe ese pokemon\n");
                                return leido;
                        }
                        hash_con_cada_clave(lista_pokemones,
			   busca_pokemon,
			   nombre);
                      
                }
                break;

                case SALIR:
                {
                        *corriendo = false;
                        hash_destruir_todo(lista_pokemones, destructor_lista);
                        hash_destruir_todo(cajas_de_pokemones, destructor_caja);
                        printf("Bye bye\n");
                        return leido;
                }
                break;
        }

        return leido;
}

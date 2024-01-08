#include "cajas.h"
#include "pokemon.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "abb.h"

#define MAX 50
#define MAX_STRING 100
#define FORMATO_STRING "%s;%i;%i;%i"
#define FORMATO_ESCRITURA "%s;%i;%i;%i\n"

struct _caja_t {
	abb_t* pokemones;
	void (*funcion)(pokemon_t *);
};

struct pokemon_buscado {
	pokemon_t* pokemon;
	int n_esimo;
}; 

//Pre: -
//Pos: devuelve > 0, si el primer string mas grande que el segundo
//     devuelve = 0, si los dos son iguales
//     devuelve < 0, si el primer menor que el segundo
int comparador(void * a, void * b)
{
	pokemon_t* pok1 = (pokemon_t*)a;
	pokemon_t* pok2 = (pokemon_t*)b;

	return strcmp(pokemon_nombre(pok1), pokemon_nombre(pok2));
}

//Pre: archivo_datos no sea NULL,y ya abrio con fopen
//Pos: devuelve una caja que carga los pokemones de ese archivo
caja_t *carga_pokemones_en_caja(FILE *archivo_datos)
{
	char un_pokemon[MAX];
	caja_t* caja = calloc(1, sizeof(caja_t));
	if(!caja)
		return NULL;
	caja->pokemones = abb_crear(comparador);
	while (fgets(un_pokemon, MAX, archivo_datos)) {
		pokemon_t *pok = pokemon_crear_desde_string(un_pokemon);
		if (pok == NULL) {
			caja_destruir(caja);
			return NULL;
		} else { 
			caja->pokemones = abb_insertar(caja->pokemones, 
							   (void*)pok);
			}
		}
	return caja;	
}

caja_t *caja_cargar_archivo(const char *nombre_archivo)
{
	if(!nombre_archivo) 
		return	NULL;	
	

	FILE* archivo_datos = fopen(nombre_archivo, "r");
	if(!archivo_datos) 
			return  NULL;
	
	caja_t *caja = carga_pokemones_en_caja(archivo_datos);

	fclose(archivo_datos);
	return caja;
}

//Pre: _pok_actual y archivo no sean NULL
//Pos: Guarda datos de un pokemoen en archivo
bool guarda_pokemon_en_archivo(void* _pok_actual, void* archivo)
{
	pokemon_t* pok_actual = _pok_actual;
	fprintf(archivo, FORMATO_ESCRITURA, 
		pokemon_nombre(pok_actual),
		pokemon_nivel(pok_actual),
		pokemon_ataque(pok_actual),
		pokemon_defensa(pok_actual));
	
	return true;
}

int caja_guardar_archivo(caja_t *caja, const char *nombre_archivo)
{
	if(!caja ) 
		return 0;
	
	if(!nombre_archivo) 
		return 0;
	

	FILE* archivo_nuevo = fopen(nombre_archivo, "w");
	abb_con_cada_elemento(caja->pokemones, INORDEN, 
	      guarda_pokemon_en_archivo, archivo_nuevo);

	fclose(archivo_nuevo);
	return (int)abb_tamanio(caja->pokemones);
}

//Pre: p no sea NULL
//Pos: devuelve un string con datos de un pokemon
pokemon_t* duplicar_pokemon(pokemon_t* p)
{
	if(!p)
		return NULL;
	char string[MAX_STRING];
	sprintf(string, FORMATO_STRING,
		pokemon_nombre(p),
		pokemon_nivel(p),
		pokemon_ataque(p),
		pokemon_defensa(p));
	
	return pokemon_crear_desde_string(string);
}

//Pre: a y b no sean NULL
//Pos: Mientras devuelve true, se inserta un pokemon a pokemones
bool inserta_pokemoenes(void* a, void* b)
{
	pokemon_t* pokemon = a;
	abb_t* pokemones = b;

	if(pokemon)
		abb_insertar(pokemones, duplicar_pokemon(pokemon));
	
	return true;
}

caja_t *caja_combinar(caja_t *caja1, caja_t *caja2)
{
	if(!caja1 || !caja2)
		return NULL;
	caja_t* caja3 = calloc(1,sizeof(caja_t));
	if(!caja3)
		return NULL;
	caja3->pokemones = abb_crear(comparador);
	if(!caja3->pokemones)
		return NULL;
	
	abb_t* pokemones_1 = caja1->pokemones;
	abb_t* pokemones_2 = caja2->pokemones;
	
	abb_con_cada_elemento(pokemones_1, INORDEN,
			    inserta_pokemoenes, caja3->pokemones);
	
	abb_con_cada_elemento(pokemones_2, INORDEN,
			    inserta_pokemoenes, caja3->pokemones);
	
	if(caja_cantidad(caja3) == caja_cantidad(caja1)+caja_cantidad(caja2))	
		return caja3;
	return NULL;
}

int caja_cantidad(caja_t *caja)
{
	if(!caja)
		return 0;
	return (int)abb_tamanio(caja->pokemones);
}

//Pre: a y b no sean NULL
//Pos: Si encuentra n_esimo poke devuelve false, sino sigue ejecutando y devuelve true
bool busca_pokemoen(void* a, void* b)
{
	pokemon_t* pok = a;
	struct pokemon_buscado* pok_buscado = b;
	
	if(pok && pok_buscado->n_esimo == 0){
		pok_buscado->pokemon = pok;
		return false;
	}
	pok_buscado->n_esimo--;
	
	return true;

}

pokemon_t *caja_obtener_pokemon(caja_t *caja, int n)
{
	if(!caja || n >= caja_cantidad(caja) || n < 0)
		return NULL;
	struct pokemon_buscado pok_buscado;

	pok_buscado.pokemon = NULL;
	pok_buscado.n_esimo = n;

	abb_con_cada_elemento(caja->pokemones, INORDEN,
			    busca_pokemoen, &pok_buscado);

	return  pok_buscado.pokemon;
}

//Pre: a y b no sean NULL
//Pos: Devuelve true mientras aplica una funcion a pokemon
bool recorre_caja(void* a, void* b)
{
	pokemon_t* pokemon = (pokemon_t*)a;
	caja_t* caja = (caja_t*)b;

	caja->funcion(pokemon);

	return true;
}
//Pre:-
//Pos: destruye un bloque de memoria de un pokemon
void destructor_pokemon(void* a)
{
	pokemon_t* pokemon = a;
	pokemon_destruir(pokemon);
}
int caja_recorrer(caja_t *caja, void (*funcion)(pokemon_t *))
{
	if(!caja || !funcion)
		return 0;
	caja->funcion = funcion;
	return (int)abb_con_cada_elemento(caja->pokemones, INORDEN,
			     recorre_caja, (void *)caja);
}

void caja_destruir(caja_t *caja)
{
	if(!caja)
		return;
	abb_destruir_todo(caja->pokemones, destructor_pokemon);
	free(caja);
}

#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CAPACIDAD_MINIMA 3
#define  FACTOR_CARGA_MAXIMO 0.7

typedef struct par {
        char* clave;
        void* valor;
	struct par* siguiente;
}par_t;

struct hash {
	size_t capacidad;
	size_t ocupados;
	size_t (*funcion_hash)(const char*);
	par_t** pares;
};

/*
Pre: str no sea NULL
Pos: Devuelve una varaiable de tipo size_t
*/
size_t funcion_de_hash(const char *str)
{
        size_t hash = 5381;
        int c;
	
        while((c = *(str++)))
                hash = ((hash << 5) + hash) + (size_t)c;

        return hash;
}

/*
Pre: -
Pos: Guarda valor al anterior
*/
void copiar_anterior(void **anterior, void* valor)
{
	if(anterior) 
		*anterior = valor;
}

hash_t *hash_crear(size_t capacidad)
{
	if(capacidad < CAPACIDAD_MINIMA)
		capacidad = CAPACIDAD_MINIMA;
	
	hash_t* hash = malloc(sizeof(hash_t));

	if(!hash)
		return NULL;
	
	hash->capacidad = capacidad;
	hash->ocupados = 0;
	hash->funcion_hash = funcion_de_hash;

	hash->pares = calloc(capacidad, sizeof(par_t*));
	if (!hash->pares) {
		free(hash);
		hash = NULL;
	}
	
	return hash;
}

/*
Pre:-
Pos:guarda string en un nuevo bloque de memoria de char, y lo devuelve.
Si no existe string o hay error, devuelve NULL
*/
char *duplica_string(const char *s)
{
	if (!s)
		return NULL;

	char *p = malloc(strlen(s) + 1);
	if (!p)
		return NULL;

	strcpy(p, s);
	return p;
}

/*
Pre: clave no sea NULL
Pos: Devuelve par_a_insertar que guarda clave y valor correcto
*/
par_t* crea_par_a_insertar(const char *clave, void *elemento,
			   par_t* siguiente)
{
	par_t* par_a_insertar = malloc(sizeof(par_t));
	if(!par_a_insertar)
		return NULL;
	par_a_insertar->clave = duplica_string(clave);
	if (!par_a_insertar->clave) {
			free(par_a_insertar);
			return NULL;
	}
	par_a_insertar->valor = elemento;
	par_a_insertar->siguiente = siguiente;

	return par_a_insertar;
}

/*
Pre: hash, clave, anterior no sean NULL, 
Pos: Guarda valor_actual en anterior, aumenta la cantidad de ocupados de hash 
y devuele un par con clave y valor correcto 
*/
par_t* inserta_par_hash(hash_t *hash, const char *clave, void *elemento,
		      void **anterior, par_t* siguiente, void* valor_actual)
{
		copiar_anterior(anterior, valor_actual);
		par_t* par_a_insertar = crea_par_a_insertar(clave, elemento, 
								 siguiente);
		if(!par_a_insertar)
			return NULL;
		hash->ocupados++;
		return par_a_insertar;
}

/*
Pre: -
Pos: Libera la clave y el par
*/
void liberar_un_par(par_t* par)
{
	if(!par)
		return;
	free(par->clave);
	free(par);
}

/*
Pre: hash no sea NULL
Pos: Devuelve true si hash paso FACTOR_CARGA_MAXIMO, si no false
*/
bool factor_carga(hash_t* hash) 
{	

	float factor_carga;
	factor_carga =  (float)hash->ocupados/(float)hash->capacidad;
	return(factor_carga >= FACTOR_CARGA_MAXIMO);
}

/*
Pre: hash, clave, anterior no sean NULL
Pos: Inserta un par en el hash y devuelve ese hash
*/
hash_t* inserta_par_en_nuevo_hash(hash_t *hash, const char *clave,
 				void *elemento, void **anterior)
{
	size_t posicion = hash->funcion_hash(clave)%hash->capacidad;
	par_t* par_actual = hash->pares[posicion];


	if (!par_actual) {
		void* valor_actual = NULL;
		par_t* siguiente = NULL;
		hash->pares[posicion] = inserta_par_hash(hash, clave, elemento,
					    anterior, siguiente, valor_actual);
		return hash;
	}

	while (par_actual ) {
		if(strcmp(par_actual->clave, clave) == 0){
			copiar_anterior(anterior, par_actual->valor);
			par_actual->valor = elemento;
			return hash;
		}

		par_actual = par_actual->siguiente;
	}
	
	hash->pares[posicion] = inserta_par_hash(hash, clave, elemento, anterior,
						    hash->pares[posicion], NULL);
	return hash;
}

/*
Pre: hash no sea NULL
Pos: Devuelve un nuevo hash con pares reubicados
*/
hash_t* hash_rehash(hash_t* hash, void** anterior)
{
	hash_t* nuevo = hash_crear(hash->capacidad*2);
	if(!nuevo)
		return NULL;
	for (size_t i = 0; i < hash->capacidad; i++) {
		par_t* par_actual = hash->pares[i];
		while (par_actual) {
			inserta_par_en_nuevo_hash(nuevo, par_actual->clave,
					      par_actual->valor, anterior);
			par_actual = par_actual->siguiente;
		}
	}

	hash_t auxiliar = *hash;
        *hash = *nuevo;
        *nuevo = auxiliar;        

        hash_destruir(nuevo);
        return hash;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if(!hash|| !clave)
		return NULL;
	
	if(factor_carga(hash))
		if(!hash_rehash(hash, anterior))
			return NULL;
	
	size_t posicion = hash->funcion_hash(clave)%hash->capacidad;
	par_t* par_actual = hash->pares[posicion];


	if (par_actual == NULL) {
		void* valor_actual = NULL;
		par_t* siguiente = NULL;
		hash->pares[posicion] = inserta_par_hash(hash, clave, elemento,
					    anterior, siguiente, valor_actual);
		if(!hash->pares[posicion])
			return NULL;
		return hash;
	}

	while (par_actual ) {
		if(strcmp(par_actual->clave, clave) == 0){
			copiar_anterior(anterior, par_actual->valor);
			par_actual->valor = elemento;
			return hash;
		}

		par_actual = par_actual->siguiente;
	}
	
	hash->pares[posicion] = inserta_par_hash(hash, clave, elemento, anterior,
						    hash->pares[posicion], NULL);
	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if(!hash || !clave)
		return NULL;
	
	size_t posicion = hash->funcion_hash(clave)%hash->capacidad;
	par_t* par_actual = hash->pares[posicion];
	if(!par_actual) 
		return NULL;
	
	bool encontrado = false;
	void* elemento_busacado = NULL;

	if (strcmp(par_actual->clave, clave)==0) {
		elemento_busacado = par_actual->valor;
		par_t* auxliar = par_actual->siguiente;
		liberar_un_par(par_actual);
		hash->pares[posicion] = auxliar;
		hash->ocupados--;
		return elemento_busacado;
	}

	while(par_actual && !encontrado ) {
		par_t* par_siguiente = par_actual->siguiente;
		if (strcmp(par_siguiente->clave, clave)==0 ) {
			encontrado = true;
			elemento_busacado = par_siguiente->valor;
			par_t* siguiente = par_siguiente->siguiente;
			liberar_un_par(par_siguiente);
			hash->ocupados--;

			if(!siguiente) 
				par_actual->siguiente = NULL; 
			
			else 
				par_actual->siguiente = siguiente; 
			
		} else
			par_actual = par_actual->siguiente;
	}

	return elemento_busacado;

}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if(!hash || !clave)
		return NULL;

	size_t posicion = hash->funcion_hash(clave)%hash->capacidad;

	par_t* par_actual = hash->pares[posicion];
	bool encontrado = false;
	while (par_actual && !encontrado) {
		if(strcmp(par_actual->clave, clave)==0)
			encontrado = true;
		else
			par_actual = par_actual->siguiente;
	}

	if(encontrado)
		return par_actual->valor;
	else
		return NULL;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if(!hash || !clave)
		return false;

	size_t posicion = hash->funcion_hash(clave)%hash->capacidad;

	par_t* par_actual = hash->pares[posicion];
	if(!par_actual)
		return false;
	while (par_actual ) {
		if (strcmp(par_actual->clave, clave)==0) {
			if(par_actual->valor)
				return true;
			else
				return false;
		}
		else
			par_actual = par_actual->siguiente;
	}

	return false;
}

size_t hash_cantidad(hash_t *hash)
{
	if(!hash)
		return 0;
	
	return hash->ocupados;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if(!hash)
		return;
	for (size_t i = 0; i < hash->capacidad; i++) {
		par_t* par_actual = hash->pares[i];
		while(par_actual) {
			par_t* par_a_destruir = par_actual;
			if(destructor)
				destructor(par_actual->valor);
			par_actual = par_actual->siguiente;
			liberar_un_par(par_a_destruir);
		}
	}
	free(hash->pares);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,bool (*f)(const char *clave,
		 		void *valor, void *aux),void *aux)
{
	if(!hash || !f)
		return 0;
	size_t cantidad_iterado = 0;
	for (size_t i = 0; i < hash->capacidad; i++) {
		par_t* par_actual = hash->pares[i];
		while (par_actual) {
			cantidad_iterado++;
			if(f(par_actual->clave, par_actual->valor, aux) == false)
				return cantidad_iterado;
			else
				par_actual = par_actual->siguiente;
		}
	}

	return cantidad_iterado;
}

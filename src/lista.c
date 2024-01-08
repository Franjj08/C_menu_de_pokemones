#include "lista.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define VACIA 0

nodo_t *recorre_lista_hasta_posicion( lista_t *lista, size_t posicion)
{
	size_t posicion_recorrida = 0;
	nodo_t *actual = lista->nodo_inicio;
	while (actual !=NULL && posicion_recorrida < posicion) {
		actual = actual->siguiente;
		posicion_recorrida++;
	}

	return actual;
}

lista_t *lista_crear()
{
	lista_t *lista = calloc(1, sizeof(lista_t));

	if(lista == NULL)
		return NULL;
	else
		return lista;

}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (lista == NULL )
		return NULL;
		
	nodo_t *ultimo_nodo = malloc(sizeof(nodo_t));
	if (ultimo_nodo == NULL)
		return NULL;
		
	ultimo_nodo->elemento = elemento;
	ultimo_nodo->siguiente = NULL;

	if (lista_vacia(lista) == true) {
		lista->nodo_inicio = ultimo_nodo;
		lista->nodo_fin = ultimo_nodo;
		lista->cantidad++;
	} else {
		lista->nodo_fin->siguiente = ultimo_nodo;
		lista->nodo_fin = ultimo_nodo;
		lista->cantidad++;
	}

	return lista;

}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista  )
		return NULL;
		
	size_t cantidad_lista = lista_tamanio(lista);
	if (posicion >= cantidad_lista) {
		lista = lista_insertar(lista, elemento);
		return lista;
	}

	nodo_t *nodo_insertado = malloc(sizeof(nodo_t));
	nodo_insertado->elemento = elemento;
	nodo_insertado->siguiente = NULL; 

	if (posicion == 0) {
		nodo_insertado->siguiente = lista->nodo_inicio;
		lista->nodo_inicio = nodo_insertado;
		lista->cantidad++;
		return lista;
	}

	nodo_t *anterior = recorre_lista_hasta_posicion(lista, posicion-1);
	if (anterior == NULL)
		return NULL;
		
	nodo_insertado->siguiente = anterior->siguiente;
	anterior->siguiente = nodo_insertado;


	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (!lista || !lista->nodo_inicio)
		return NULL;
		
	size_t cantidad = lista_tamanio(lista);

	void* elemento = lista->nodo_fin->elemento;

	if (cantidad == 1) {
		free(lista->nodo_inicio);
		lista->nodo_inicio = NULL;
		lista->nodo_fin = NULL;
		lista->cantidad--;
		return elemento;
	}

	nodo_t *anteultimo = recorre_lista_hasta_posicion(lista, cantidad-2);
	free(lista->nodo_fin);
	anteultimo->siguiente = NULL;
	lista->nodo_fin = anteultimo;
	lista->cantidad--;

	return elemento;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || !lista->nodo_inicio)
		return NULL;
		
	size_t cantidad = lista_tamanio(lista);

	if (posicion >= cantidad) {
		return lista_quitar(lista);
	}

	if (cantidad == 1) {
		void* elemento = lista->nodo_fin->elemento;
		free(lista->nodo_inicio);
		lista->nodo_inicio = NULL;
		lista->nodo_fin = NULL;
		lista->cantidad--;
		return elemento;
	}

	if ( posicion == 0) {
		void* elemento = lista->nodo_inicio->elemento;
		nodo_t* primero = lista->nodo_inicio;
		lista->nodo_inicio = primero->siguiente;
		free(primero);
		lista->cantidad--;
		return elemento;
	}

	nodo_t *anterior = recorre_lista_hasta_posicion(lista, posicion-1);
	void* elemento = anterior->siguiente->elemento;
	nodo_t *a_destruir = anterior->siguiente;
	anterior->siguiente = a_destruir->siguiente;
	free(a_destruir);
	lista->cantidad--;

	return elemento;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || !lista->nodo_inicio)
		return NULL;

	size_t cantidad =lista_tamanio(lista);
	if (posicion >= cantidad)
		return NULL;

	nodo_t *actual = recorre_lista_hasta_posicion(lista, posicion);
	if (actual ==NULL)
		return NULL;
	
        return actual->elemento;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if(!lista || !comparador )
		return NULL;

	nodo_t *actual = lista->nodo_inicio;
	while (actual != NULL ) {

	        if (comparador(actual->elemento, contexto) == 0) {
		        return actual->elemento;
	        } else {
		        actual = actual->siguiente;
	        }
		
	}     	
		return NULL;	
}

void *lista_primero(lista_t *lista)
{
	if(lista == NULL ||!lista->nodo_inicio)
		return NULL;
	else	
		return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if(!lista || !lista->nodo_fin)
		return NULL;
		
	return lista->nodo_fin->elemento;
}

bool lista_vacia(lista_t *lista)
{
	if(!lista || lista->nodo_inicio == NULL  || lista->nodo_fin == NULL
                                                || lista->cantidad == VACIA )
		return true;
	else
		return false;
}

size_t lista_tamanio(lista_t *lista)
{
	if (lista == NULL || lista->nodo_inicio == NULL)
		return 0;
		
	nodo_t *actual = lista->nodo_inicio;
	lista->cantidad = 0;
	while (actual != NULL) {
		actual = actual->siguiente;
		lista->cantidad++;
	}

	return lista->cantidad;

}

void lista_destruir(lista_t *lista)
{
	if (lista == NULL)
		return;
		
	nodo_t *actual = lista->nodo_inicio;

	while (actual) {
		nodo_t *a_destruir = actual;
		actual = actual->siguiente;
		free(a_destruir);
	}

		
	free(lista);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista)
		return;
	nodo_t *actual = lista->nodo_inicio;

	while (actual) {
		nodo_t *a_destruir = actual;
		if (funcion != NULL)
			funcion(actual->elemento);

		actual = actual->siguiente;
		free(a_destruir);
	}

	free(lista);			
}	

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista )
		return NULL;
	
	lista_iterador_t *iterador = malloc(sizeof(lista_iterador_t));
	if (!iterador)
		return NULL;
	iterador->corriente = lista->nodo_inicio;
	iterador->lista = lista;

	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->lista)
		return false;

	return iterador->corriente != NULL;

}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->lista)
		return false;

	if (lista_iterador_tiene_siguiente(iterador)){
		iterador->corriente = iterador->corriente->siguiente;

		if (iterador->corriente != NULL)
			return true;
		else 
			return false;
				
	} else {
		return false;
	}
		
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if(!iterador || !iterador->lista)
		return NULL;
		
	if (iterador->corriente)
		return iterador->corriente->elemento;
	else
		return NULL;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->lista)
		return;
		
	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if(!lista || !funcion )
		return 0;

	size_t cantidad_iterado = 0;
	nodo_t *actual = lista->nodo_inicio;
	while (actual != NULL ) {
		cantidad_iterado++;
		if (funcion(actual->elemento, contexto) == false){ 
			return cantidad_iterado;
		} else {
			actual = actual->siguiente;
                         }
	}

	return cantidad_iterado;
}

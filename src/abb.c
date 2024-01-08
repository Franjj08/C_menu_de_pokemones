#include "abb.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

abb_t *abb_crear(abb_comparador comparador)
{
	if(comparador == NULL)
 		return NULL;
	abb_t* arbol= calloc(1, sizeof(abb_t));
	arbol->comparador = comparador;

	return arbol;
}

nodo_abb_t *crea_nodo(void * elemento)
{
	nodo_abb_t *nodo = calloc(1, sizeof(nodo_abb_t));
        if(nodo == NULL)
                return nodo;
        nodo->elemento = elemento;
        return nodo;
}

nodo_abb_t *abb_insertar_rec(abb_t *arbol, nodo_abb_t *raiz, void *elemento,
								 bool *exito)
{
	if (raiz == NULL) {
		nodo_abb_t *nodo_a_insertar = crea_nodo(elemento);
		if(nodo_a_insertar == NULL)
			*exito = false;
		else
			*exito = true;

		return nodo_a_insertar;
	}

	if (arbol->comparador(elemento,raiz->elemento) >0 )
		raiz->derecha = abb_insertar_rec(arbol, raiz->derecha, 
							elemento, exito);
	else
		raiz->izquierda = abb_insertar_rec(arbol, raiz->izquierda,
							 elemento, exito);
	
	return raiz;
}
abb_t *abb_insertar(abb_t *arbol, void *elemento)
{
	if(!arbol)
		return NULL;

	bool exito = false;
	arbol->nodo_raiz = abb_insertar_rec(arbol,arbol->nodo_raiz,
						 elemento, &exito);

	if (exito == true) {
		arbol->tamanio++;
		return arbol;
	}
	else
		return NULL;
}

nodo_abb_t *buscar_mas_derecho (nodo_abb_t *raiz, nodo_abb_t **predecesor)
{
        if (raiz->derecha == NULL) {
                nodo_abb_t *izquierda = raiz->izquierda;
		*predecesor = raiz;
                return izquierda;
        }
        raiz->derecha = buscar_mas_derecho(raiz->derecha, predecesor);
        return raiz;
}

nodo_abb_t *abb_quitar_rec(abb_t *arbol, nodo_abb_t *raiz, void *elemento,
				    bool *exito, void **elemento_a_quitar)
{
	if (raiz == NULL) {
		*exito = false;
		return NULL;
	}

	if (arbol->comparador(elemento, raiz->elemento) > 0)
		raiz->derecha = abb_quitar_rec(arbol, raiz->derecha, elemento,
						    exito, elemento_a_quitar);
	else if (arbol->comparador(elemento, raiz->elemento) < 0)
		raiz->izquierda = abb_quitar_rec(arbol, raiz->izquierda, 
				    elemento, exito, elemento_a_quitar);
	else {
		if (raiz->derecha == NULL || raiz->izquierda == NULL) {
                        nodo_abb_t *hijo_no_nulo = raiz->derecha;
                        if(hijo_no_nulo == NULL)
                                hijo_no_nulo = raiz->izquierda;
			*elemento_a_quitar = raiz->elemento;
                        free(raiz);
			*exito = true;
                        return hijo_no_nulo;
                }
		else{
			*elemento_a_quitar = raiz->elemento;
                        nodo_abb_t *predecesor = NULL;
                        raiz->izquierda = buscar_mas_derecho(raiz->izquierda,
								 &predecesor);
			nodo_abb_t * actual = raiz;
			predecesor->izquierda = actual->izquierda;
			predecesor->derecha = actual->derecha;
			free(actual);
                        raiz = predecesor;
			*exito = true;
                        return raiz;
                }

	}
	return raiz;
}

void *abb_quitar(abb_t *arbol, void *elemento)
{
	if(!arbol)
		return NULL;
	bool exito = false;
	void *elemento_a_quitar = NULL;

	arbol->nodo_raiz = abb_quitar_rec(arbol,arbol->nodo_raiz, elemento,
						&exito, &elemento_a_quitar);
	if (exito == true) {
		arbol->tamanio--;
		return elemento_a_quitar;
	}
	else {
		return NULL;
	}

}

void *abb_buscar(abb_t *arbol, void *elemento)
{
	if(!arbol)
	 	return NULL;
	bool encontrado = false;
	nodo_abb_t *nodo = arbol->nodo_raiz;
        while(nodo != NULL && encontrado == false){
                if( arbol->comparador(elemento,nodo->elemento) > 0)
                        nodo = nodo->derecha;
                else if (arbol->comparador(elemento,nodo->elemento) < 0)
                        nodo = nodo->izquierda;
                else 
			encontrado = true;
        }
        
	if(encontrado == true)
		return nodo->elemento;
	return NULL;
}

bool abb_vacio(abb_t *arbol)
{
	return (!arbol|| arbol->tamanio == 0 || !arbol->nodo_raiz);
}

size_t abb_tamanio(abb_t *arbol)
{
	if(!arbol)
		return 0;
	
	return arbol->tamanio;
}

void abb_destruir_rec(nodo_abb_t *raiz)
{
	if(raiz == NULL)
                return;
        
        abb_destruir_rec(raiz->izquierda);
        abb_destruir_rec(raiz->derecha);
        
        free(raiz);
}

void abb_destruir(abb_t *arbol)
{
	if(!arbol)
		return;
	
	abb_destruir_rec(arbol->nodo_raiz);
	free(arbol);
}

void abb_destruir_todo_rec(nodo_abb_t *raiz, void (*destructor)(void *))
{
	if(raiz == NULL)
		return;
	abb_destruir_todo_rec(raiz->izquierda, destructor);
	abb_destruir_todo_rec(raiz->derecha, destructor);

	if(destructor)
		destructor(raiz->elemento);
	free(raiz);
}

void abb_destruir_todo(abb_t *arbol, void (*destructor)(void *))
{
	if(!arbol)
		return;
	
	abb_destruir_todo_rec(arbol->nodo_raiz, destructor);
	free(arbol);
}

void abb_con_cada_elemento_inorder(nodo_abb_t *raiz, bool (*funcion)(void *, void *),
				 void *aux, size_t *cantidad_recorrido, bool *exito)
{
	if(!raiz || !(*exito))
		return;
	abb_con_cada_elemento_inorder(raiz->izquierda, funcion, aux, 
					cantidad_recorrido, exito);
	if (*exito == true) {
		if (funcion(raiz->elemento, aux) == false) {
			(*cantidad_recorrido)++;
			*exito = false;
			return;
		}
		(*cantidad_recorrido)++;
	}
	abb_con_cada_elemento_inorder(raiz->derecha, funcion, aux, cantidad_recorrido, exito);

}

void abb_con_cada_elemento_preorder(nodo_abb_t *raiz, bool (*funcion)(void *, void *),
				    void *aux, size_t *cantidad_recorrido, bool *exito)
{
	if(!raiz || !(*exito))
		return;

	if (*exito == true) {
		if (funcion(raiz->elemento, aux) == false) {
			(*cantidad_recorrido)++;
			*exito = false;
			return;
		}
		(*cantidad_recorrido)++;
	}
	
	abb_con_cada_elemento_preorder(raiz->izquierda, funcion, aux,
					 cantidad_recorrido, exito);
	
	abb_con_cada_elemento_preorder(raiz->derecha, funcion, aux,
					cantidad_recorrido, exito);
	

	
}

void abb_con_cada_elemento_postorder(nodo_abb_t *raiz, bool (*funcion)(void *, void *),
				    void *aux, size_t *cantidad_recorrido, bool *exito)
{
	if(!raiz || !(*exito))
		return;
	
	abb_con_cada_elemento_postorder(raiz->izquierda, funcion, aux,
					    cantidad_recorrido, exito);
	
	abb_con_cada_elemento_postorder(raiz->derecha, funcion, aux, 
	   				cantidad_recorrido, exito);
	

	if (*exito == true) {
		if (funcion(raiz->elemento, aux) == false) {
			(*cantidad_recorrido)++;
			*exito = false;
			return;
		}
		(*cantidad_recorrido)++;
	}
}

size_t abb_con_cada_elemento(abb_t *arbol, abb_recorrido recorrido,
			     bool (*funcion)(void *, void *), void *aux)
{
	if(arbol == NULL || !funcion)
		return 0;
	size_t cantidad_recorrido = 0;
	bool exito = true;
	switch(recorrido) {
		case INORDEN:
			abb_con_cada_elemento_inorder(arbol->nodo_raiz, funcion,
					      aux, &cantidad_recorrido, &exito);
			break;
		case PREORDEN:
			abb_con_cada_elemento_preorder(arbol->nodo_raiz, funcion,
						 aux, &cantidad_recorrido,&exito);
			break;
		case POSTORDEN:
			abb_con_cada_elemento_postorder(arbol->nodo_raiz, funcion,
						 aux, &cantidad_recorrido,&exito);
			break;
		default:
			break;
	}
	return cantidad_recorrido;
}

void abb_recorrer_inorder(nodo_abb_t *raiz, void **array, size_t tamanio_array,
						    size_t *cantidad_recorrido)
{
	if(!raiz)
		return;

	abb_recorrer_inorder(raiz->izquierda, array, tamanio_array, cantidad_recorrido);

	if ((*cantidad_recorrido) < tamanio_array) {
		array[(*cantidad_recorrido)] = raiz->elemento;
		(*cantidad_recorrido)++;
	}
	else
		return;
	
	abb_recorrer_inorder(raiz->derecha, array, tamanio_array, cantidad_recorrido);
}

void abb_recorrer_preorder(nodo_abb_t *raiz, void **array, size_t tamanio_array,
						    size_t *cantidad_recorrido)
{
	if(!raiz)
		return;

	if ((*cantidad_recorrido) < tamanio_array) {
		array[(*cantidad_recorrido)] = raiz->elemento;
		(*cantidad_recorrido)++;
	}
	else
		return;

	abb_recorrer_preorder(raiz->izquierda, array, tamanio_array,
						 cantidad_recorrido);
	
	abb_recorrer_preorder(raiz->derecha, array, tamanio_array,
						 cantidad_recorrido);
}

void abb_recorrer_postorder(nodo_abb_t *raiz, void **array, size_t tamanio_array,
						     size_t *cantidad_recorrido)
{
	if(!raiz)
		return;
		
	abb_recorrer_postorder(raiz->izquierda, array, tamanio_array,
						 cantidad_recorrido);
	
	abb_recorrer_postorder(raiz->derecha, array, tamanio_array,
						 cantidad_recorrido);

	if ((*cantidad_recorrido) < tamanio_array) {
		array[(*cantidad_recorrido)] = raiz->elemento;
		(*cantidad_recorrido)++;
	}
	else
		return;
}
size_t abb_recorrer(abb_t *arbol, abb_recorrido recorrido, void **array,
		    size_t tamanio_array)
{
	if(!arbol || !array)
		return 0;
	size_t cantidad_recorrido = 0;

	switch(recorrido) {
		case INORDEN:
			abb_recorrer_inorder(arbol->nodo_raiz, array,
				 tamanio_array, &cantidad_recorrido);
			break;
		case PREORDEN:
			abb_recorrer_preorder(arbol->nodo_raiz, array,
				  tamanio_array, &cantidad_recorrido);
			break;
		case POSTORDEN:
			abb_recorrer_postorder(arbol->nodo_raiz, array, 
				   tamanio_array, &cantidad_recorrido);
			break;
		default:
			break;
	}
	return cantidad_recorrido;
}

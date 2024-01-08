#include "src/abb.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "src/pokemon.h"
#include "src/cajas.h"
#include "pa2m.h"

#define MAX_STRING 20

void caja_carga_archivo()
{
	caja_t* caja =caja_cargar_archivo("datos.csv");
	caja_t* caja2 =caja_cargar_archivo("sakjd.csv");
	pa2m_afirmar(caja != NULL, "caja se puede cargar un archivo valido");
	pa2m_afirmar(caja_cantidad(caja) == 2, "La caja despues de carga archivo su tamaño es correcto");
	pa2m_afirmar(caja2 == NULL, "caja carga un archivo invalido devuele NULL");

	caja_destruir(caja);
}


void caja_guarda_archivo()
{
	caja_t* caja1 =caja_cargar_archivo("datos.csv");
	int cantidad_errores = 0;

	pa2m_afirmar(caja_guardar_archivo(caja1, "nuevo2.csv") == caja_cantidad(caja1), "La caja se guarda en un archivo nuevo");
	pa2m_afirmar(caja_guardar_archivo(NULL, "nuevo2.csv") == 0, "La caja NULL no se guarda en un archivo nuevo");
	
	caja_t* caja2 =caja_cargar_archivo("nuevo2.csv");
	for(int i =0; i < caja_cantidad(caja1); i++){
		pokemon_t* pok1 = caja_obtener_pokemon(caja1, i);
		pokemon_t* pok2 = caja_obtener_pokemon(caja2, i);
		 
		if(strcmp(pokemon_nombre(pok1), pokemon_nombre(pok2)) !=0)
			cantidad_errores++;
		
		if(pokemon_ataque(pok1) != pokemon_ataque(pok2 ))
			cantidad_errores++;
		
		if(pokemon_nivel(pok1) != pokemon_nivel(pok2 ))
			cantidad_errores++;

		if(pokemon_defensa(pok1) != pokemon_defensa(pok2 ))
			cantidad_errores++;

	}
	pa2m_afirmar(cantidad_errores == 0, "los datos de pokemoens coinciden con los de archivo");


	caja_destruir(caja1);
	caja_destruir(caja2);
}

void caja_combina_cajas()
{
	caja_t* caja1 =caja_cargar_archivo("datos.csv");
	caja_t* caja2 =caja_cargar_archivo("datos2.csv");
	caja_t* caja3 =caja_combinar(caja1, caja2);
	caja_t* caja4 =caja_combinar(caja1, NULL);

	int cantidad_caja1 = caja_cantidad(caja1);
	int cantidad_caja2 = caja_cantidad(caja2);

	pa2m_afirmar(caja4 == NULL, "La caja no se puede combinar con caja NULL devuele NULL");
	pa2m_afirmar(caja3 != NULL, "La caja despuesde la combinacion no devuele NULL");
	pa2m_afirmar(caja_cantidad(caja3) == cantidad_caja1 + cantidad_caja2 , "La caja combinada tiene la cantidad correcta");



	caja_destruir(caja1);
	caja_destruir(caja2);
	caja_destruir(caja3);
	caja_destruir(caja4);
}

void cantidad_caja()
{
	caja_t* caja1 =caja_cargar_archivo("datos.csv");

	pa2m_afirmar(caja_cantidad(caja1) == 2, "La caja despues de carga archivo su tamaño es correcto");
	pa2m_afirmar(caja_cantidad(NULL) == 0, "La caja NULL no devuelve 0");

	caja_destruir(caja1);
}

void caja_obtiene_pokemon()
{
	caja_t* caja1 =caja_cargar_archivo("datos.csv");
	int cantidad_errores = 0;

	for(int i = 0; i< caja_cantidad(caja1); i++)
	{
		if(caja_obtener_pokemon(caja1, 0) == NULL)
			cantidad_errores++;
	}

	pa2m_afirmar(cantidad_errores == 0,"La caja obtiene las pokemones correctamente");
	pa2m_afirmar(caja_obtener_pokemon(NULL,0) == NULL, "La caja NULL no se obtiene ningun pokemon");

	caja_destruir(caja1);
}

void funcion(pokemon_t* a)
{
	pokemon_ataque(a);
}
void caja_recorre()
{
	caja_t* caja1 =caja_cargar_archivo("nuevo.csv");

	pa2m_afirmar( caja_recorrer(caja1, funcion) == caja_cantidad(caja1), "La funcion aplica a todos los pokemones");
	pa2m_afirmar( caja_recorrer(NULL, funcion) == 0, "La caja NULL no se puede aplicar ninguna funcion");
	pa2m_afirmar( caja_recorrer(caja1, NULL) == 0, "La funcion NULL no se puede aplicar");
	caja_destruir(caja1);
}
int main()
{
	pa2m_nuevo_grupo("Pruebas caja carga archivo");
	caja_carga_archivo();

	pa2m_nuevo_grupo("Pruebas caja carga archivo");
	caja_guarda_archivo();

	pa2m_nuevo_grupo("Pruebas caja comnbina cajas");
	caja_combina_cajas();

	pa2m_nuevo_grupo("Pruebas la cantidad de caja");
	cantidad_caja();

	pa2m_nuevo_grupo("Pruebas la caja obtiene pokemon");
	caja_obtiene_pokemon();

	pa2m_nuevo_grupo("Pruebas la caja recorrer");
	caja_recorre();


	return pa2m_mostrar_reporte();
}

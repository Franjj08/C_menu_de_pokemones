#include "pokemon.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NOMBRE_POKEMON 30
#define MAX 10
#define CANTIDAD_DATOS 4
#define FORMATO_LECTURA "%[^;];%[^;];%[^;];%[^;]"

struct _pokemon_t {
		char nombre[MAX_NOMBRE_POKEMON];
		int nivel;
		int ataque;
		int defensa;
};

pokemon_t *pokemon_crear_desde_string(char *string)
{
		if(!string){
				return NULL;
		}
		pokemon_t *pokemon;
		pokemon = malloc(1*sizeof(pokemon_t));

		if (pokemon == NULL){
				return NULL;
		}

		char nivel[MAX];
		char ataque[MAX];
		char defensa[MAX];
		int cantidad_guardada = sscanf(string, FORMATO_LECTURA,
								pokemon->nombre, nivel, ataque, defensa);
		if (cantidad_guardada != CANTIDAD_DATOS) {
				free(pokemon);
				return NULL;
		} else {
				pokemon->nivel = atoi(nivel);
				pokemon->ataque = atoi(ataque);
				pokemon->defensa = atoi(defensa);

		}		
		return pokemon;		
}

int pokemon_nivel(pokemon_t *pokemon)
{
		if (pokemon != NULL)
				return (pokemon->nivel);
		else
				return 0;
}

int pokemon_ataque(pokemon_t *pokemon)
{
		if (pokemon != NULL)
				return (pokemon->ataque);
		else
				return 0;
}

int pokemon_defensa(pokemon_t *pokemon)
{
		if (pokemon != NULL)
				return (pokemon->defensa);
		else
				return 0;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
		if (pokemon != NULL)
				return (pokemon->nombre);
		else
				return NULL;
}

void pokemon_destruir(pokemon_t *pokemon)
{
		if (pokemon != NULL)
				free(pokemon);
}

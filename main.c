#include "menu.h"

int main(int argc, char *argv[]) {

	hash_t* hash =hash_crear(3);
        hash_t* hash2 =hash_crear(3);
        bool corriendo = true;
	inicia_menu(argc,argv, hash, hash2, &corriendo);

        while(corriendo){
                mostrar_menu();
                ejecuta_opcion(hash, hash2, &corriendo);
        }


	return 0;
}
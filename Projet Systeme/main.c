#include "structure.h"

int main()
{
	if(!charger_disque()){
		printf("Le disque '%s' n'existe pas. Voulez-vous le creer ?", NOM_DISQUE);
		if(confirmation()){
			printf("Creation du disque...\n");
			creation_disque();
		}
	}
	if(!charger_disque()){
		return 0;
	}
	
	MainInterpreteur(); //lancer l'interpreteur de commande après la création ou le chargement du disque
	close(disk);
}

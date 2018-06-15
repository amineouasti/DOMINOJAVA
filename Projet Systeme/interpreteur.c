#include "structure.h"

int MainInterpreteur (void) {

	char * chaine = NULL; // chaine qui va contenir ce que l'utilisateur saisie
	char ** arguments = NULL; // chaine transformer en tableau d'arguments
	int nbarg = 0; // nombre d'arguments
	numInodeActu = root;
	int continuer = 1; // variable pour continuer le programme ou non

	while(continuer) {
		write (1, "[MONSHELL]:~$ ", 14);
		chaine = saisie_cmd ();

		if (chaine == NULL)
			continue;

		arguments = cmd_avec_args(chaine, &nbarg);

		if (arguments == NULL)
			continue;

		continuer = interpreteur (arguments, nbarg);

		free (arguments);
		free (chaine);
	}

	return 0;
}

char* saisie_cmd ()
{
	int t = 0;
	char* chaine = NULL;
	while (1) {  // saisie de la ligne de commande

		char buffer[TAILLE_BUFF] = {0}; // buffer pour récupérer la chaine (commande) entrée
		if (read (0, & buffer, TAILLE_BUFF) == 0) // saisie d'un buffer
			break;

		t++;
		char * chaineTmp = (char *) malloc (sizeof(char) * TAILLE_BUFF * t +1); // concaténation des buffers, si la commande est saisie
		if (chaine == NULL) {
			strcpy (chaineTmp, buffer);
		} else {
			strcpy (chaineTmp, chaine);
			strcat (chaineTmp, buffer);
		}
		if (chaine != NULL)
			free (chaine);
		chaine = chaineTmp;
		chaine [TAILLE_BUFF * t] = '\0';

		if (chaine [strlen(chaine) - 1] == '\n') {
			chaine [strlen (chaine) - 1] = '\0';
			break;
		}
	}
	return chaine;
}

char** cmd_avec_args (char * commande, int * nbarg)
{
	int i;
	*nbarg = 0;
	char** argv = NULL;
	char* arg = strtok(commande," ");

	while (arg != NULL) {
		char ** argvTmp = (char **) malloc ((*nbarg + 1) * sizeof(char *)); // agrandissement du tableau d'argument
		for(i = 0; i <*nbarg; i ++) // copie de l'ancien tableau dans le nouveau
			argvTmp[i] = argv[i];
		if (argv != NULL)
			free(argv);
		argv = argvTmp;

		argv [*nbarg] = arg;
		*nbarg += 1;

		arg = strtok (NULL, " ");
	}
	return argv;
}

int interpreteur(char** argv, int nbarg) 
{
	if (argv == NULL)
		return 1;
	if (argv [0] == NULL)
		return 1;
		
	if (strcmp (argv [0], "exit") == 0) { // la commande exit
		return 0;
	}

	else if (strcmp (argv [0], "ls") == 0) { //la commande ls
		ls_cmd (&argv [1], nbarg-1);
		return 1;
	}

	else if (strcmp (argv [0], "cat") == 0) { //la commande cat
		cat_cmd (&argv [1], nbarg-1);
		return 1;
	}

	else if (strcmp (argv [0], "touch") == 0) { //la commande touch
		touch_cmd (& argv [1], nbarg-1);
		return 1;
	}
	
	else if (strcmp (argv [0], "echo") == 0) { //la commande echo
		echo_cmd (& argv [1], nbarg-1);
		return 1;
	}
	
	else if (strcmp (argv [0], "rm") == 0) { //la commande rm
		rm_cmd (& argv [1], nbarg-1);
		return 1;
	}
	
	else if (strcmp (argv [0], "mkdir") == 0) { //la commande mkdir
		mkdir_cmd (& argv [1], nbarg-1);
		return 1;
	}
	
	else if (strcmp (argv [0], "ln") == 0) { //la commande ln
		ln_mkdir (& argv [1], nbarg-1);
		return 1;
	}

	else if (strcmp (argv [0], "cp") == 0) { //la commande cp
		cp_cmd (& argv [1], nbarg-1);
		return 1;
		}

	else if (strcmp (argv [0], "mv") == 0) { //la commande mv
		mv_cmd (& argv [1], nbarg-1);
		return 1;
	}
		
	else if (strcmp (argv [0], "cd") == 0) { //la commande cd
		cd_cmd (& argv [1], nbarg-1);
		return 1;
	}

	else if (strcmp (argv [0], "sauv") == 0) { //la commande sauv
		sauv_cmd (& argv [1], nbarg-1);
		return 1;
	}

	else if (strcmp (argv [0], "recharger") == 0) { //la commande recharger
		recharger_cmd (& argv [1], nbarg-1);
		return 1;
	}
	
	else if (strcmp (argv [0], "aide") == 0) { //la commande aide
		aide_cmd(&argv [1], nbarg-1);
		return 1;
	}
	
	else if (strcmp (argv [0], "formater") == 0) { //la commande formater
		format_cmd ();
		return 1;
	}
	
	/*else if (strcmp (argv [0], "grep") == 0) { //la commande grep
		grep_cmd (&argv [1], nbarg-1);
		return 1;
	}*/

	else if (strcmp (argv [0], "df") == 0) { //la commande df
		df_cmd();
		return 1;
	}

	else
		printf ("La commande '% s' n'existe pas \n", argv [0]);


	return 1;
}

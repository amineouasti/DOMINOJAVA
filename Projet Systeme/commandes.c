#include "structure.h"

int ls_cmd(char** argv, int nbarg)
{
	if(nbarg < 1) {
		affiche_contenu_dossier(numInodeActu,0);
		return 1;
	}
		
	
	char mode = get_mode(argv[0]);	// 1er argument est le mode (-l -r)
	if(mode == -1) {	// si les modes entres sont faux, sinon, si c'est pas les modes ( soit un chemin) mode =0
		printf("Argument incorrect : '%s'\n",argv[0]);
		return 0;
	}
	if((mode != 0)&&(nbarg == 1)) {
		affiche_contenu_dossier(numInodeActu,mode);
		return 1;
	}

	
	
	int i = 1;	
	if(mode == 0)
		i = 0;
		
	while(i < nbarg) {	// parcours de tout les affichage demander ( si plusieur ont ete entree en meme temps)
		int inode = deplacement_arborescence(numInodeActu,argv[i]);	// deplacement jusqu au dossier voulu
		if(inode == -1){		// -1 si l acces est impossible ( chemin invalide, ou dossier inaccesible ( pas de droits))
			printf("Fichier introuvable : '%s'\n",argv[i]);
			return 0;
		}
		else
			affiche_contenu_dossier(inode,mode);		// affichage de celui-ci
		i++;
	}
	
	return 1;
}

int cat_cmd(char** argv, int nbarg)
{
	int j = 0;
	while(j < nbarg)
	{
		int inode = deplacement_arborescence(numInodeActu,argv[j]);	// chemin vers le fichier donné en argument
		if(inode == -1)		// (-1) si l acces est impossible
			printf("Fichier introuvable : '%s'\n",argv[j]);
		else
			affiche_contenu_texte(inode);
		j++;
	}
}

int touch_cmd(char** argv, int nbarg)
{
	int j =0;
	while(j < nbarg)
	{
		int inode;
		char * nomFic = recup_nom_fic_chemin(argv[j]);
		if(nomFic[0] != '\0') {
			if(strlen(argv[j])==strlen(nomFic))
				inode = numInodeActu;
			else {
				argv[j][strlen(argv[j])-strlen(nomFic)-1] = '\0';
				inode = deplacement_arborescence(numInodeActu,argv[j]);
			}
			if(inode == -1)		// -1 si l acces est impossible ( chemin invalide, ou dossier inaccesible ( pas le droit d'execution)
				printf("Dossier introuvable : '%s'\n",argv[j]);
			else
				creer_texte(inode, nomFic);
		} else {
			printf("Nom non saisie\n");
		}
		
		j++;
		free(nomFic);
	}
	return 1;
}

int echo_cmd(char** argv, int nbarg)
{
	if(nbarg < 1)
		return 0;
	
	int j = 1;
	int  inode = deplacement_arborescence(numInodeActu,argv[0]);
	if(inode == -1) {
		printf("Fichier introuvable\n");
		return 0;
	}
	while(j < nbarg)
	{
		ajout_texte(inode, argv[j]);
		if(j+1 < nbarg)
			ajout_texte(inode," ");
		j++;
	}
	return 1;
	
}

int rm_cmd(char** argv, int nbarg)
{
	int j = 0;
	while(j < nbarg)
	{
		int inode;
		char * nomFic = recup_nom_fic_chemin(argv[j]);
		if(nomFic[0] != '\0') {
			if(strlen(argv[j])==strlen(nomFic))
				inode = numInodeActu;
			else {
				argv[j][strlen(argv[j])-strlen(nomFic)-1] = '\0';
				inode = deplacement_arborescence(numInodeActu,argv[j]);
			}
			if(inode == -1)		// -1 si l acces est impossible ( chemin invalide, ou dossier inaccesible ( pas le droit d'execution)
				printf("Dossier introuvable : '%s'\n",argv[j]);
			else
				suppr_dossier_fichier(inode, nomFic);
		} else {
			printf("Nom non saisie\n");
		}
		free(nomFic);
		j++;
	}
	return 1;
}

int mkdir_cmd(char** argv, int nbarg)
{
	int j =0;
	while(j < nbarg)
	{
		int inode;
		char * nomFic = recup_nom_fic_chemin(argv[j]);
		if(nomFic[0] != '\0') {
			if(strlen(argv[j])==strlen(nomFic))
				inode = numInodeActu;
			else {
				argv[j][strlen(argv[j])-strlen(nomFic)-1] = '\0';
				inode = deplacement_arborescence(numInodeActu,argv[j]);
			}
			if(inode == -1)		// -1 si l acces est impossible ( chemin invalide, ou dossier inaccesible ( pas le droit d'execution)
				printf("Dossier introuvable : '%s'\n",argv[j]);
			else
				creer_dossier(inode, nomFic);
		} else {
			printf("Nom non saisie\n");
		}
		
		j++;
		free(nomFic);
	}
	return 1;
	
}

int ln_mkdir(char** argv, int nbarg)
{
	if(nbarg != 2) {
		printf("Nombre d'argument invalide\n");
		return 0;
	}
	
	int inode;
	int inodePere;
	char * nomFic = recup_nom_fic_chemin(argv[1]);
	if(nomFic[0] != '\0') {
		if(strlen(argv[1])==strlen(nomFic))
			inodePere = numInodeActu;
		else {
			argv[1][strlen(argv[1])-strlen(nomFic)-1] = '\0';
			inodePere = deplacement_arborescence(numInodeActu,argv[1]);
		}
		inode = deplacement_arborescence(numInodeActu,argv[0]);
		
		if((inodePere == -1)||(inode == -1))	// -1 si l acces est impossible ( chemin invalide, ou dossier inaccesible ( pas le droit d'execution)
			printf("Dossier introuvable : '%s'\n",argv[0]);
		else
			creer_lien(inodePere,inode,nomFic);
	} else {
			printf("Nom non saisie\n");
	}
		
	free(nomFic);
	return 1;
}

int mv_cmd(char** argv, int nbarg)
{
	if(nbarg != 2) {
		printf("Nombre d'argument invalide\n");
		return 0;
	}
	
	int inodeSrc;
	int inodeDest;
	char* nomFicSrc = recup_nom_fic_chemin(argv[0]);
	char* nomFicDest = recup_nom_fic_chemin(argv[1]);
	
	if(nomFicSrc[0] == '\0') {
		free(nomFicDest);
		free(nomFicSrc);
		return 0;
	}
	
	if(strlen(argv[0])==strlen(nomFicSrc))
		inodeSrc = numInodeActu;
	else {
		argv[0][strlen(argv[0])-strlen(nomFicSrc)-1] = '\0';
		inodeSrc = deplacement_arborescence(numInodeActu,argv[0]);
	}
	if(strlen(argv[1])==strlen(nomFicDest))
		inodeDest = numInodeActu;
	else {
		argv[1][strlen(argv[1])-strlen(nomFicDest)-1] = '\0';
		inodeDest = deplacement_arborescence(numInodeActu,argv[1]);
	}
	
	if(nomFicDest[0] == '\0') {
		free(nomFicDest);
		nomFicDest = nomFicSrc;
	}
	
	if((inodeSrc == -1)||(inodeDest == -1))	// -1 si l acces est impossible ( chemin invalide, ou dossier inaccesible ( pas le droit d'execution)
			printf("Dossier introuvable\n");
	else
		deplacer_fichier(inodeSrc,nomFicSrc,inodeDest,nomFicDest);
			
	if(nomFicDest != nomFicSrc)
		free(nomFicDest);
	free(nomFicSrc);
	return 1;
}

int cp_cmd(char** argv, int nbarg)
{
	
	if(nbarg != 2) {
		printf("Nombre d'argument invalide\n");
		return 0;
	}
	
	int inodeSrc;
	int inodeDest;
	char* nomFicSrc = recup_nom_fic_chemin(argv[0]);
	char* nomFicDest = recup_nom_fic_chemin(argv[1]);
	
	if(nomFicSrc[0] == '\0') {
		free(nomFicDest);
		free(nomFicSrc);
		return 0;
	}
	
	if(strlen(argv[0])==strlen(nomFicSrc))
		inodeSrc = numInodeActu;
	else {
		argv[0][strlen(argv[0])-strlen(nomFicSrc)-1] = '\0';
		inodeSrc = deplacement_arborescence(numInodeActu,argv[0]);
	}
	if(strlen(argv[1])==strlen(nomFicDest))
		inodeDest = numInodeActu;
	else {
		argv[1][strlen(argv[1])-strlen(nomFicDest)-1] = '\0';
		inodeDest = deplacement_arborescence(numInodeActu,argv[1]);
	}
	
	if(nomFicDest[0] == '\0') {
		free(nomFicDest);
		nomFicDest = nomFicSrc;
	}
	
	if((inodeSrc == -1)||(inodeDest == -1))	// -1 si l acces est impossible ( chemin invalide, ou dossier inaccesible ( pas le droit d'execution)
			printf("Dossier introuvable\n");
	else
			copie_fichier(inodeSrc,nomFicSrc,inodeDest,nomFicDest);
			
	if(nomFicDest != nomFicSrc)
		free(nomFicDest);
	free(nomFicSrc);
	return 1;
	
}

int cd_cmd(char** argv, int nbarg)
{
	if(nbarg != 1) {
		printf("Nombre d'argument invalide\n");
		return 0;
	}
	
	int inode = deplacement_arborescence(numInodeActu,argv[0]);
	if(test_inode_dossier(inode))
	{
		numInodeActu = inode;
	} else
		printf("Chemin invalide\n");
	
	return 0;
}

int df_cmd()
{
	calc_succ_inode_libre();  
	calc_succ_bloc_libre();
	return 0;
}

int sauv_cmd()
{
	sauvegarder_disque();
}
int recharger_cmd()
{
	charger_disque();
}
int format_cmd()
{
	formater_disque();
}

char* recup_nom_fic_chemin(char* chemin) 
{
	char* nom = (char*)malloc(sizeof(char)*FIC_NOM_TAILLE);

	int i;
	for(i = strlen(chemin); i >= 0; i--) {
		if(chemin[i] == '/')
			break;
	}
	if((chemin[i] == '/')||(i < 0))
		i++;
	int j;
	for(j =0; (chemin[i] != '\0')&&(i < FIC_NOM_TAILLE -1); i++) {
		
		nom[j] = chemin[i];
		j++;
	}
	nom[j] = '\0';

	return nom;
}

int deplacement_arborescence(int numIDossier, char* chemin)	 
{
    
    if(!test_inode_fichier(numIDossier))
		return -1;
	
    if(test_inode_lien(numIDossier))
		numIDossier = deplacement_lien(numIDossier);
   
    if(!test_inode_dossier(numIDossier))
		return -1;
    
    if(!test_droits_inode(numIDossier,'x')) {
		printf("Ouverture impossible : '%s'\n",chemin);
		return -1;
	}
    
    char _chemin[FIC_NOM_TAILLE];
    int i;
    for(i =0; (i < FIC_NOM_TAILLE)&&(chemin[i] != '/')&&(chemin[i] != '\0'); i++) 
    {
		_chemin[i] = chemin[i];
	}
  
    _chemin[i] = '\0';
    int numI = recherche_inode_nom(numIDossier,_chemin);
    if(numI == -1) {
		printf("Fichier introuvable : '%s'\n",_chemin);
		return -1;
	}
	if(chemin[i] == '\0')
		return numI;
	else
		return deplacement_arborescence(numI,&chemin[i+1]);
}

int get_mode(char* string)
{	
	if (string[0] == '-')
	{
		if((strcmp(string,"-lr") == 0)||(strcmp(string,"-rl") == 0))
			return 3;
		if(strcmp(string,"-l") == 0)
			return 1;
		if(strcmp(string,"-r") == 0)
			return 2;
		return -1;
	}
	else 
		return 0;
}

int aide_cmd(char** argv, int nbarg)
{
	if(nbarg != 1) {
		printf("Nombre d'argument invalide\n");
		return 0;
	}
	if (strcmp (argv [0], "ls") == 0){
		printf("Aide de la commande ls:\n");
		printf("Syntaxe : ls [OPTION]... [Fichier/Dossier]...\n");
		printf("lister les informations sur les FICHIERS (le répertoire courant par défaut).\n");
		printf("Option:\n");
		printf("\t-a\tn'ignorez pas les entrées commençant par '.'\n");
		printf("\t-l\tutiliser un format de liste longue (plus de détails sur le fichier)\n");
		return 1;
	}
	else if (strcmp (argv [0], "cat") == 0){
		printf("Aide de la commande ls:\n");
		printf("Syntaxe : cat [OPTION]... [Fichier/Dossier]...\n");
		printf("concaténer le(s) fichier(s) vers la sortie standard.\n");
		return 1;
	}
	return 0;
}
/*int grep_cmd(char** argv, int nbarg){
	FILE *fp;
	char fline[100];
	char *newline;
	int i,count=0,occ=0;
	int inode = deplacement_arborescence(numInodeActu,argv[1]);
	fp=affiche_contenu_texte(inode);

	if(nbarg != 2) {
		printf("Nombre d'argument invalide\n");
		return 0;
	}

	while(fgets(fline,100,fp)!=NULL)
	{
		count++;
    		if(newline=strchr(fline,'\n'))
        		*newline='\0';
    		if(strstr(fline,argv[0])!=NULL)
    		{
        		printf("%s %d %s",argv[0],count,fline);
    			occ++;  
    		}
	}

	printf("\n Occurence= %d",occ);

	return 1;
}
*/

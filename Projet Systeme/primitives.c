#include "structure.h"


int charger_disque()
{
	disk = open(NOM_DISQUE,O_RDWR);
	if(disk == -1)
		return 0;
	root = ROOT;
	calc_succ_inode_libre();  
	calc_succ_bloc_libre();
	return 1;
}

int formater_disque()
{
	printf("Formatage du disque ? ");
	if(!confirmation())
		return 0;

	close(disk);
	creation_disque();
	charger_disque();
	return 1;
}

void creation_disque()
{ 
	
	disk = open(NOM_DISQUE,O_RDWR | O_CREAT | O_TRUNC,0644);
	
	int i;
	for(i =0; i < NB_INODE; i++)
		ecraser_inode(i,creer_inode_vide());

	InfoBloc info;
	info.libre = 0;
	info.posBloc = sizeof(Inode)*NB_INODE +sizeof(InfoBloc)*NB_BLOC;
	
	for(i =0; i < NB_BLOC; i++)
	{
		ecraser_infoBloc(i,info);
		init_bloc(info.posBloc);
		info.posBloc += sizeof(Bloc);
	}
	
	numInodeLibre = 0;
	numBlocLibre = 0;
	
	creer_rep_racine();
	numInodeActu = root;
	
	close(disk);
	disk = 0;
}

int creer_rep_racine()
{
	int numInode = ajouter_inode(creer_inode(DOS,7));
	ajout_dossier_fichier(numInode,numInode,".");
	ajout_dossier_fichier(numInode,numInode,"..");
	increm_lien(numInode);
	root = numInode;
	return numInode;
}

void sauvegarder_disque()
{
	close(disk);
	charger_disque();
}


// ********** FICHIER **********

int deplacer_fichier(int numIDossier, char* nom, int numIDossierDest, char* nomDest)
{
	numIDossier = deplacement_lien(numIDossier);
	numIDossierDest = deplacement_lien(numIDossierDest);
	
	
	int numI = recherche_inode_nom(numIDossier,nom);
	if(numI == -1)
		return 0;
		
	ajout_dossier_fichier(numIDossierDest,numI,nomDest);
	suppr_dossier_fichier(numIDossier,nom);
	
	return 1;
}

int copie_fichier(int numIDossier, char* nom, int numIDossierDest, char* nomDest)
{
	int numISrc = recherche_inode_nom(numIDossier,nom);
	Inode inodeSrc = lire_inode(numISrc);
	if(inodeSrc.typeFichier == 0)
		return 0;
	
	if((strcmp(nom,".")==0)||(strcmp(nom,"..") == 0))
		return 0;
	
	if(test_inode_lien(numISrc)) {
		int numIDest = creer_lien(numIDossierDest,inodeSrc.bloc[0],nomDest);
		changer_droits(numIDest,inodeSrc.droits);
	}
	if(test_inode_dossier(numISrc)) {
		int numIDest = creer_dossier(numIDossierDest,nomDest);
		copie_fichier_rep(numISrc,numIDest);
		changer_droits(numIDest,inodeSrc.droits);
	}
	if(test_inode_texte(numISrc)) {
		int numIDest = creer_texte(numIDossierDest,nomDest);
		copie_fichier_txt(numISrc,numIDest);
		changer_droits(numIDest,inodeSrc.droits);
	}
	
	return 1;
}

int copie_fichier_rep(int numISrc, int numIDest)
{
	Inode inodeSrc = lire_inode(numISrc);
	
	int i;
	for(i =0; i < inodeSrc.taille; i++)
	{
		int j;
		for(j =0 ;j < sizeof(Bloc)/sizeof(InfoFic); j++) {
			InfoFic info = lire_infoFic(numISrc,i,j);
			if(!((info.inode < 0)||(info.inode >= NB_INODE)
				||(info.nom[0] == '\0')))
				copie_fichier(numISrc,info.nom,numIDest,info.nom);
		}
	}
	
	return 1;
}


int copie_fichier_txt(int numISrc, int numIDest)
{
	
	Inode inodeSrc = lire_inode(numISrc);
	Inode inodeDest = lire_inode(numIDest);
	
	int i;
	for(i =0; i < inodeSrc.taille; i++)
	{
		if(!ajout_bloc(numIDest)) 
			return 0;
		inodeDest = lire_inode(numIDest);
		ecraser_bloc(inodeDest.bloc[i],lire_bloc(inodeSrc.bloc[i]));
	}
	
	return 1;
}

int recherche_inode_nom(int numI, char* nom)
{
	numI = deplacement_lien(numI);
	
	if(!test_inode_dossier(numI)) {
		return -1;
	}

	int i, j;
	for(i =0; i < NB_BLOC_MAX; i++) {
		for(j =0; j < sizeof(Bloc)/sizeof(InfoFic); j++) {
			InfoFic info = lire_infoFic(numI,i,j);
			if(strcmp(nom,info.nom) == 0) {
				return info.inode;
			}
		}
	}
	return -1;
}

// ********** FICHIER : LIEN **********

int creer_lien(int numIPere, int numIFic, char *nom)
{
	if(!test_inode_fichier(numIFic))
		return -1;
	if(!test_inode_dossier(numIPere)) {
		return -1;
	}
	if(!test_droits_inode(numIPere,'w')) {
		printf("Pas de droit d'ecriture dans le fichier\n");
		return -1;
	}
	
	int numI = ajouter_inode(creer_inode(LIEN,7));
	Inode inode = lire_inode(numI);
	inode.bloc[0] = numIFic;
	ecraser_inode(numI,inode);
	increm_lien(numIFic);
	
	ajout_dossier_fichier(numIPere,numI,nom);
	
	return numI;
}

int deplacement_lien(int numI)
{
	while(test_inode_lien(numI)) {
		if(test_droits_inode(numI,'x'))
			numI = lire_inode(numI).bloc[0];
		else {
			printf("Lien non executable\n");
			return -1;
		}
	}
	return numI;
}


// ********** FICHIER : TEXTE **********

int creer_texte(int numIPere, char *nom)
{
	numIPere = deplacement_lien(numIPere);
	
	if(!test_inode_dossier(numIPere)) {
		printf("Dossier pere invalide\n");
		return -1;
	}
	if(!test_droits_inode(numIPere,'w')) {
		printf("Pas de droit d'ecriture dans le fichier\n");
		return -1;
	}
	
	int numI= ajouter_inode(creer_inode(TXT,6));
	ajout_dossier_fichier(numIPere,numI,nom);
	
	return numI;
}

int ajout_texte(int numI, char* texte)
{
	numI = deplacement_lien(numI);
	
	if(!test_inode_texte(numI)) {
		printf("Fichier invalide\n");
		return 0;
	}
	if(!test_droits_inode(numI,'w')) {
		printf("Pas les droits d'ecriture dans le fichier\n");
		return 0;
	}
	
	return _ecriture_texte(numI,texte);
}

int _ecriture_texte(int numI, char* texte)
{
	int n =0, nb =0;
	int i;
	
	Inode fichier = lire_inode(numI);
	
	for(i =0; i < NB_BLOC_MAX; i++) {
		
		if(n >= strlen(texte))
			break;
		
		if(fichier.bloc[i] == -1) {
			ajout_bloc(numI);
			fichier = lire_inode(numI);
		}
		
		if(bloc_espace_restant(fichier.bloc[i]) <= 0)
			continue;
			
		nb = bloc_espace_restant(fichier.bloc[i]);
		if((n +nb) > strlen(texte))
			nb = strlen(texte) -n +1;
		
		lseek(disk,fichier.bloc[i] + bloc_espace_utilise(fichier.bloc[i]),0);
		write(disk,&texte[n],nb);
		n += nb;
	}
	
	if(i >= NB_BLOC_MAX) {
		printf("Taille max du fichier atteint\n");
		return 0;
	}
	return 1;
}

// ********** FICHIER : DOSSIER **********

int creer_dossier(int numIPere, char* nom)
{
	numIPere = deplacement_lien(numIPere);
	
	if(!test_inode_dossier(numIPere)) {
		printf("Dossier pere invalide\n");
		return -1;
	}
	if(!test_droits_inode(numIPere,'w')) {
		printf("Pas de droit d'ecriture dans le fichier\n");
		return -1;
	}
	
	int numI = ajouter_inode(creer_inode(DOS,7));
	ajout_dossier_fichier(numI,numI,".");
	ajout_dossier_fichier(numI,numIPere,"..");
	ajout_dossier_fichier(numIPere,numI,nom);
	
	return numI;
}

int ajout_dossier_fichier(int numIDossier,int numIFichier,char* nom)
{
	numIDossier = deplacement_lien(numIDossier);
	
	if(!test_inode_dossier(numIDossier)) {
		printf("Dossier invalide\n");
		return 0;
	}
	if(!test_droits_inode(numIDossier,'w')) {
		printf("Pas de droit d'ecriture dans le fichier\n");
		return 0;
	}
	
	if(!test_inode_fichier(numIFichier)) {
		printf("Ajout d'un fichier invalide\n");
		return 0;
	}
	
	InfoFic infoFichier = creer_infoFic(numIFichier,nom);
	if(strcmp(infoFichier.nom,"") == 0)
		return 0;
		
	return _ajout_dossier_infoFic(numIDossier,infoFichier);
}

int _ajout_dossier_infoFic(int numI, InfoFic info)
{
	if((info.inode < 0)||(info.inode >= NB_INODE)||(info.nom[0] == '\0'))
		return 0;
	
	Inode dossier = lire_inode(numI);
	
	int t = strlen(info.nom);
	int n = 0;
	int ii = -1;
	int i, j;
	for(i = 0; i < NB_BLOC_MAX; i++) {
		if(dossier.bloc[i] == -1) {
			ajout_bloc(numI);
			dossier = lire_inode(numI);
		}
		
		InfoFic tmp;
		for(j =0; j < sizeof(Bloc)/sizeof(InfoFic); j++) {
			tmp = lire_infoFic(numI,i,j);
			if(strcmp(tmp.nom,info.nom) == 0)
			{
				printf("'%s' existe deja\nRemplacer ?",info.nom);
				if(!confirmation()) {
					n++;
					info.nom[t] = '\0';
					char* tmpC = (char*)malloc(FIC_NOM_TAILLE+10);
					sprintf(tmpC,"%s%d",info.nom,n);
					test_nom_fichier(tmpC,info.nom);
					i = -1;
					free(tmpC);
					break;
				}
				else
					ii = j;
				
			} else if((tmp.inode < 0)||(tmp.inode >= NB_INODE)||(tmp.nom[0] == '\0')) {
				ii = j;
				break;
			}
		}
		if(ii != -1)
			break;
	}
	
	if(ii == -1) {
		printf("Dossier rempli\n");
		return 0;
	}
	
	
	ecraser_infoFic(numI,i,ii,info);
	if((numI != info.inode)&&(strcmp(info.nom,"..") != 0))
		increm_lien(info.inode);

	return 1;
}

InfoFic creer_infoFic(int numI, char *nom)
{
	InfoFic info;
	info.inode = numI;
	test_nom_fichier(nom,info.nom);
	return info;
}

InfoFic creer_infoFic_vide()
{
	return creer_infoFic(-1,"");
}

int ecraser_infoFic(int numI, int numB, int numF, InfoFic info)
{
	if(!test_inode_dossier(numI))
		return 0;
	if(!test_num_bloc(numB))
		return 0;
	if(!test_num_fic(numF))
		return 0;
	
	Inode inode = lire_inode(numI);
	
	if(inode.bloc[numB] == -1)
		return 0;

	lseek(disk,inode.bloc[numB] +sizeof(InfoFic)*numF,0);
	write(disk,&info,sizeof(info));
	
	return 1;
}

int decrem_lien(int numI)
{
	if(!test_inode_fichier(numI)) {
		printf("Fichier invalide\n");
		return 0;
	}
	
	
	
	Inode inode = lire_inode(numI);
	if(inode.typeFichier == LIEN)
		decrem_lien(inode.bloc[0]);
	
	inode.nbLien --;
	if(inode.nbLien <= 0) {
			if(vider_inode(numI))
				inode = creer_inode_vide();
			else
				inode.nbLien = 1;
	}
	return ecraser_inode(numI,inode);
	
}
	
int increm_lien(int numI)
{
	if(!test_inode_fichier(numI)) {
		printf("Fichier invalide\n");
		return 0;
	}
	
	Inode inode = lire_inode(numI);
	inode.nbLien ++;
	return ecraser_inode(numI,inode);
}

InfoFic lire_infoFic(int numI,int numB, int numF)
{
	if(!test_num_fic(numF))
		return creer_infoFic_vide();
	if(!test_num_bloc(numB))
		return creer_infoFic_vide();
	
	if(!test_inode_dossier(numI))
		return creer_infoFic_vide();
	
	Inode inode = lire_inode(numI);
	if(inode.bloc[numB] == -1)
		return creer_infoFic_vide();

	InfoFic info;
	lseek(disk,inode.bloc[numB] +sizeof(InfoFic)*numF,0);
	read(disk,&info,sizeof(info));
	return info;
}

int suppr_dossier_fichier(int numIDossier, char* nom)
{
	numIDossier = deplacement_lien(numIDossier);
	
	if(!test_inode_dossier(numIDossier)) {
		printf("Dossier invalide : %d\n",numIDossier);
		return 0;
	}
	if(!test_droits_inode(numIDossier,'w')) {
		printf("Pas de droit de ecriture dans le fichier\n");
		return 0;
	}
	
	if((strcmp(nom,".")==0)||(strcmp(nom,"..")==0))
		return 0;
	
	int i, j;
	for(i =0; i < NB_BLOC_MAX; i++) {
		for(j =0; j < sizeof(Bloc)/sizeof(InfoFic); j++) {
			InfoFic info = lire_infoFic(numIDossier,i,j);
			if(strcmp(nom,info.nom) == 0) {
				
				if(test_inode_dossier(info.inode)) {
					if(lire_infoFic(info.inode,0,2).nom[0] != '\0') {
						printf("Dossier non vide\n");
						return 0;
					}
				}
				decrem_lien(info.inode);
				ecraser_infoFic(numIDossier,i,j,creer_infoFic_vide());
				defragmenter_dossier(numIDossier);
				return 1;
			}
		}
	}
	
	return 0;
}

void defragmenter_dossier(int numI)
{
	if(!test_inode_dossier(numI))
		printf("Fichier non dossier\n");
	
	int i, j, nB = -1, nF =-1;
	for(i =0; i < NB_BLOC_MAX; i++) {
		for(j =0; j < sizeof(Bloc)/sizeof(InfoFic); j++) {
			InfoFic info = lire_infoFic(numI,i,j);
			if((info.nom[0] == '\0')&&(nB == -1)) {
				nB = i;
				nF = j;
			} else if((nB != -1)&&(info.nom[0] != '\0')) {
				ecraser_infoFic(numI,i,j,creer_infoFic_vide());
				ecraser_infoFic(numI,nB,nF,info);
				i = nB;
				j = nF;
				nB = -1;
				nF = -1;
			}
		}
	}
	for(i =0; i < NB_BLOC_MAX; i++) {
		if(test_dossier_bloc_vide(numI,i))
			suppr_bloc(numI,i);	
	}
}

// ********** BLOC **********

int test_dossier_bloc_vide(int numI, int numB)
{
	if(!test_inode_dossier(numI))
		return 0;
	
	int i;
	for(i =0; i < sizeof(Bloc)/sizeof(InfoFic); i++) {
		InfoFic info = lire_infoFic(numI,numB,i);
		if(info.nom[0] != '\0')
			return 0;
	}
	return 1;
	
}

int bloc_espace_utilise(int pos)
{
	Bloc bloc = lire_bloc(pos);
	int i;
	
	for(i = BLOC_TAILLE-1; (bloc.donnees[i] == '\0')&&(i >= 0); i--);
	
	return i +1;
}

int bloc_espace_restant(int pos)
{
	Bloc bloc = lire_bloc(pos);
	int i;
	
	for(i = BLOC_TAILLE-1; (bloc.donnees[i] == '\0')&&(i >= 0); i--);
	
	return BLOC_TAILLE -i -1;
}

int ajout_bloc(int numI)
{
	if(numBlocLibre == -1) {
		printf("Aucun bloc disponible\n");
		return 0;
	}
	if(!test_inode_fichier(numI)) {
		printf("Fichier invalide\n");
		return 0;
	}
	Inode inode = lire_inode(numI);
	
	if(inode.taille >= NB_BLOC_MAX) {
		printf("Taille max atteinte\n");
		return -1;
	}
	
	InfoBloc infoBloc = lire_infoBloc(numBlocLibre);
	init_bloc(infoBloc.posBloc);
	
	inode.bloc[inode.taille] = infoBloc.posBloc;
	inode.taille += 1;
	
	change_infoBloc(numBlocLibre);
	calc_succ_bloc_libre();
	
	ecraser_inode(numI,inode);

	return 1;
}

void init_bloc(int pos)
{
	int i;
	Bloc bloc = lire_bloc(pos);
	for(i =0; i < BLOC_TAILLE; i++)
		bloc.donnees[i] = 0;
	ecraser_bloc(pos,bloc);
}

int ecraser_bloc(int pos, Bloc bloc)
{
	if(pos < sizeof(Inode)*NB_INODE + sizeof(InfoBloc)*NB_BLOC)
		return 0;
	
	lseek(disk,pos,0);
	write(disk,&bloc,sizeof(bloc));
	
	return 1;
}

Bloc lire_bloc(int pos)
{
	Bloc bloc;
	
	lseek(disk,pos,0);
	read(disk,&bloc,sizeof(bloc));

	return bloc;
}

InfoBloc lire_infoBloc(int num)
{
	InfoBloc info;
	
	int posTable = sizeof(Inode)*NB_INODE;
	lseek(disk,posTable +sizeof(InfoBloc)*num,0);
	read(disk,&info,sizeof(info));

	return info;
}

int ecraser_infoBloc(int num, InfoBloc info)
{
	if((0 > num)||(num >= NB_BLOC))
		return 0;
	
	int posTable = sizeof(Inode)*NB_INODE;
	lseek(disk,posTable +sizeof(InfoBloc)*num,0);
	write(disk,&info,sizeof(info));
	
	return 1;
}

int change_infoBloc(int num)
{
	if((0 > num)||(num >= NB_BLOC))
		return 0;
	
	InfoBloc info = lire_infoBloc(num);
	info.libre = 1 - info.libre;
	ecraser_infoBloc(num,info);
}

int recup_infoBloc(int pos)
{
	int i;
	for(i =0; i < NB_BLOC; i++) {
		InfoBloc info = lire_infoBloc(i);
		if(info.posBloc == pos)
			return i;
	}
	return -1;
}

void calc_succ_bloc_libre()
{
	int i , cpt =0;
	int posTable = sizeof(Inode)*NB_INODE;
	for(i = numBlocLibre; cpt < NB_BLOC; i++) {
		char l;
		lseek(disk,posTable +sizeof(InfoBloc)*i,0);
		read(disk,&l,sizeof(l));
		if(l == 0) {
			numBlocLibre = i;
			break;
		}
		if(i >= NB_BLOC) {
			i = 0;
		}
		cpt++;
	}
	if(cpt >= NB_BLOC)
		numBlocLibre = -1;
}

int suppr_bloc(int numI,int numB)
{
	if(!test_inode_fichier(numI))
		return 0;
	if(!test_droits_inode(numI,'w'))
		return 0;
	if(!test_num_bloc(numB))
		return 0;
	
	Inode inode = lire_inode(numI);

	change_infoBloc(recup_infoBloc(inode.bloc[numB]));
	inode.bloc[numB] = -1;
	
	return 1;
}


// ********** INODE **********

int changer_droits(int numI, char droits)
{
	if(!test_droits_valide(droits)) {
		printf("Droits invalide\n");
		return 0;
	}
	
	if(!test_inode_fichier(numI)) {
		printf("Fichier invalide\n");
		return 0;
	}
	
	Inode inode = lire_inode(numI);
	inode.droits = droits;
	ecraser_inode(numI,inode);
	
	return 1;
}

int get_posBloc(int numI,int numB)
{
	if(!test_num_bloc(numB))
		return -1;
	Inode inode = lire_inode(numI);
	return inode.bloc[numB];
}

int vider_inode(int numI)
{
	if(test_inode_lien(numI))
		return 1;
	
	int i;
	for(i =0; i < NB_BLOC_MAX; i++) 
		suppr_bloc(numI,i);
	
	return 1;
}

int ajouter_inode(Inode inode)
{
	if(numInodeLibre != -1) {
		ecraser_inode(numInodeLibre,inode);
		
		int ret = numInodeLibre;		
		calc_succ_inode_libre();
		
		return ret;
	}
	else {
		printf("Plus d'inode libre\n");
		return -1;
	}
}

int ecraser_inode(int numI, Inode inode) 
{
	if(!test_inode(numI)) {
		printf("Numero d'inode invalide : %d\n",numI);
		return 0;
	}
	
	lseek(disk,sizeof(Inode)*numI,0);
	write(disk,&inode,sizeof(inode));
	
	return 1;
}

void calc_succ_inode_libre()
{
	int i , cpt =0;
	for(i = numInodeLibre; cpt < NB_INODE; i++) {
		char l;
		lseek(disk,sizeof(Inode)*i,0);
		read(disk,&l,sizeof(l));
		if(l == 0) {
			numInodeLibre = i;
			break;
		}
		if(i >= NB_INODE) {
			i = 0;
		}
		cpt++;
	}
	if(cpt >= NB_INODE)
		numInodeLibre = -1;
}

Inode creer_inode(int type, char droits) 
{
	Inode inode = creer_inode_vide();
	
	if(test_droits_valide(droits))
		inode.droits = droits;
	else {
		printf("Droits invalide\n");
		return inode;
	}
	
	if((0 < type)&&( type <= 3))
		inode.typeFichier = type;
	else {
		printf("Type invalide\n");
		return inode;
	}
	
	inode.nbLien = 0;
	
		
	return inode;
}

Inode creer_inode_vide() 
{
	Inode inode;
	
	inode.typeFichier = 0;
	inode.droits = 0;
	inode.nbLien = 0;
	inode.taille = 0;
	
	int i;
	for(i =0; i < NB_BLOC_MAX; i++)
		inode.bloc[i] = -1;
		
	return inode;
}

Inode lire_inode(int numI)
{
	if(!test_inode(numI))
		return creer_inode_vide();
	
	Inode inode;
	
	lseek(disk,sizeof(Inode)*numI,0);
	read(disk,&inode,sizeof(inode));
		
	return inode;
}

// ********** TESTS **********

int test_num_bloc(int numB)
{
	if((0 <= numB)&&(numB < NB_BLOC_MAX))
		return 1;
	else 
		return 0;
}

int test_num_fic(int numF)
{
	if((0 <= numF)&&(numF < sizeof(Bloc)/sizeof(InfoFic)))
		return 1;
	else 
		return 0;
}

int test_inode(int numI)
{
	if((0 <= numI)&&(numI < NB_INODE)) {
		return 1;
	}else
		return 0;
}

int test_inode_fichier(int numI)
{
	if(!test_inode(numI))
		return 0;
	Inode inode = lire_inode(numI);
	if(inode.typeFichier != 0)
		return 1;
	else
		return 0;
}

int test_inode_texte(int numI)
{
	if(!test_inode_fichier(numI))
		return 0;
	Inode inode = lire_inode(numI);
	if(inode.typeFichier == TXT)
		return 1;
	else
		return 0;
}

int test_inode_dossier(int numI)
{
	if(!test_inode_fichier(numI))
		return 0;
	Inode inode = lire_inode(numI);
	if(inode.typeFichier == DOS)
		return 1;
	else
		return 0;
}

int test_inode_lien(int numI)
{
	if(!test_inode_fichier(numI))
		return 0;
	Inode inode = lire_inode(numI);
	if(inode.typeFichier == LIEN)
		return 1;
	else
		return 0;
}

int test_nom_fichier(char* nom, char sortie[FIC_NOM_TAILLE])
{
	int i;
	for(i =0; i < FIC_NOM_TAILLE; i++) {
		if((nom[i] == ' ')||(nom[i] == '/')) {
			sortie[0] = '\0';
			break ;
		}
		if(nom[i] == '\0')
			break;
		sortie[i] = nom[i];
	}
	for(; i< FIC_NOM_TAILLE;i++)
		sortie[i] = '\0';
		
	if(sortie[0] == '\0')
		return 0;
	else
		return 1;
}

int test_droits_inode(int numI,char d)
{
	if(!test_inode_fichier(numI))
		return 0;
	
	Inode inode = lire_inode(numI);
	switch(d) {
		case 'r':
			return _test_lecture(inode.droits);
			break;
		case 'w':
			return _test_ecriture(inode.droits);
			break;
		case 'x':
			return _test_execution(inode.droits);
			break;
		default:
			return 0;
	}
}

int _test_lecture(int droits)
{
	return (droits>>2)&1;
}

int _test_ecriture(int droits)
{
	return (droits>>1)&1;
}

int _test_execution(int droits)
{
	return droits&1;
}

int test_droits_valide(char p) 
{
	if((0 <= p)&&(p <=7))
		return 1;
	else
		return 0;
}

int confirmation()
{
	char c;
	
	printf(" (o/n) ");
	do {
		scanf(" %c",&c);
	}while((c != 'n')&&(c != 'o'));
	
	if(c == 'o')
		return 1;
	else 
		return 0;
}

int test_ls_r(char mode)
{
	return (mode>>1)&1;
}

int test_ls_l(char mode)
{
	return mode&1;
}

int test_pere(int numI, int numPere)
{
	if(!test_inode_dossier(numI))
		return 0;
	if(!test_inode_dossier(numPere))
		return 0;
	
	do {
		numI = recherche_inode_nom(numI,"..");
		if(numI == numPere)
			return 1;
		
	}while((numI != root)&&(numI != -1));
	
	return 0;
}



// ********** AFFICHAGE **********

int affiche_contenu_texte(int numI)
{
	numI = deplacement_lien(numI);
	
	if(!test_inode_texte(numI))
		return 0;
	if(!test_droits_inode(numI,'r'))
		return 0;
	
	Inode inode = lire_inode(numI);
	int i;
	for(i =0; i < inode.taille; i++) {
		affiche_bloc(lire_bloc(inode.bloc[i]));
	}
	printf("\n");
	return 1;
}

int affiche_contenu_dossier(int numI, char mode)
{
	if(!test_inode_dossier(numI))
		return 0;
	if(!test_droits_inode(numI,'r')) {
		printf("Lecture non autorisee\n");
		return 0;
	}
	
	Inode inode = lire_inode(numI);
	int i;
	for(i =0; i < inode.taille; i++) {
		int j;
		for(j =0; j < sizeof(Bloc)/sizeof(InfoBloc); j++) {
			InfoFic info = lire_infoFic(numI,i,j);
			if(!((info.inode < 0)||(info.inode >= NB_INODE)
				||(info.nom[0] == '\0')))
			{
				if((test_ls_l(mode))&&(strcmp(info.nom,".")!=0)&&(strcmp(info.nom,"..")!=0)) {
					affiche_inode(lire_inode(info.inode));
					printf("%s\n",info.nom);
				} else if(!test_ls_l(mode))
					printf("%s ",info.nom);
			}
		}
		if(test_ls_r(mode)) {
			for(j =0; j < sizeof(Bloc)/sizeof(InfoBloc); j++) {
				InfoFic info = lire_infoFic(numI,i,j);
				if(!((info.inode < 0)||(info.inode >= NB_INODE)
					||(info.nom[0] == '\0')))
				{
					if((test_inode_dossier(info.inode))&&(strcmp(info.nom,".")!=0)&&(strcmp(info.nom,"..")!=0)&&(!test_pere(numI,info.inode))) {
						printf("\n%s :\n",info.nom);
						if(!test_droits_inode(info.inode,'x')) {
							printf("Ouverture non autorisee\n");
						}else
							affiche_contenu_dossier(info.inode,mode);
					}	
				}
			}
		}
		printf("\n");
	}
	
	return 1;
}

void affiche_contenu_disque()
{
	
	
	int i;
	
	for(i =0; i < NB_INODE; i++) {
		affiche_inode_tout(lire_inode(i));
	}
	printf("\n");
	
	for(i =0; i< NB_BLOC; i++) {
		InfoBloc info = lire_infoBloc(i);
		printf("%d %d\n",info.libre,info.posBloc);
	}
	printf("\n");
	
	
	for(i =0; i < NB_INODE; i++) {
		if(test_inode_dossier(i)) {
			affiche_contenu_dossier(i,1);
			printf("\n");
		}
		else if(test_inode_texte(i)) {
			affiche_contenu_texte(i);
			printf("\n");
		}	
	}
	
}

void affiche_inode_tout(Inode inode)
{
	printf("%d %d %d %d",inode.typeFichier,inode.droits,inode.nbLien,inode.taille);
	int i;
	for(i =0; i< NB_BLOC_MAX ;i++)
		printf(" %d",inode.bloc[i]);
	printf("\n");
}

void affiche_bloc(Bloc bloc)
{
	if(bloc.donnees[0] == '\0')
		return;
	write(1,&bloc,sizeof(bloc));	
}

void affiche_inode(Inode inode)
{	
	if(!_test_lecture(inode.droits)) {
		printf("Lecture non autorisee\n");
		return ;
	}
		
	affiche_typeFichier(inode.typeFichier);
	affiche_droits(inode.droits);
	printf(" %d %d ",inode.nbLien,inode.taille);	
}

void affiche_droits(char droits)
{
	printf("%c",((droits>>2)&1)?'r':'-');
	printf("%c",((droits>>1)&1)?'w':'-');
	printf("%c",((droits)&1)?'x':'-');
}

void affiche_typeFichier(int type)
{
	switch(type) {
		case DOS:
			printf("d");
			break;
		case TXT:
			printf("-");
			break;
		case LIEN:
			printf("l");
			break;
		default:
			return;
		}
}


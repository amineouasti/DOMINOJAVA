/*################################## SGF ###############################################*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define NB_INODE 30
#define NB_BLOC 100

#define NB_BLOC_MAX 10

#define BLOC_TAILLE (1024)

#define FIC_NOM_TAILLE (28)

#define NOM_DISQUE "disque"
#define ROOT 0
 
#define TXT 1
#define DOS 2
#define LIEN 3

int disk;
int root;
int numInodeLibre;		// -1 si plus de inode libre
int numBlocLibre;		// -1 si plus de bloc libre
int numInodeActu;

typedef struct
{
	char typeFichier; // 1 = texte, 2 = dossier, 3 lien , 0 = libre ...
	char droits; // (rwx)
	int nbLien;
	int taille;
	int bloc[NB_BLOC_MAX];	// -1 si le bloc n'est pas utilisé, "pointeur" vers le(s) bloc(s)
}Inode;

typedef struct
{
	char libre;	// 0 oui,1 non
	int posBloc;
}InfoBloc;

typedef struct
{
	char donnees[BLOC_TAILLE];
}Bloc;

typedef struct
{
	int inode;
	char nom[FIC_NOM_TAILLE];
}InfoFic;


// "Primitives" de SGF
int creer_texte(int numIPere, char *nom);	// touch
int ajout_texte(int numI, char* texte);		// echo 
int creer_lien(int numIPere, int numIFic, char *nom); //ln
int creer_dossier(int numIPere, char* nom);	// mkdir
int suppr_dossier_fichier(int numIDossier, char* nom);	//rm

int affiche_contenu_texte(int numI);		// cat
int affiche_contenu_dossier(int numI, char mode);	// ls

int deplacer_fichier(int numIDossier, char* nom, int numIDossierDest, char* nomDest);	//mv
int copie_fichier(int numIDossier, char* nom, int numIDossierDest, char* nomDest); //cp
// *******************


// ********** DISQUE **********
int charger_disque();
void sauvegarder_disque();		// save
int formater_disque();
void creation_disque();
int creer_rep_racine();

// ********** FICHIER **********

int copie_fichier_rep(int numISrc, int numIDest);
int copie_fichier_txt(int numISrc, int numIDest);
int recherche_inode_nom(int numI, char* nom);

// ********** FICHIER : LIEN **********

int deplacement_lien(int numI);

// ********** FICHIER : TEXTE **********
int _ecriture_texte(int numI, char* texte);

// ********** FICHIER : DOSSIER **********
int ajout_dossier_fichier(int numIDossier,int numIFichier,char* nom);
int recherche_inode_nom(int numI, char* nom);
int _ajout_dossier_infoFic(int numI, InfoFic info);
InfoFic creer_infoFic(int numI, char *nom);
InfoFic creer_infoFic_vide();
int ecraser_infoFic(int numI, int numB, int numF, InfoFic info);
int decrem_lien(int numI);
int increm_lien(int numI);
InfoFic lire_infoFic(int numI,int numB, int numF);
void defragmenter_dossier(int numI);

// ********** BLOC **********
int test_dossier_bloc_vide(int numI, int numB);
int bloc_espace_utilise(int pos);
int bloc_espace_restant(int pos);
int ajout_bloc(int numI);
void init_bloc(int pos);
int ecraser_bloc(int pos, Bloc bloc);
Bloc lire_bloc(int pos);
InfoBloc lire_infoBloc(int num);
int ecraser_infoBloc(int num, InfoBloc info);
int change_infoBloc(int num);
int recup_infoBloc(int pos);
void calc_succ_bloc_libre();
int suppr_bloc(int numI,int numB);

// ********** INODE **********
int changer_droits(int numI, char droits);
int vider_inode(int numI);
int ajouter_inode(Inode inode);
int ecraser_inode(int numI, Inode inode);
void calc_succ_inode_libre();
Inode creer_inode(int type, char droits);
Inode creer_inode_vide();
Inode lire_inode(int numI);

// ********** TESTS **********
int test_num_bloc(int numB);
int test_num_fic(int numF);
int test_inode(int numI);
int test_inode_fichier(int numI);
int test_inode_texte(int numI);
int test_inode_dossier(int numI);
int test_inode_lien(int numI);
int test_nom_fichier(char* nom, char sortie[FIC_NOM_TAILLE]);
int test_droits_inode(int numI,char d);
int _test_lecture(int droits);
int _test_ecriture(int droits);
int _test_execution(int droits);
int test_droits_valide(char p) ;
int test_ls_r(char mode);
int test_ls_l(char mode);
int confirmation();
int test_pere(int numI, int numPere);


// ********** AFFICHAGE **********

void affiche_inode_tout(Inode inode);
void affiche_contenu_disque();
void affiche_bloc(Bloc bloc);
void affiche_inode(Inode inode);
void affiche_droits(char droits);
void affiche_typeFichier(int type);
/*######################################################################################*/

/*################################ INTERPRETEUR ########################################*/
#define TAILLE_BUFF 512

int MainInterpreteur(void);
char * saisie_cmd();
char** cmd_avec_args(char* commande, int* nbarg);
int interpreteur(char** argv, int nbarg);

/*######################################################################################*/

/*################################# COMMANDES #########################################*/
int ls_cmd(char** argv, int nbarg);
int cat_cmd(char** argv, int nbarg);
int touch_cmd(char** argv, int nbarg);
int echo_cmd(char** argv, int nbarg);
int rm_cmd(char** argv, int nbarg);
int mkdir_cmd(char** argv, int nbarg);
int ln_mkdir(char** argv, int nbarg);
int mv_cmd(char** argv, int nbarg);
int cp_cmd(char** argv, int nbarg);
int cd_cmd(char** argv, int nbarg);
int sauv_cmd();
int recharger_cmd();
int format_cmd();
int aide_cmd(char** argv, int nbarg);
char* recup_nom_fic_chemin(char* chemin);
int deplacement_arborescence(int numIDossier, char* chemin);
int get_mode(char* string);
int grep_cmd(char** argv, int nbarg);
int df_cmd();
/*######################################################################################*/



#include <stdbool.h>

#ifndef libraries_h
#define libraries_h

#define DEBUGLINE printf ("This is line %d of function '%s'\n",__LINE__, __func__)
#define MAX_SIZE 200


/// @brief Enumeration des cas possibles retournés par les fonctions tournormal() et tourspecial(), permet de recevoir l'information, et d'agir en conséquence
typedef enum
{
    sauve_partie,
    abandon_partie,
    continue_partie,
    continue_afficher_partie,
} tour_partie_e;

typedef enum
{
    menu_lancer,
    menu_reprendre,
    menu_regle,
    menu_quitter,
} menu_e;

typedef enum
{
    mode_none,
    mode_normal,
    mode_aleatoire,
    mode_special,
} mode_e;

typedef struct partie_s     // - Declare la structure contenant tout ce que l'on a besoin
{                           //   au cours de l'execution du programme
    char date[11];          // - Date de la sauvegarde, ex: "11/05/2023"
    char nomSauvegarde[21]; // - Nom  de la sauvegarde, ex: "Hex_Save"
    char mode[10];          // - Mode de jeu,           ex: "normal" ou "special" ou "aleatoire"
    char joueur;            // - Pion du joueur,        ex: "X" ou "O"
    int  taille;            // - Taille du plateau        : 9 a 14
    char plateau[14][14];   // - Tableau 2D designant le plateau de jeu, il contient 'X', 'O', ' ', 3, 2, 1
                            //   selon le mode de jeu, il est aussi toujours de taille 14x14, mais peut etre utilise seulement en 9x9
    int canplay_O;         // En mode special, le joueur 'O' peut-il joueur ?
    int canplay_X;         // de meme
} partie_t;                 

/*
int gagnant(int taille, char plateau[14][14]);
void init_plateauNormal(int taille, char plateau[14][14]);
int init_plateauAlea(int taille, char plateau[14][14]);
int tournormal(int taille, char plateau[14][14], char joueur);
int save(char joueur, const char *nomSauvegarde, const char *mode, int taille, char plateau[14][14]);
void afficherplateau (int taille, char plateau[14][14]);
partie_t reprise(char *nomSauvegarde);
*/


menu_e          menu                    (void);
mode_e          mode                    (char *mode);
int             taillePlateau           (void);
void            changeConsoleMode       (void);
void            regle                   (void);
void            afficherplateau         (partie_t save);
void            init_plateauNormal      (partie_t *save);
void            init_plateauAlea        (partie_t *save);
bool            gagnant                 (partie_t save);
bool            sauvegarde              (const partie_t *partie, const char *path);
bool            reprise                 (      partie_t *partie, const char *path, const char *nomsauvegarde);
bool            commonfiles             (                        const char *path);
bool            lettrevalide            (char lettre, int taille);
bool            nombrevalide            (int nombre, int taille);
tour_partie_e   tournormal              (partie_t *save);
tour_partie_e   tourspecial             (partie_t *save);

#endif

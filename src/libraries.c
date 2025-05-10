#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h> // for GetStdHandle, GetConsoleMode, SetConsoleMode
#endif

#include "include/couleurs.h"
#include "include/libraries.h"

/// @brief Affiche le menu et attend une de ses options
/// @return Énumération du menu
menu_e menu(void)
{
    do
    {
        char menu[2] = "";
        int c;

        printf("\n*Menu*\n");
        printf("\t1 : Lancer une nouvelle partie\n");
        printf("\t2 : Reprendre une partie\n");
        printf("\t3 : Afficher les regles du jeu\n");
        printf("\t0 : Quitter\n");
        printf("Votre choix : ");
        if (fgets(menu, sizeof(menu), stdin) != NULL)
        {
            if (strchr(menu, '\n') == NULL)
            {
                while ((c = getchar()) != '\n' && c != EOF)
                {
                }
            }
        }
        else
        {
            clearerr(stdin);
            continue;
        }

        switch (menu[0])
        {
        case '0':
            return menu_quitter;
        case '1':
            return menu_lancer;
        case '2':
            return menu_reprendre;
        case '3':
            return menu_regle;
        }
    } while (true);
}

/// @brief Demande le mode de jeu de la partie
/// @param mode pointeur sur le mode de jeu de la partie
/// @return Énumération du mode
mode_e mode(char *mode)
{
    char c;
    do
    {
        printf("\nMode de jeu pour votre partie ? 'normal'/'special'/'aleatoire' : ");
        char buffer[30] = "";

        fgets(buffer, sizeof(buffer), stdin);

        if (strchr(buffer, '\n') == NULL)
        {
            // Si le buffer n'a pas de '\n', on consomme les caractères restants
            while ((c = getchar()) != '\n' && c != EOF)
            {
                // Consommer les caractères restants
            }
        }
        buffer[strcspn(buffer, "\n")] = 0;

        if (0 == strcmp(buffer, "normal"))
        {
            strcpy(mode, "normal");
            return mode_normal;
        }
        else if (0 == strcmp(buffer, "aleatoire"))
        {
            strcpy(mode, "aleatoire");
            return mode_aleatoire;
        }
        else if (0 == strcmp(buffer, "special"))
        {
            strcpy(mode, "special");
            return mode_special;
        }
    } while (true);
}

/// @brief Demande la taille du plateau et ne s'arrête pas pas tant que la taille n'est pas la bonne
/// @returns la taille du plateau
int taillePlateau(void)
{
    int taille;
    do
    {
        char buffernombre[10] = "";
        int c;

        printf("Veuillez donner la dimension du plateau (9 a 14) : ");
        if (fgets(buffernombre, sizeof(buffernombre), stdin) != NULL)
        {
            if (strchr(buffernombre, '\n') == NULL)
            {
                while ((c = getchar()) != '\n' && c != EOF)
                {
                }
            }
        }
        else
        {
            clearerr(stdin);
            taille = 0;
        }

        taille = atoi(buffernombre);
    } while (taille < 9 || taille > 14);
    return taille;
}

/// @brief Initialise un plateau de jeu vide
/// @param save : structure (informations) de la partie
void init_plateauNormal(partie_t *save)
{
    /*for (int i = 0; i < save->taille; i++)
    {
        for (int j = 0; j < save->taille; j++)
        {

            save->plateau[i][j] = ' ';
        }

    }*/
    memset(save->plateau, ' ', sizeof(save->plateau));
    return;
}

/// @brief Initialise un plateau de jeu vide mais avec (tailleduplateau-1)/2 pions sur le plateau pour chaque joueur
/// @param save : structure (informations) de la partie
void init_plateauAlea(partie_t *save)
{
    int row, col;
    int pieces = (int)((save->taille - 1) / 2);
    srand(time(0));

    init_plateauNormal(save);

    for (int i = 0; i < pieces; i++)
    {
        do
        {
            row = rand() % (save->taille);
            col = rand() % (save->taille);
        } while (save->plateau[row][col] != ' ');
        save->plateau[row][col] = 'O';
    }

    for (int j = 0; j < pieces; j++)
    {
        do
        {
            row = rand() % (save->taille);
            col = rand() % (save->taille);
        } while (save->plateau[row][col] != ' ');
        save->plateau[row][col] = 'X';
    }

    return;
}

/// @brief Affiche le plateau de jeu
/// @param save : structure (informations) de la partie
void afficherplateau(partie_t save)
{
    printf("   ");
    for (int j = 0; j < save.taille; j++)
    {
        printf(enROUGE " %c " enFin, 'A' + j);
    }
    printf("\n");
    for (int i = 0; i < save.taille; i++)
    {
        for (int j = 0; j < i; j++)
        {
            printf(" ");
        }
        printf(enBLEU "%2d " enFin, i + 1);
        for (int j = 0; j < save.taille; j++)
        {
            char caractere = save.plateau[i][j];

            if (save.plateau[i][j] < ' ')
            {
                caractere += '0';
            }
            if (caractere == 'O')
            {
                printf(enBLANC "[" enROUGE "%c" enBLANC "]" enFin, caractere);
            }
            else if (caractere == 'X')
            {
                printf(enBLANC "[" enBLEU "%c" enBLANC "]" enFin, caractere);
            }
            else
            {
                printf(enBLANC "[" enVERT "%c" enBLANC "]" enFin, caractere);
            }
        }
        printf(enBLEU " %2d\n" enFin, i + 1);
    }
    printf("  ");
    for (int i = 0; i < save.taille; i++)
    {
        printf(" ");
    }
    for (int j = 0; j < save.taille; j++)
    {
        printf(enROUGE " %c " enFin, 'A' + j);
    }
    printf("\n");
}

/// @brief Fonction permettant de placer un pion sur le plateau, ou de sauvegarder la partie, ou de l'abandonner
/// @param *save pointeur sur la structure (informations) de la partie
/// @returns continue_partie /
/// save_partie /
/// abandon_partie
tour_partie_e tournormal(partie_t *save)
{
    // boucle jusqu'a ce que les entrees soient valides
    for (;;)
    {
        char input[20] = ""; // chaine de caracteres pour stocker l'entree du joueur
        char color[6];
        strcpy(color, save->joueur == 'O' ? enROUGE : enBLEU);
        printf("%s%c" enFin " doit jouer", color, save->joueur);
        printf(", entrez les coordonnees, ou sauvegarder ou quitter (ex : a3 ou 'S' ou 'X') : ");
        fflush(stdout); // force une sortie standard, il n'y a pas de '\n' au dernier printf

        fflush(stdin); // Supprime les caracteres parasites
        fgets(input, sizeof(input), stdin);

        char lettre1;
        int nombre1;

        int n = sscanf(input, " %c %d", &lettre1, &nombre1);
        lettre1 = tolower(lettre1);

        switch (n)
        {
            {
                {
                case 1:
                {
                    {
                        switch (lettre1) // on teste alors le caractere,
                        {
                        case 'X':
                        case 'x':
                            return abandon_partie; // le tour s'arretera apres le break et la valeur renvoyee et testee dans la main()

                        case 'S':
                        case 's':
                            printf("Sauvegarde et arret de la partie\n");
                            return sauve_partie; // de meme

                        default:
                            printf("Une seule lettre a ete donnee, 'X'/'S' attendues. Reessayez\n");
                            continue;
                        }
                        break;
                    }
                }

                case 2:
                {
                    {
                        if (!lettrevalide(lettre1, save->taille))
                        {
                            continue;
                        }

                        if (!nombrevalide(nombre1, save->taille))
                        {
                            continue;
                        }

                        // calcule les indices de ligne et de colonne correspondants aux coordonnées entrées
                        int row1 = nombre1 - 1;
                        int col1 = lettre1 - 'a';

                        if (save->plateau[row1][col1] != ' ')
                        {
                            printf("\nVous ne pouvez pas placer de pion la ou il y en a deja un\n");
                            continue;
                        }
                        else
                        {
                            printf("\nDernier tour effectue : %c%d\n", lettre1, nombre1);
                            // place le pion sur le plateau
                            save->plateau[row1][col1] = save->joueur; // joueur designe soit 'O' soit 'X', cela change à chaque fois que tour() est appelé
                            return continue_afficher_partie;
                        }
                        break;
                    }
                }

                default:
                {
                    {
                        printf("Entree invalide. Reesayez\n");
                        continue;
                    }
                }
                }
            }
        }

        /*
        Ancien Systeme de verification (fonctionne mais inefficace et plus compliqué)

        size_t len = strlen(input);
        if (len && input[len-1] == '\n')
        { // enleve le \n s'il est en derniere position par rapport a la longueur de la chaine

            input[len-1] = '\0';
            len--;
        }

        switch (len)
        {
            default:
                printf("Entree invalide. Reessayez\n");
                continue;
            case 1:
                switch (input[0]) // on teste alors le caractere,
                {
                    case 'X':
                    case 'x':
                        return abandon_partie; // le tour s'arretera apres le break et la valeur renvoyee et testee dans la main()

                    case 'S':
                    case 's':
                        printf("Sauvegarde et arret de la partie\n");
                        return sauve_partie; // de meme

                    default:
                        printf("Une seule lettre a ete donnee, 'X'/'S' attendues. Reessayez\n");
                        continue;
                }
                break;

            case 2:
            case 3:
            {

                char lettre = tolower(input[0]);    // convertit la première lettre en minuscule pour faciliter la vérification
                int  nombre = atoi(&input[1]);      // obtient le nombre suivant la lettre

                if (!lettrevalide(lettre, save->taille))
                {
                    continue;
                }

                if (!nombrevalide(nombre, save->taille))
                {
                    continue;
                }

                // calcule les indices de ligne et de colonne correspondants aux coordonnées entrées
                int row = nombre - 1;
                int col = lettre - 'a';

                if (save->plateau[row][col] != ' ')
                {
                    printf("\nVous ne pouvez pas placer de pion la ou il y en a deja un\n");
                    continue;
                }
                else
                {
                    printf("\nDernier tour effectue : %c%d\n", lettre, nombre);
                    // place le pion sur le plateau
                    save->plateau[row][col] = save->joueur; // joueur designe soit 'O' soit 'X', cela change à chaque fois que tour() est appelé
                    return continue_afficher_partie;
                }
                break;
            }
        }
        */
    }
}

/// @brief Sauvegarde la partie en cours
/// @param partie pour sauvegarder la partie
/// @param path le chemin d'acces au dossier
/// @return vrai ou faux selon si la partie a été sauvegardée
bool sauvegarde(const partie_t *partie, const char *path)
{
    char leFichier[MAX_SIZE];

    snprintf(leFichier, sizeof(leFichier), "%s/%s", path, partie->nomSauvegarde);
    FILE *file = fopen(leFichier, "w+");

    if (!file)
    {
        fprintf(stderr, "Erreur ouverture de '%s'\n", leFichier);
        return false;
    }
    else
    {
        // https://koor.fr/C/ctime/strftime.wp

        time_t timestamp = time(0);
        struct tm *pTime = localtime(&timestamp);
        char date[20];

        strftime(date, sizeof(date), "%d/%m/%Y", pTime);
        fprintf(file, "%s\n", date);
        fprintf(file, "%s\n", partie->mode);
        fprintf(file, "%c\n", partie->joueur);
        fprintf(file, "%d\n", partie->taille);
        for (int i = 0; i < partie->taille; i++)
        {
            for (int j = 0; j < partie->taille; j++)
            {
                fprintf(file, "%d ", partie->plateau[i][j]);
            }
            fprintf(file, "\n");
        }
        if (0 == strcmp(partie->mode, "special"))
        {
            fprintf(file, "%d", partie->canplay_O);
            fprintf(file, "%d", partie->canplay_X);
        }
        fclose(file);
        printf("Sauvegarde '%s' effectue dans '%s'\n", partie->nomSauvegarde, leFichier);
        return true;
    }
}

/// @brief Reprend une partie sauvegardée dans le dossier 'path'
/// @param partie la structure de la partie qui reprendra les informations de la partie sauvegardée
/// @param path le chemin au dossier de sauvegardes
/// @param nomsauvegarde le nom de la sauvegarde donnée
/// @return true si la reprise est bonne, false sinon
bool reprise(partie_t *partie, const char *path, const char *nomsauvegarde)
{
    char leFichier[MAX_SIZE];

    snprintf(leFichier, sizeof(leFichier), "%s/%s", path, nomsauvegarde);

    FILE *file = fopen(leFichier, "r");

    if (!file)
    {
        fprintf(stderr, "Erreur ouverture de <%s>\n", leFichier);
        return false;
    }
    else
    {
        snprintf(partie->nomSauvegarde, sizeof(partie->nomSauvegarde), "%s", nomsauvegarde);

        char joueur[2];                   // déclare une variable joueur de 2 caractères dont on récupérera le premier caractère
        fscanf(file, "%s", partie->date); // de la partie à reprendre
        fscanf(file, "%s", partie->mode);
        fscanf(file, "%s", joueur);
        partie->joueur = joueur[0]; // récupère donc le premier caractère du tableau joueur[2] =  {'O/X', '\0'}
        fscanf(file, "%d", &partie->taille);
        for (int i = 0; i < partie->taille; i++)
        { // pour chaque ligne et colonne, on recupere le nombre associé, dont on place
            for (int j = 0; j < partie->taille; j++)
            { // la valeur dans le plateau
                int caseplateau = ' ';
                fscanf(file, "%d", &caseplateau);    // scanne le nombre associé au caractère ' ', ou 'X' ou 'O' respectivement 32, 79, 88
                partie->plateau[i][j] = caseplateau; // place tous les caractères (ici sous forme de nombre ensuite convertis en caractères)
            }
        }
        bool modeSpecial = 0 == strcmp(partie->mode, "special");
        if (modeSpecial)
        {
            fscanf(file, "%d", &partie->canplay_O);
            fscanf(file, "%d", &partie->canplay_X);
        }

        printf("\n");
        printf("Date                        : %s\n", partie->date);
        printf("Mode de jeu                 : %s\n", partie->mode);
        printf("Pion du joueur devant jouer : %c\n", partie->joueur);
        printf("Taille du plateau           : %d\n", partie->taille);
        if (modeSpecial)
        {
            printf("Le joueur 'O' peut jouer : %s\n", partie->canplay_O == 1 ? "Oui" : "Non");
            printf("Le joueur 'X' peut jouer : %s\n", partie->canplay_X == 1 ? "Oui" : "Non");
        }
        fclose(file);
        return true; // retourne true si correcte
    }
}

/// @brief Analyse le plateau et indique s'il y a un gagnant
/// @param save (structure de la partie en cours)
/// @return true s'il y a un gagnant
/// @returns faux sinon}
bool gagnant(partie_t save)
{

    save.joueur = save.joueur == 'X' ? 'O' : 'X';
    bool chemin_trouve;
    bool premier_pion;
    bool T[save.taille + 1][save.taille + 1]; // tableau parallèle au plateau, taille +1, même coordonnées et contient dans ses cases 1 ou 0

    for (int i = 0; i < save.taille + 1; i++)
    { // toutes les cases du tableau contiennent des 0.
        for (int j = 0; j < save.taille + 1; j++)
        {
            T[i][j] = 0;
        }
    }
    if (save.joueur == 'X') // Condition de victoire de X : il gagne de gauche à droite
    {
        do
        {
            chemin_trouve = false;
            premier_pion = false;
            for (int i = 0; i < save.taille; i++) // Vérifie l'existance d'un pion X sur la première colonne
            {
                if (save.plateau[i][0] == 'X')
                {
                    T[i][0] = 1;
                    premier_pion = true;
                }
            }
            if (premier_pion == false) // La fonction s'arrête si aucun pion n'est détecté sur la première colonne
            {
                return 0;
            }
            for (int j = 1; j < save.taille; j++)
            {
                for (int i = 0; i < save.taille; i++)
                {
                    if (save.plateau[i][j] == 'X') // donne la valeur 1 à la case T[i][j]si elle est adjacente à une case du chemin(qui a comme valeur 1) et si P[i][j] contient le pion X
                    {
                        if (T[i][j - 1] == 1 || T[i][j + 1] == 1 || T[i - 1][j] == 1 || T[i + 1][j] == 1 || T[i + 1][j - 1] == 1 || T[i - 1][j + 1] == 1)
                        {
                            if (T[i][j] == 0)
                            {
                                chemin_trouve = true;
                            }
                            T[i][j] = 1;
                        }
                    }
                }
            }
            for (int i = 0; i < save.taille; i++)
            { // Si un pion X appartient au chemin et est sur la dernière colonne, la fonction retourne 1.
                if (T[i][save.taille - 1] == 1)
                {
                    return 1;
                }
            }
        } while (chemin_trouve); // continue tant qu'un nouveau jeton s'ajoute au chemin
        return 0;
    }

    if (save.joueur == 'O') // Condition de victoire de O : il gagne de de haut en bas
    {
        do
        {
            chemin_trouve = false;
            for (int j = 0; j < save.taille; j++) // Vérifie l'existence d'un pion O sur la première ligne
            {
                if (save.plateau[0][j] == 'O')
                {
                    T[0][j] = 1;
                    premier_pion = true;
                }
            }
            if (premier_pion == false) // La fonction s'arrête si aucun pion n'est détecté sur la première ligne
            {
                return 0;
            }
            for (int i = 1; i < save.taille; i++)
            {
                for (int j = 0; j < save.taille; j++)
                {
                    if (save.plateau[i][j] == 'O') // donne la valeur 1 à la case T[i][j] si elle est adjacente à une case du chemin(qui a comme valeur 1) et si P[i][j] contient le pion
                    {
                        if (T[i][j - 1] == 1 || T[i][j + 1] == 1 || T[i - 1][j] == 1 || T[i + 1][j] == 1 || T[i - 1][j + 1] == 1 || T[i + 1][j - 1] == 1)
                        {
                            if (T[i][j] == 0)
                            {
                                chemin_trouve = true;
                            }
                            T[i][j] = 1;
                        }
                    }
                }
            }
            for (int j = 0; j < save.taille; j++)
            { // Si un pion O appartient au chemin et est sur la dernière ligne, la fonction retourne 1.
                if (T[save.taille - 1][j] == 1)
                {
                    return 1;
                }
            }
        } while (chemin_trouve); // continue² tant qu'un nouveau jeton s'ajoute au chemin

        return 0;
    }
    return 0;
}

/// @brief affiche les règles du jeu
void regle(void)
{
    printf(enRouge "\n*Regles du jeu* :\n" enFin);

    printf("          Au debut de la partie, un plateau vide separe deux joueurs.\n"
           "          Ce plateau en forme de losange est compose d'hexagones reguliers.\n"
           "          Les deux joueurs sont representes par une forme :" enRouge " X" enFin " et " enBleu "O" enFin ".\n"
           "          Les joueurs disposent tour a tour leurs pions sur une case de leur choix et un par un.\n"
           "          Le but est de relier les deux cotes opposes du losange.\n"
           "          La partie est gagnee lorsqu'un joueur  a reussi a creer un chemin\n"
           "          reliant un bord a l'autre.\n");

    printf("\nPartie aleatoire :\n");
    printf("          Des pions sont aleatoirement places sur le plateau au demarrage de la partie\n");

    printf("\nMode special : \n");
    printf("          Des coups speciaux sont ajoutes.\n"
           "          Coups speciaux :\n"
           "          - Retirer un pion adverse (cela consomme le tour du joueur qui realise cette action, la case d'ou\n"
           "          le pion a ete retire devient inutilisable pour 3 tours : le tour de l'adversaire, le tour du joueur,\n"
           "          et a nouveau le tour de l'adversaire)\n"
           "          - Doubler un coup (le joueur place deux pions d'un coup, mais pas adjacents, et il devra passer\n"
           "          le tour suivant)\n");
    return;
}

/// @brief Indique si le caractère donné est valide
/// @param lettre le caractère à tester
/// @param taille la taille du plateau de la partie
/// @return true si vrai, false sinon
bool lettrevalide(char lettre, int taille)
{
    if (lettre < 'a' || lettre > 'a' + taille - 1)
    {
        printf("\nCoordonnee '%c' invalide. Reessayez.\n", lettre);
        return false;
    }
    else
    {
        return true;
    }
}

/// @brief Indique si le nombre donné est valide
/// @param nombre le nombre à tester
/// @param taille la taille du plateau de la partie
/// @return true si vrai, false sinon
bool nombrevalide(int nombre, int taille)
{
    if (nombre < 1 || nombre > taille)
    {
        printf("\nCoordonnees '%d' invalides. Reessayez.\n", nombre);
        return false;
    }
    else
    {
        return true;
    }
}

/// @brief Donne les fichiers courants dans /saves
/// @param path
/// @return true si le dossier est trouvé et affiche le contenu du dossier,
/// false sinon
bool commonfiles(const char *path)
{
    DIR *d;

    if (NULL != (d = opendir(path)))
    {
        struct dirent *dir;

        if (NULL == (dir = readdir(d)))
        {
            printf("Le dossier %s ne contient aucun fichier.\n", path);
            return false;
        }
        while (NULL != (dir = readdir(d)))
        {
            struct stat st;
            char filepath[MAX_SIZE + sizeof(dir->d_name)];

            snprintf(filepath, sizeof(filepath), "%s/%s", path, dir->d_name);

            if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode))
            {
                printf("- %s \n", dir->d_name);
            }
        }
        closedir(d);
        return true;
    }
    else
    {
        printf("Le dossier '%s' n'existe pas\n", path);
        return 0;
    }
}

void changeConsoleMode(void)
{
#ifdef _WIN32
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
    HANDLE hStdout;
    if (INVALID_HANDLE_VALUE != (hStdout = GetStdHandle(STD_OUTPUT_HANDLE)))
    {
        DWORD wMode;

        if (GetConsoleMode(hStdout, &wMode))
        {
            if (!SetConsoleMode(hStdout, wMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
            {
                fprintf(stderr, "Error SetConsoleMode\n");
            }
            else
            {
                printf("Changement de couleur effectue\n");
            }
        }
    }
#endif
    return;
}

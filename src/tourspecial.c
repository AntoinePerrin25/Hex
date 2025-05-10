#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

#include "include/couleurs.h"
#include "include/libraries.h"

/// @brief Fonction permettant de placer un pion sur le plateau, ou de sauvegarder la partie, ou de l'abandonner.
/// Des coups spéciaux sont disponibles ; placer 2 pions d'un coup (mais passer son tour), rendre une case de l'adversaire inutilisable
/// @param *save pointeur sur la structure (informations) de la partie
/// @returns continue_partie /
/// save_partie /
/// abandon_partie
tour_partie_e tourspecial(partie_t *save)
{
    
    int valid = 0;
    
    if (save->joueur == 'O')
    {
        if (!save->canplay_O)
        {
            printf("Le joueur 'O' passe son tour\n");
            save->canplay_O = true;
            return continue_partie;
        }
    }
    else if (save->joueur == 'X')
    {
        if (!save->canplay_X)
        {
            printf("Le joueur 'X' passe son tour\n");
            save->canplay_X = true;
            return continue_partie;
        }
    }

    // boucle jusqu'à ce que les coordonnées entrées soient valides
    do
    {
        char input[20] = ""; // chaine de caracteres pour stocker l'entree du joueur
        
        printf("\n%c doit jouer", save->joueur);
        printf(", entrez les coordonnees, ou sauvegarder ou quitter (ex : 'a3'/'a3 a5'/'S'/'X') : ");
        fflush(stdout);
        fflush(stdin);
        fgets(input, sizeof(input), stdin);

        char lettre1, lettre2;
        int nombre1, nombre2;

        int n = sscanf(input, " %c %d %c %d", &lettre1, &nombre1, &lettre2, &nombre2);
        lettre1 = tolower(lettre1);
        lettre2 = tolower(lettre2);


        switch(n)
        {{{
            case 1:
            {{
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
            }}

            case 2:
            {{
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
                
                switch(save->plateau[row1][col1])
                {{
                    case ' ':
                            valid = 1;
                            printf("\nDernier tour effectue : %c%d\n", lettre1, nombre1);
                            // place le pion sur le plateau
                            save->plateau[row1][col1] = save->joueur;
                            break;
                    case '1':
                    case '2':
                    case '3':
                    {
                        printf("Case inutilisable pour %d tour%s\n", save->plateau[row1][col1]-'0', save->plateau[row1][col1] > '1' ? "s" : "" );
                        continue;
                    }
                    default:
                        if(save->plateau[row1][col1] == save->joueur)
                        {
                            printf("Premiere case interdite, elle vous appartient deja\n");
                            continue;
                        }
                        else
                        {
                            printf("Coup special !\n");
                            valid = 1;

                            printf("\nDernier tour effectue : %c%d\n", lettre1, nombre1);
                            // place le pion sur le plateau
                            save->plateau[row1][col1] = '4'; // la case devient inutilisable
                        }
                        break;

                }}
                break;
            }}
            
            case 4:
            {{
                if(lettre1 == lettre2 && nombre1 == nombre2)
                {
                    printf("Vous ne pouvez pas donner 2 fois la meme case\n");
                    continue;
                }

                if (!lettrevalide(lettre1, save->taille) ||
                    !lettrevalide(lettre2, save->taille))
                {

                    continue;
                }

                if (!nombrevalide(nombre1, save->taille) ||
                    !nombrevalide(nombre2, save->taille))
                {
                    continue;
                }

                // calcule les indices de ligne et de colonne correspondants aux coordonnées entrées
                int row1 = nombre1 - 1;
                int col1 = lettre1 - 'a';
                int row2 = nombre2 - 1;
                int col2 = lettre2 - 'a';


                if ((row1 == row2     && col1 == col2 + 1) || // voisin de droite
                    (row1 == row2     && col1 == col2 - 1) || // voisin de gauche
                    (row1 == row2 + 1 && col1 == col2    ) || // voisin bas
                    (row1 == row2 - 1 && col1 == col2    ) || // voisin haut
                    (row1 == row2 + 1 && col1 == col2 - 1) || // voisin bas droite
                    (row1 == row2 - 1 && col1 == col2 + 1))   // voisin haut gauche
                {
                    printf("Vous ne pouvez pas placer 2 pions adjacents\n");
                    continue;
                }


                int validnormal = 0;    // variables pour savoir si le premier pion est
                int validspecial = 0;   // un coup normal (on place un pion), ou special
                                        // dans ce cas la case sera inutilisable
                
                switch(save->plateau[row1][col1]) // on teste les coordonnees de la premiere case donnee
                {
                    case ' ':
                        valid = 1;                        
                        validnormal = 1;
                        break;

                    case '1':
                    case '2':
                    case '3':
                        printf("Premiere case inutilisable pour %d tour%s\n", save->plateau[row1][col1] - '0', save->plateau[row1][col1] > '1' ? "s" : "" );
                        continue;

                    default:
                        if(save->plateau[row1][col1] == save->joueur)
                        {
                            printf("Premiere case interdite, elle vous appartient deja\n");
                            continue;
                        }
                        else
                        {
                            valid = 1;
                            validspecial = 1;
                        }
                        break;
                }
                
                if (valid)
                {
                    valid = 0;
                    switch(save->plateau[row2][col2])
                    {
                        case ' ':
                            valid = 1;
                            if(validnormal)                                 // coup normal pour les 2 pions
                            {
                                save->plateau[row1][col1] = save->joueur;   // on place les 2 pions
                                save->plateau[row2][col2] = save->joueur;
                                printf("Dernier tour effectue : %c%d %c%d\n", lettre1, nombre1, lettre2, nombre2); // on affiche les derniers coups
                            }
                            else if (validspecial)                          // coup special pour le premier pion, normal pour le second
                            {
                                printf("\nCoup special !");
                                printf("\nDernier tour effectue : %c%d\n", lettre1, nombre1);
                                save->plateau[row1][col1] = '4';              // la case devient inutilisable pour 3 tours (a la fin de la fonction on enleve 1 a chaque case inutilisable)
                                save->plateau[row2][col2] = save->joueur;   // on place le pion
                            }
                            else 
                            {
                                fprintf(stderr, "bug\n");

                            }
                            break;
                        case '1':
                        case '2':
                        case '3':
                            printf("Seconde case inutilisable pour %d tour%s\n", save->plateau[row2][col2] - '0', save->plateau[row2][col2] > '1' ? "s" : "" );
                            continue;
                        default:
                            if(save->plateau[row2][col2] == save->joueur)
                            {
                                printf("Seconde case interdite, elle vous appartient deja\n");
                                continue;
                            }
                            else//
                            {
                                if (validnormal)
                                {
                                    save->plateau[row1][col1] = save->joueur;   // de meme qu'au dessus, mais inverse 
                                    save->plateau[row2][col2] = '4';
                                    printf("\nCoup special !\n");
                                    printf("Dernier tour effectue : %c%d %c%d\n", lettre1, nombre1, lettre2, nombre2);
                                    valid = 1;
                                }
                                else if (validspecial)
                                {
                                    printf("\nDouble coup special !\n");
                                    printf("Dernier tour effectue : %c%d %c%d\n", lettre1, nombre1, lettre2, nombre2);
                                    save->plateau[row1][col1] = '4';          // Double coup special, les 2 cases deviennent inutilisables
                                    save->plateau[row2][col2] = '4';          // pour 3 tours
                                    valid = 1;
                                }
                                else
                                {
                                    fprintf(stderr, "bug\n");
                                }
                            }
                            break;
                    }
                }
                if(save->joueur == 'O')
                {
                    save->canplay_O = false;
                }
                else // (save->joueur == 'X')
                {
                    save->canplay_X = false;
                }
                break;
            }}
            default:            
                printf("Entree invalide. Reessayez\n");
                continue;
        }}}
    
    } while(!valid); // redemande les coordonnees jusqu'a ce qu'elles soient bonnes
    // ajuste les caracteres du plateau si necéssaire (0-4)
    for (int i = 0; i < save->taille; i++)
    {
        for (int j = 0; j < save->taille; j++)
        {
            switch(save->plateau[i][j]) // pour chaque case du plateau de jeu, si la case est inutilisable
            {                           // on enleve 1, sinon on ne fait rien
                case 'O':               
                case 'X':
                case ' ':               // on ne fait rien
                    break;
                default:                // la case peut contenir : '1', '2', '3' ou '4', donc '4'->'3'->'2'->'1'->' '
                    save->plateau[i][j]--;
                    if (save->plateau[i][j] == '0') // 1->' '
                    {
                        save->plateau[i][j] = ' ';
                    }
                    break;
            }
        }
    }   
    return continue_afficher_partie;             // tout s'est bien passe, on peut continuer la partie et afficher le plateau
}

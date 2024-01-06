#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "couleurs.h"
#include "libraries.h"
//#include "tourspecial.c"
//#include "libraries.c"
//windres ressources.rc -o icone.o

int main()
{
    changeConsoleMode();
    
    const char savepath[] = "saves";        // revient a "./saves"
    char continuerajouer = 'n';
    
    mkdir(savepath);                        // si le dossier saves n'existe pas, il est crée
    do
    {
        
        int jouer = 0;
        partie_t actuelle = {"",};          // declare la structure des données de la partie actuelle
        bool initialiserplateau;      // Lors d'une reprise de partie, on ne ré-initialise pas le plateau, mais on le fait au début d'une nouvelle partie
        mode_e eMode = mode_none;
        
        do
        {   

            initialiserplateau = true;
            
            switch (menu())
            {
                case menu_quitter:
                    printf("Au revoir\n");
                    return 0;

                case menu_lancer:
                    eMode = mode(actuelle.mode);

                    actuelle.taille = taillePlateau();
                    actuelle.joueur = 'O';
                    jouer = 1;
                    break;

                case menu_reprendre:
                {
                    bool bonnereprise = false;
                    do
                    {
                        if (commonfiles(savepath))
                        {
                            char nomsauvegarde[MAX_SIZE];
                            printf("Quelle partie voulez-vous reprendre : ");
                            scanf(" %s", nomsauvegarde);
                            
                            bonnereprise = reprise(&actuelle, savepath, nomsauvegarde);
                            if(0 == strcmp(actuelle.mode, "normal"))
                            {
                                eMode = mode_normal;
                            }
                            else if(0 == strcmp(actuelle.mode, "aleatoire"))
                            {
                                eMode = mode_aleatoire;
                            }
                            else if (0 == strcmp(actuelle.mode, "special"))
                            {
                                eMode = mode_special;
                            }
                            printf("Pour continuer et jouer, appuyez sur 'ENTER'\n");
                            fflush(stdin);
                            getchar();
                            initialiserplateau = false;
                            jouer = 1;
                        }
                        else
                        {
                            break; // probleme de fichier/dossier, re-menu
                        }
                    } while (!bonnereprise);
                    break;
                }
                case menu_regle:
                    regle();
                    break;
            }
        } while(!jouer);


        if (eMode != mode_none)
        {
            if (initialiserplateau)
            {
                if (eMode == mode_normal)
                {
                    init_plateauNormal(&actuelle);
                }
                else if (eMode == mode_special)
                {
                    init_plateauNormal(&actuelle);
                    actuelle.canplay_O = true;
                    actuelle.canplay_X = true;
                }
                else // (eMode == mode_aleatoire)
                {
                    init_plateauAlea(&actuelle);
                }
            }
            afficherplateau(actuelle);

            bool quitter = false;
            bool boolgagnant = false;
            do
            {
                switch((eMode == mode_special) ? tourspecial(&actuelle) : tournormal(&actuelle))
                {
                    case abandon_partie:
                        printf("Abandon de la partie\n");
                        printf("Le joueur '%c' a gagne !\n", actuelle.joueur == 'O' ? 'X' : 'O');
                        quitter = true;
                        break;
                        
                    case sauve_partie:
                        do{
                            if (!actuelle.nomSauvegarde[0])
                            {
                                printf("Veuiller donner le nom de la sauvegarde : ");
                                fflush(stdout);
                                fflush(stdin);
                                fgets(actuelle.nomSauvegarde, sizeof(actuelle.nomSauvegarde), stdin);
                                size_t len = strlen(actuelle.nomSauvegarde);
                                if (len && actuelle.nomSauvegarde[len-1] == '\n') 
                                { // si on a bien récupéré un chaîne et que le dernier caractère de la sauvegarde est '\n'
                                  // on le remplace par '\0'
                                    actuelle.nomSauvegarde[len-1] = '\0';
                                }
                            }
                            
                            if (sauvegarde(&actuelle, savepath))
                            {
                                printf("-------------\n"     
                                    "     Fin     \n"
                                    "-------------\n");
                                quitter = true;
                                break;
                            }
                            else
                            {
                                printf("Erreur de sauvegarde, donnez un autre nom");
                                strcpy(actuelle.nomSauvegarde, "");
                            }
                        } while (true);
                        break;

                    case continue_partie:
                        break;

                    case continue_afficher_partie:
                        afficherplateau(actuelle);                                      // affiche le plateau après que le joueur ait donné la case à jouer
                        break;
                }
                actuelle.joueur = actuelle.joueur == 'O' ? 'X' : 'O';           // change le pion du joueur devant jouer
                boolgagnant = gagnant(actuelle);
                if(boolgagnant)
                {
                    printf(enVERT "Le joueur '%c' a gagne !\n"enFin, actuelle.joueur == 'O' ? 'X' : 'O');
                    break;
                }
            } while(!quitter);    
        }
        printf("Voulez-vous continuer a jouer ? O/N\n");
        continuerajouer = getchar();
    } while (tolower(continuerajouer) == 'o');
    printf("Au revoir\n");        
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp24.h"
#include "bmp8.h"

void afficherBMP() {
    printf("Veuillez choisir une option :\n");
    printf("1. Image bmp8\n");
    printf("2. Image bmp24\n");
    printf("3. Quitter\n");

}

void afficherMenu8() {
    printf("Veuillez choisir une option :\n");
    printf("1. Ouvrir une image\n");
    printf("2. Sauvegarder une image\n");
    printf("3. Appliquer un filtre\n");
    printf("4. Afficher les informations de l'image\n");
    printf("5. Retourner au menu precedent\n");

}

void afficherMenuFiltres8() {
    printf("Veuillez choisir un filtre :\n");
    printf("1. Negatif\n");
    printf("2. Luminosite\n");
    printf("3. Threshold\n");
    printf("4. Box Blur\n");
    printf("5. Gaussian Blur\n");
    printf("6. Outline\n");
    printf("7. Emboss\n");
    printf("8. Sharpen\n");
    printf("9. Equalisation d'histogramme\n");
    printf("10. Retourner au menu precedent\n");
}


void afficherMenu24() {
    printf("Veuillez choisir une option :\n");
    printf("1. Ouvrir une image\n");
    printf("2. Sauvegarder une image\n");
    printf("3. Appliquer un filtre\n");
    printf("4. Appliquer un filtre de convolution\n");
    printf("5. Afficher les informations de l'image\n");
    printf("6. Retourner au menu precedent\n");
}

void afficherMenuFiltres24() {
    printf("Veuillez choisir un filtre :\n");
    printf("1. Negatif\n");
    printf("2. Luminosite\n");
    printf("3. Niveaux de gris\n");
    printf("4. Equalize\n");
    printf("5. Retourner au menu precedent\n");
}

void afficherConvolution24() {
    printf("Veuillez choisir un filtre :\n");
    printf("1. BoxBlur\n");
    printf("2. gaussianBlur\n");
    printf("3. outline\n");
    printf("4. emboss\n");
    printf("5. sharpen\n");
    printf("6. Retourner au menu precedent\n");
}

int obtenirChoix() {
    int choix;
    while (1) {
        printf(">>> Votre choix : ");
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n'); // Vide le buffer
            printf("Entree invalide. Veuillez entrer un nombre.\n");
        } else {
            break;
        }
    }
    return choix;
}

void afficherInformationsImage(t_bmp24 *img) {
    printf("Informations de l'image :\n");
    printf("Largeur : %d pixels\n", img->width);
    printf("Hauteur : %d pixels\n", img->height);
    printf("Profondeur de couleur : %d bits\n", img->colorDepth);
}

int main() {
    t_bmp24 *image24 = NULL;
    t_bmp8 *image8 = NULL;
    int choix;

    while (1){
        afficherBMP();
        int choixbmp = obtenirChoix();
        switch (choixbmp) {
            case 1: {
                int quitter8 = 1;
                while (quitter8) {
                    afficherMenu8();
                    choix = obtenirChoix();

                    switch (choix) {
                        case 1: {
                            char chemin[256];
                            printf("Chemin du fichier : ");
                            scanf("%s", chemin);
                            image8 = bmp8_loadImage(chemin);
                            if (image8 != NULL) {
                                printf("Image chargee avec succes !\n");
                            }
                            break;
                        }
                        case 2: {
                            if (image8 != NULL) {
                                char chemin[256];
                                printf("Chemin du fichier : ");
                                scanf("%s", chemin);
                                // Correction de l'ordre des paramètres
                                bmp8_saveImage(chemin, image8);
                                printf("Image sauvegardee avec succes !\n");
                            } else {
                                printf("Aucune image chargee.\n");
                            }
                            break;
                        }

                        case 3: {
                            if (image8 != NULL) {
                                int choixFiltre;
                                while (1) {
                                    afficherMenuFiltres8();
                                    choixFiltre = obtenirChoix();

                                    switch (choixFiltre) {
                                        case 1:
                                            bmp8_negative(image8);
                                            printf("Filtre applique avec succes !\n");
                                            break;
                                        case 2: {
                                            int luminosite;
                                            printf("Entrez la valeur de luminosite : ");
                                            scanf("%d", &luminosite);
                                            bmp8_brightness(image8, luminosite);
                                            printf("Filtre applique avec succes !\n");
                                            break;
                                        }
                                        case 3: {
                                            int threshold;
                                            printf("Entrez la valeur de threshold : ");
                                            scanf("%d", &threshold);
                                            bmp8_threshold(image8, threshold);
                                            printf("Filtre applique avec succes !\n");
                                            break;
                                        }
                                        case 9:
                                            bmp8_equalize(image8);
                                            printf("Egalisation d'histogramme appliquee avec succes !\n");
                                            break;
                                        case 10:
                                            break;
                                        default:
                                            printf("Choix invalide. Veuillez reessayer.\n");
                                            continue;
                                    }
                                    if (choixFiltre == 10) break;


                                }
                            } else {
                                printf("Aucune image chargee.\n");
                            }
                            break;
                        }
                        case 4: {
                            if (image8 != NULL) {
                                bmp8_printInfo(image8);
                            } else {
                                printf("Aucune image chargee.\n");
                            }
                            break;
                        }
                        case 5:
                            if (image8 != NULL) {
                                bmp8_free(image8);
                            }
                            printf("Au revoir !\n");
                            quitter8 = 0;
                            break;
                        default:
                            printf("Choix invalide. Veuillez reessayer.\n");
                    }
                }
            }
            case 2:{
                int quitter24 = 1;
                while (quitter24) {
                    afficherMenu24();
                    choix = obtenirChoix();

                    switch (choix) {
                        case 1: {
                            char chemin[256];
                            printf("Chemin du fichier : ");
                            scanf("%s", chemin);
                            image24 = bmp24_loadImage(chemin);
                            if (image24 != NULL) {
                                printf("Image chargee avec succes !\n");
                            }
                            break;
                        }
                        case 2: {
                            if (image24 != NULL) {
                                char chemin[256];
                                printf("Chemin du fichier : ");
                                scanf("%s", chemin);
                                bmp24_saveImage(image24, chemin);
                                printf("Image sauvegardee avec succes !\n");
                            } else {
                                printf("Aucune image chargee.\n");
                            }
                            break;
                        }
                        case 3: {
                            if (image24 != NULL) {
                                int choixFiltre;
                                while (1) {
                                    afficherMenuFiltres24();
                                    choixFiltre = obtenirChoix();

                                    switch (choixFiltre) {
                                        case 1:
                                            bmp24_negative(image24);
                                        printf("Filtre applique avec succes !\n");
                                        break;
                                        case 2: {
                                            int luminosite;
                                            printf("Entrez la valeur de luminosite : ");
                                            scanf("%d", &luminosite);
                                            bmp24_brightness(image24, luminosite);
                                            printf("Filtre applique avec succes !\n");
                                            break;
                                        }
                                        case 3:
                                            bmp24_grayscale(image24);
                                            printf("Filtre applique avec succes !\n");
                                            break;
                                        case 4 :
                                            bmp24_equalize(image24);
                                            printf("Filtre applique avec succes !\n");
                                            break;
                                        case 5:
                                            break;
                                        default:
                                            printf("Choix invalide. Veuillez reessayer.\n");
                                        continue;
                                    }
                                    if (choixFiltre == 5) break;
                                }
                            } else {
                                printf("Aucune image chargee.\n");
                            }
                            break;
                        }
                        case 4 : {
                            if (image24 != NULL) {
                                int choixconv;
                                afficherConvolution24();
                                choixconv = obtenirChoix();
                                switch (choixconv) {
                                    case 1:
                                        bmp24_boxBlur(image24);
                                    printf("Filtre applique avec succes !\n");
                                    break;
                                    case 2:
                                        bmp24_gaussianBlur(image24);
                                    printf("Filtre applique avec succes !\n");
                                    break;
                                    case 3:
                                        bmp24_outline(image24);
                                    printf("Filtre applique avec succes !\n");
                                    break;
                                    case 4:
                                        bmp24_emboss(image24);
                                    printf("Filtre applique avec succes !\n");
                                    break;
                                    case 5:
                                        bmp24_sharpen(image24);
                                    printf("Filtre applique avec succes !\n");
                                    break;
                                    case 6:
                                        break;
                                    default:
                                        printf("Choix invalide. Veuillez reessayer.\n");
                                    continue;
                                    if (choixconv == 6) break;
                                }
                            }
                            else {
                                printf("Aucune image chargee.\n");
                            }
                            break;
                        }

                        case 5: {
                            if (image24 != NULL) {
                                afficherInformationsImage(image24);
                            } else {
                                printf("Aucune image chargee.\n");
                            }
                            break;
                        }
                        case 6:
                            if (image24 != NULL) {
                                bmp24_free(image24);
                            }
                            printf("Au revoir !\n");
                            quitter24 = 0;
                            break;
                        default:
                            printf("Choix invalide. Veuillez reessayer.\n");
                    }
                }
                break;
            }
            case 3:
                if (image8) bmp8_free(image8);
                if (image24) bmp24_free(image24);
                printf("Au revoir !");
                return 0;
            default:
                printf("Choix invalide.\n");

        }
    }
}

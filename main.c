#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_RESET "\x1B[0m"
#define ANSI_BLACK "\x1B[30m"
#define ANSI_RED "\x1B[31m"
#define ANSI_GREEN "\x1B[32m"
#define ANSI_YELLOW "\x1B[33m"
#define ANSI_BLUE "\x1B[34m"
#define ANSI_MAGENTA "\x1B[35m"
#define ANSI_CYAN "\x1B[36m"
#define ANSI_WHITE "\x1B[37m"

int imposta_gioco(int status);
void gioca(int status);
void termina_gioco();

int main(int argc, char const *argv[])
{

    int status = 1; // 0 = APERTO, 1 = BLOCCATO, 2 = LAVORO MAPPE

    int option = 0;
    do
    {
        do{
            option = 0;
            printf("\n##############################\n");
            printf("\tSCALOGNA QUEST\t");
            printf("\n##############################");
            printf("\n#\n#\t-Come desideri procedere?");
            printf("\n#\t1) Imposta gioco\t\t");
            printf("\n#\t2) Gioca\t\t(%s)", (status == 1) ? "BLOCCATO" : "APERTO");
            printf("\n#\t3) Termina gioco\t\t");
            printf("\n#\n#\tScegli un'alternativa: ");
            scanf("%d", &option);
            while ((getchar()) != '\n'); // Per evitare che si inseriscano caratteri testuali
        }while(option < 1 || option > 3);
        switch (option)
        {
        case 1:
            status = 1;
            status = imposta_gioco(status);

            break;
        case 2:

            if (status == 1)
            {
                printf(ANSI_RED "\n\n\n\n\n\n\n\n\n\n\n\n###\tPer poter giocare è necessario");
                printf("\n###\tprima impostare il gioco:\n\n\n" ANSI_RESET);
                option = 0;
            }
            else
            {
                gioca(status);
            }
            break;
        case 3:
            termina_gioco();
            break;
        default:
            printf(ANSI_RED "\n\n\n\n\n\n\n\n\n\n###\tRilevato errore di sintassi.");
            printf("\n###\tTi prego di inserire un numero valido, riprovare:\n\n" ANSI_RESET);
            break;
        }

    } while (option != 3);

    return 0;
}

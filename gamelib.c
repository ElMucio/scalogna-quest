// Libreria importata da file esterno
#include "gamelib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Costanti per poter colorare le scritte del testo sul terminale
#define ANSI_RESET "\x1B[0m"
#define ANSI_BLACK "\x1B[30m"
#define ANSI_RED "\x1B[31m"
#define ANSI_GREEN "\x1B[32m"
#define ANSI_YELLOW "\x1B[33m"
#define ANSI_BLUE "\x1B[34m"
#define ANSI_MAGENTA "\x1B[35m"
#define ANSI_CYAN "\x1B[36m"
#define ANSI_WHITE "\x1B[37m"

// Variabili salvate globalmente sul file
struct Zona_segrete *pFirst = NULL;
struct Zona_segrete *pLast = NULL;

static int numero_giocatori = 0;
static struct Giocatore *nuovo_giocatore[4];
static struct Giocatore *nuovo_giocatore_copy[4];
static int pass = 0; // 0 = utilizzabile, 1 = non utilizzabile . Usata per l'interruzzione dell'avanzamento
int tesoro_preso = 0;

// Dichiarazioni funzioni (base) utilizzate poi in seguito
int imposta_gioco(int status);
void gioca(int status);
void termina_gioco();

// Dichiarazioni funzioni sottostanti all'imposta gioco
static void aggiungi_giocatore(int n_giocatori, int status);
static int numero_casuale(int minimo, int massimo);
static int genera_mappa(int status);
static void inserisci_zona(int status);
static void cancella_zona(int status);
static void stampa_mappa(int status);
static int chiudi_mappa(int status);
static int verifica_contenuto_array(int turno[], int j, int valore_supporto);

// Dichiarazioni funzioni sottostanti al gioca
static void avanza(int turno[], int ordine_turni);
static void indietreggia(int turno[], int ordine_turni);
static void stampa_giocatore(int turno[], int ordine_turni);
static void stampa_zona(struct Zona_segrete *z);
static void apri_porta(int turno[], int ordine_turni);
static void prendi_tesoro(int turno[], int ordine_turni);
static int scappa(int turno[], int ordine_turni, struct Giocatore *abitante);
static int combatti(int turno[], int ordine_turni, struct Giocatore *abitante);
static void gioca_potere_speciale(int turno[], int ordine_turni, struct Giocatore *abitante);
static struct Giocatore *genera_abitante_segrete(int turno[], int ordine_turni);
static int attacca(int sequenza, int turno[], int ordine_turni, struct Giocatore *abitante);
static int difendi(int attacco_danni, int sequenza, int turno[], int ordine_turni, struct Giocatore *abitante);
int passa(int ordine_turni);
static int controlla_multiplo(int gestione_turni, int numero_giocatori);
int lancio_dado(int min, int max, int faccia_dado);

// Si impostano i dati principali per giocare
int imposta_gioco(int status)
{

    int opzione_mappa = 0;

    do
    {
        numero_giocatori = 0;
        printf("\n\n\n#\t Inserire numero di giocatori   min 1  |----------------|  max 4\n#\t:");
        scanf("%d", &numero_giocatori);
        while ((getchar()) != '\n'); // Per evitare che si inseriscano caratteri testuali
        if (numero_giocatori < 1 || numero_giocatori > 4)
        {
            printf(ANSI_RED "\n\n\n\n\n\n\n\n\n###\tSi prega di inserire un numero tra 1 e 4:\n" ANSI_RESET);
        }

    } while (numero_giocatori < 1 || numero_giocatori > 4);

    aggiungi_giocatore(numero_giocatori, status);

    do
    {
        do
        {
            opzione_mappa = 0;
            printf("\n#\tImpostazioni mappa:");
            printf("\n#\t1) Crea nuova mappa.");
            printf("\n#\t2) Inserisci nuova zona.");
            printf("\n#\t3) Cancella una zona.");
            printf("\n#\t4) Stampa mappa.");
            printf("\n#\t5) Chiudi mappa.\n");
            printf("\n#\t: ");
            scanf("%d", &opzione_mappa);
            while ((getchar()) != '\n');
            if (opzione_mappa < 1 || opzione_mappa > 5)
            {

                printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 5:\n\n\n" ANSI_RESET);
            }
        } while (opzione_mappa < 1 || opzione_mappa > 5);

        switch (opzione_mappa)
        {
        case 1:
            status = genera_mappa(status);
            break;
        case 2:
            inserisci_zona(status);
            break;
        case 3:
            cancella_zona(status);
            break;
        case 4:
            stampa_mappa(status);
            break;
        case 5:
            status = chiudi_mappa(status);

            break;
        }

    } while (status != 0);
    return status;
}

static void aggiungi_giocatore(int n_giocatori, int status)
{

    char nome[253];
    int classe = 0;
    int sacrificio = 0;

    printf("\n\n###########");

    for (int i = 0; i < 4; i++)
    {
        nuovo_giocatore[i] = NULL;
    }

    for (int i = 0; i < n_giocatori; i++)
    {

        nuovo_giocatore[i] = (struct Giocatore *)malloc(sizeof(struct Giocatore));

        printf("\n\n#\tDATI GIOCATORE N.%d\n\n\n", i + 1);
        printf("\n#\tInserire il nome:\t");
        scanf("%s", nome);
        do
        {
            classe = 0;
            printf("\n#\tChe classe desideri usare tra le seguenti?\n");
            printf("#\t1) Barbaro\n");
            printf("#\t2) Nano\n");
            printf("#\t3) Elfo\n");
            printf("#\t4) Mago\n");
            printf("#\t:");
            scanf("%d", &classe);
            while ((getchar()) != '\n');
            if (classe < 1 || classe > 4)
            {

                printf(ANSI_RED "\n\n\n\n\n\n###\tSi prega di inserire un numero tra 1 e 4:\n\n\n" ANSI_RESET);
            }

        } while (classe < 1 || classe > 4);

        if (nuovo_giocatore[i] != NULL)
        {

            strncpy(nuovo_giocatore[i]->nome_giocatore, nome, sizeof(nuovo_giocatore[i]->nome_giocatore) - 1);
            nuovo_giocatore[i]->classe_giocatore = (classe - 1);
            nuovo_giocatore[i]->posizione = NULL;

            srand((unsigned int)time(NULL)); // Per inizializzare il generatore di numeri casuali

            if (classe == 1)
            { // BARBARO
                nuovo_giocatore[i]->p_vita = 8;
                nuovo_giocatore[i]->dadi_attacco = 3;
                nuovo_giocatore[i]->dadi_difesa = 2;
                nuovo_giocatore[i]->mente = numero_casuale(1, 2);
                nuovo_giocatore[i]->potere_speciale = 0;
            }
            else if (classe == 2)
            { // NANO
                nuovo_giocatore[i]->p_vita = 7;
                nuovo_giocatore[i]->dadi_attacco = 2;
                nuovo_giocatore[i]->dadi_difesa = 2;
                nuovo_giocatore[i]->mente = numero_casuale(2, 3);
                nuovo_giocatore[i]->potere_speciale = 1;
            }
            else if (classe == 3)
            { // ELFO
                nuovo_giocatore[i]->p_vita = 6;
                nuovo_giocatore[i]->dadi_attacco = 2;
                nuovo_giocatore[i]->dadi_difesa = 2;
                nuovo_giocatore[i]->mente = numero_casuale(3, 4);
                nuovo_giocatore[i]->potere_speciale = 1;
            }
            else if (classe == 4)
            { // MAGO
                nuovo_giocatore[i]->p_vita = 4;
                nuovo_giocatore[i]->dadi_attacco = 1;
                nuovo_giocatore[i]->dadi_difesa = 2;
                nuovo_giocatore[i]->mente = numero_casuale(4, 5);
                nuovo_giocatore[i]->potere_speciale = 3;
            }

            do
            {
                sacrificio = 0;
                printf("\n\n#\tSe desideri, puoi:\n\n");
                printf("#\t1) Sacrificare 1 punto mente, ottendendo + 1 vita.\n");
                printf("#\t2) Sacrificare 1 punto vita, ottendendo + 1 mente.\n");
                printf("#\t3) Lasciare statistiche attuali.\n");
                printf("#\t:");

                scanf("%d", &sacrificio);
                while ((getchar()) != '\n');
                if (sacrificio < 1 || sacrificio > 3)
                {

                    printf(ANSI_RED "\n\n\n\n\n\n###\tSi prega di inserire un numero tra 1 e 3:\n\n\n" ANSI_RESET);
                }

            } while (sacrificio < 1 || sacrificio > 3);

            switch (sacrificio)
            {
            case 1:
                nuovo_giocatore[i]->p_vita++;
                nuovo_giocatore[i]->mente--;
                printf(ANSI_GREEN "\n\n#\tModifiche per %s apportate con SUCCESSO\n\n" ANSI_RESET, nuovo_giocatore[i]->nome_giocatore);
                break;
            case 2:
                nuovo_giocatore[i]->p_vita--;
                nuovo_giocatore[i]->mente++;
                printf(ANSI_GREEN "\n\n#\tModifiche per %s apportate con SUCCESSO\n\n" ANSI_RESET, nuovo_giocatore[i]->nome_giocatore);
                break;
            case 3:
                printf(ANSI_GREEN "\n\n#\tIl giocatore %s manterra' le statistiche attuali\n\n" ANSI_RESET, nuovo_giocatore[i]->nome_giocatore);
                break;
            }
        }
        else
        {
            printf(ANSI_RED "\n\n\n###\tErrore rilevato nell'allocazione della memoria\n\n\n" ANSI_RESET);
        }

        printf(ANSI_CYAN "\n----------------------------------------------");
        printf("\n#\tGiocatore: %s", nuovo_giocatore[i]->nome_giocatore);
        if (nuovo_giocatore[i]->classe_giocatore == 0)
        {

            printf("\n#\tClasse: Barbaro");
        }
        else if (nuovo_giocatore[i]->classe_giocatore == 1)
        {

            printf("\n#\tClasse: Nano");
        }
        else if (nuovo_giocatore[i]->classe_giocatore == 2)
        {

            printf("\n#\tClasse: Elfo");
        }
        else if (nuovo_giocatore[i]->classe_giocatore == 3)
        {

            printf("\n#\tClasse: Mago");
        }
        printf("\n#\tPunti vita: %d", nuovo_giocatore[i]->p_vita);
        printf("\n#\tDadi attacco: %d", nuovo_giocatore[i]->dadi_attacco);
        printf("\n#\tDadi difesa: %d", nuovo_giocatore[i]->dadi_difesa);
        printf("\n#\tMente: %d", nuovo_giocatore[i]->mente);
        printf("\n#\tPotere speciale: %d", nuovo_giocatore[i]->potere_speciale);
        printf("\n----------------------------------------------\n\n" ANSI_RESET);
    }
}

static int numero_casuale(int minimo, int massimo)
{
    return rand() % (massimo - minimo + 1) + minimo;
    // nella generazione del numero casuale sono compresi anche gli stessi
    // massimi e minimi
}

static int genera_mappa(int status)
{
    struct Zona_segrete *zona_precedente;

    for (int j = 0; j < 15; j++)
    {

        struct Zona_segrete *nuova_zona = (struct Zona_segrete *)malloc(sizeof(struct Zona_segrete));

        nuova_zona->tipo_zona = (numero_casuale(1, 10) - 1);
        nuova_zona->tipo_tesoro = numero_casuale(1, 4);
        nuova_zona->tipo_porta = (numero_casuale(1, 3) - 1);

        if (j == 0)
        {
            // Prima zona

            pFirst = nuova_zona;
            zona_precedente = nuova_zona;
        }
        else if (j == 14)
        {
            // Ultima zona
            pLast = nuova_zona;
            nuova_zona->zona_precedente = zona_precedente;
            zona_precedente->zona_successiva = nuova_zona;
        }
        else
        {
            // Zone in mezzo

            nuova_zona->zona_precedente = zona_precedente;
            zona_precedente->zona_successiva = nuova_zona;
            zona_precedente = nuova_zona;
        }
    }

    printf(ANSI_GREEN "\n\n#\tMappa creata con successo\n\n" ANSI_RESET);

    status = 2;
    return status;
}

static void inserisci_zona(int status)
{
    int i_nuova_zona;
    struct Zona_segrete *zona_precedente;
    zona_precedente = NULL;
    struct Zona_segrete *z = pFirst;
    int quantita_zone = 0;

    if (status == 2)
    {

        while (z != NULL)
        {

            quantita_zone++;
            z = z->zona_successiva;
        }
        printf("\n\n###\tquantita_zone: %d\n", quantita_zone);

        do
        {
            i_nuova_zona = 0;
            printf("\n\n#\tIn che punto vuoi inserire la nuova zona?");
            printf("\n#\tmin 1 |-----------------------| max %d", quantita_zone);
            printf("\n#\t:");
            scanf("%d", &i_nuova_zona);
            while ((getchar()) != '\n');
            if (i_nuova_zona < 1 || i_nuova_zona > quantita_zone)
            {

                printf(ANSI_RED "\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e %d:\n\n\n" ANSI_RESET, quantita_zone);
            }

        } while (i_nuova_zona < 1 || i_nuova_zona > quantita_zone);

        z = pFirst;

        for (int i = 1; i <= quantita_zone; i++)
        {
            printf("\n\ni: %d", i);

            if (i == i_nuova_zona)
            {

                struct Zona_segrete *nuova_zona = (struct Zona_segrete *)malloc(sizeof(struct Zona_segrete));

                nuova_zona->tipo_zona = (numero_casuale(1, 10) - 1);

                nuova_zona->tipo_tesoro = (numero_casuale(1, 4) - 1);

                nuova_zona->tipo_porta = (numero_casuale(1, 3) - 1);

                if (pFirst == z)
                {
                    nuova_zona->zona_successiva = pFirst;
                    pFirst->zona_precedente = nuova_zona;
                    pFirst = nuova_zona;
                }
                else if (pLast == z)
                {
                    nuova_zona->zona_precedente = pLast;
                    pLast->zona_successiva = nuova_zona;
                    pLast = nuova_zona;
                }
                else
                {
                    z->zona_precedente->zona_successiva = nuova_zona;
                    nuova_zona->zona_precedente = z->zona_precedente;
                    nuova_zona->zona_successiva = z;
                    z->zona_precedente = nuova_zona;
                }
            }

            z = z->zona_successiva;
        }

        printf(ANSI_GREEN "\n\n#\tZona aggiunta con successo\n\n" ANSI_RESET);
    }
    else if (status == 1)
    {

        printf(ANSI_RED "\n\n###\tPer poter aggiungere una zona,\n");
        printf("###\te' necessario prima generare la mappa\n\n\n" ANSI_RESET);
    }
}

static void cancella_zona(int status)
{

    int i_elimina_zona;
    struct Zona_segrete *zona_precedente = NULL;
    struct Zona_segrete *z = pFirst;
    int quantita_zone = 0;

    if (status == 2)
    {

        while (z != NULL)
        {

            quantita_zone++;
            z = z->zona_successiva;
        }

        if (quantita_zone < 16)
        {
            printf(ANSI_RED "\n\n###\tImpossibile completare l'azione; il numero di\n");
            printf("###\tzone presenti e' gia' al valore minimo (15)\n\n\n" ANSI_RESET);
        }
        else if (quantita_zone >= 16)
        {

            do
            {
                i_elimina_zona = 0;
                printf("\n\n#\tIn che punto vuoi eliminare la zona?");
                printf("\n#\tmin 1 |-----------------------| max %d", quantita_zone);
                printf("\n#\t:");
                scanf("%d", &i_elimina_zona);
                while ((getchar()) != '\n');
                if (i_elimina_zona < 1 || i_elimina_zona > quantita_zone)
                {

                    printf(ANSI_RED "\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e %d:\n\n\n" ANSI_RESET, quantita_zone);
                }

            } while (i_elimina_zona < 1 || i_elimina_zona > quantita_zone);

            z = pFirst;

            for (int i = 1; i <= quantita_zone; i++)
            {

                if (i == i_elimina_zona)
                {

                    if (zona_precedente == NULL)
                    {
                        pFirst = z->zona_successiva;
                    }
                    else
                    {
                        zona_precedente->zona_successiva = z->zona_successiva;
                    }

                    free(z);
                    break;
                }

                zona_precedente = z;
                z = z->zona_successiva;
            }

            printf(ANSI_GREEN "\n\n#\tZona cancellata con successo\n\n" ANSI_RESET);
        }
    }
    else if (status == 1)
    {
        printf(ANSI_RED "\n\n###\tPer poter cancellare una zona,\n");
        printf("###\te' necessario prima generare la mappa\n\n\n" ANSI_RESET);
    }
}

static void stampa_mappa(int status)
{

    struct Zona_segrete *z = pFirst;

    if (status == 2)
    {

        printf("\n---------------------------------------------------------\n");

        while (z != NULL)
        {
            printf("\n#\tTipo zona: %d", z->tipo_zona);

            if (z->tipo_zona == 0)
            {
                printf("\n#\t\t\t CORRIDOIO");
            }
            else if (z->tipo_zona == 1)
            {
                printf("\n#\t\t\t SCALE");
            }
            else if (z->tipo_zona == 2)
            {
                printf("\n#\t\t\t SALA DEL BANCHETTO");
            }
            else if (z->tipo_zona == 3)
            {
                printf("\n#\t\t\t MAGAZZINO");
            }
            else if (z->tipo_zona == 4)
            {
                printf("\n#\t\t\t GIARDINO");
            }
            else if (z->tipo_zona == 5)
            {
                printf("\n#\t\t\t POSTO DI GUARDIA");
            }
            else if (z->tipo_zona == 6)
            {
                printf("\n#\t\t\t PRIGIONE");
            }
            else if (z->tipo_zona == 7)
            {
                printf("\n#\t\t\t CUCINA");
            }
            else if (z->tipo_zona == 8)
            {
                printf("\n#\t\t\t ARMERIA");
            }
            else if (z->tipo_zona == 9)
            {
                printf("\n#\t\t\t TEMPIO");
            }

            printf("\n#\tTipo tesoro: %d", z->tipo_tesoro);
            if (z->tipo_tesoro == 1)
            {
                printf("\n#\t\t\t NESSUN TESORO");
            }
            else if (z->tipo_tesoro == 2)
            {
                printf("\n#\t\t\t VELENO");
            }
            else if (z->tipo_tesoro == 3)
            {
                printf("\n#\t\t\t GUARIGIONE");
            }
            else if (z->tipo_tesoro == 4)
            {
                printf("\n#\t\t\t DOPPIA GUARIGIONE");
            }
            printf("\n#\tTipo porta: %d", z->tipo_porta);
            if (z->tipo_porta == 0)
            {
                printf("\n#\t\t\t NESSUNA PORTA");
            }
            else if (z->tipo_porta == 1)
            {
                printf("\n#\t\t\t PORTA NORMALE");
            }
            else if (z->tipo_porta == 2)
            {
                printf("\n#\t\t\t PORTA DA SCASSINARE");
            }
            printf("\n---------------------------------------------------------\n");

            z = z->zona_successiva;
        }
    }
    else if (status == 1)
    {
        printf(ANSI_RED "\n\n###\tPer poter stampare le zone,\n");
        printf("###\te' necessario prima generare la mappa\n\n\n" ANSI_RESET);
    }
}

static int chiudi_mappa(int status)
{

    struct Zona_segrete *z = pFirst;
    int quantita_zone = 0;

    while (z != NULL)
    {

        quantita_zone++;
        z = z->zona_successiva;
    }

    if (quantita_zone >= 15 && status == 2)
    {
        status = 0;
    }
    else if (quantita_zone < 15)
    {
        status = 1;
        printf(ANSI_RED "\n\n###\tPer poter chiudere questa sezione,\n");
        printf("###\te' necessario prima generare la mappa\n\n\n" ANSI_RESET);
    }
    return status;
}

void gioca(int status)
{

    int turno[numero_giocatori];
    int ordine_turni = 0;
    int opzione_gioco = 0;
    int numero_turni = -1;
    int chiusura_gioco;

    // Salvo una copia dei giocatori per i saluti finali
    for (int y = 0; y < numero_giocatori; y++)
    {
        nuovo_giocatore_copy[y] = nuovo_giocatore[y];
    }

    if (status == 0)
    {

        printf(ANSI_GREEN "\n\n#\tLa mappa è stata impostata correttamente,\n#\tora e' tempo di giocare:\n\n\n" ANSI_RESET);

        // Tutti i giocatori vanno nella prima zona per poter iniziare
        for (int i = 0; i < numero_giocatori; i++)
        {
            nuovo_giocatore[i]->posizione = pFirst;
        }

        do
        {
            // Per gestire l'alternarsi dei turni, in maniera anche randomica
            if (controlla_multiplo(ordine_turni, numero_giocatori) && pass == 0)
            {

                numero_turni++;
                printf(ANSI_YELLOW "\n\n#\t------------------------------------");
                printf("\n#\t-----------   TURNO %d   -----------\n", numero_turni);
                printf("#\t------------------------------------\n\n" ANSI_RESET);

                ordine_turni = 0;

                for (int i = 0; i < numero_giocatori; i++)
                {

                    int valore_supporto;

                    do
                    {

                        valore_supporto = numero_casuale(1, numero_giocatori);

                    } while (verifica_contenuto_array(turno, i, valore_supporto));// Per la gestione della parte randomica

                    turno[i] = valore_supporto;
                }

                pass = 2; // Metto a 2 in quanto se mettessi 0, e non avanzassi subito
                          //  ma applicherei un'altra funzione, il pass rimarrebbe 0
                          //  facendo poi cambiare il turno
            }

            if (nuovo_giocatore[turno[ordine_turni] - 1]->p_vita != 0) 
            {

                do
                {
                    opzione_gioco = 0;
                    printf("\n#\t------------GIOCATORE %s------------", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);
                    printf(ANSI_GREEN "\n#\t1) Passa turno." ANSI_RESET);

                    if (pass != 1)
                    {
                        printf(ANSI_GREEN "\n#\t2) Avanza." ANSI_RESET);
                    }
                    else if (pass == 1)
                    {
                        printf(ANSI_RED "\n#\t2) Avanza." ANSI_RESET);
                    }

                    printf(ANSI_GREEN "\n#\t3) Indietreggia.");
                    printf("\n#\t4) Valori giocatore.");
                    printf("\n#\t5) Zona attuale.");
                    printf("\n#\t6) Raccogli tesoro.");
                    printf("\n#\t: ");
                    scanf("%d", &opzione_gioco);
                    while ((getchar()) != '\n');
                    printf(ANSI_RESET "#\t------------------------------------\n\n" ANSI_RESET);
                    if (opzione_gioco < 1 || opzione_gioco > 6)
                    {

                        printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 6:\n\n\n" ANSI_RESET);
                    }
                } while (opzione_gioco < 1 || opzione_gioco > 6);

                switch (opzione_gioco)
                {
                case 1:
                    ordine_turni = passa(ordine_turni);

                    break;
                case 2:
                    if (pass != 1)
                    {
                        avanza(turno, ordine_turni);
                        if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro == 0)
                        {
                            nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro = numero_casuale(1, 4);
                        }
                    }
                    else
                    {
                        printf(ANSI_RED "\n\n\n###\tImpossibile avanzare\n" ANSI_RESET);
                    }
                    break;
                case 3:
                    indietreggia(turno, ordine_turni);
                    if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro == 0)// Per far rigenerare i tesori
                    {
                        nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro = numero_casuale(1, 4);
                    }
                    break;
                case 4:
                    stampa_giocatore(turno, ordine_turni);
                    break;
                case 5:
                    stampa_zona(nuovo_giocatore[(turno[ordine_turni] - 1)]->posizione);
                    break;
                case 6:
                    if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro > 0)
                    {
                        prendi_tesoro(turno, ordine_turni);
                    }
                    else
                    {
                        printf(ANSI_RED "\n\n###\tIl tesoro e' gia' stato preso.\n");
                        printf("###\tPuoi riprovare se ritorni nella stanza.\n" ANSI_RESET);
                    }

                    break;
                }
            }
            else
            {
                ordine_turni++;
            }

            chiusura_gioco = 0;

            for (int k = 0; k < numero_giocatori; k++) // Controllo che ci sia almeno un giocatore vivo, 
                                                        // per continuare a giocare
            {
                if (nuovo_giocatore[k]->p_vita == 0)
                {
                    pass = 0;
                    chiusura_gioco++;
                }
            }

        } while (chiusura_gioco != numero_giocatori);
    }
}

static int controlla_multiplo(int ordine_turni, int numero_giocatori)
{
    return (ordine_turni % numero_giocatori == 0);
}

static int verifica_contenuto_array(int turno[], int j, int valore_supporto)
{

    for (int i = 0; i < j; i++)
    {

        if (turno[i] == valore_supporto)
        {

            return 1; // presente
        }
    }

    return 0; // assente
}

static void avanza(int turno[], int ordine_turni)
{
    pass = 1;
    int possibilita_nascita = 0;
    int opzione_comabattimento = 0;
    int chiusura_menu = 0;
    int scelta_porta = 0;

    possibilita_nascita = numero_casuale(1, 10);

    if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione != pLast)
    {

        if (possibilita_nascita >= 1 && possibilita_nascita <= 3)
        {
            // nascita abitante
            struct Giocatore *abitante = NULL;
            abitante = genera_abitante_segrete(turno, ordine_turni);
            printf(ANSI_YELLOW "\n\n#\tE' stato generato l'abitante %s\n\n" ANSI_RESET, abitante->nome_giocatore);

            do
            {
                do
                {
                    opzione_comabattimento = 0;
                    printf("\n#\t------------GIOCATORE %s------------", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);

                    printf(ANSI_GREEN "\n#\t1) Valori giocatore.");
                    printf("\n#\t2) Scappa.");
                    printf("\n#\t3) Combatti.");

                    printf("\n#\t: ");
                    scanf("%d", &opzione_comabattimento);
                    while ((getchar()) != '\n');
                    printf("#\t------------------------------------\n\n" ANSI_RESET);
                    if (opzione_comabattimento < 1 || opzione_comabattimento > 3)
                    {

                        printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 3:\n\n\n" ANSI_RESET);
                    }

                } while (opzione_comabattimento < 1 || opzione_comabattimento > 3);

                switch (opzione_comabattimento)
                {
                case 1:
                    stampa_giocatore(turno, ordine_turni);
                    break;
                case 2:
                    if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione != pFirst){
                        chiusura_menu = scappa(turno, ordine_turni, abitante);
                    }else{
                        printf(ANSI_RED"\n\n###\tImpossibile scappare in quanto ti trovi nella prima zona"ANSI_RESET);
                    }
                    break;
                case 3:
                    chiusura_menu = combatti(turno, ordine_turni, abitante);
                    break;
                }

            } while (chiusura_menu > 2 || chiusura_menu < 1);

            if (chiusura_menu == 2 && nuovo_giocatore[turno[ordine_turni] - 1]->p_vita == 0)
            { // E' morto il giocatore X

                free(nuovo_giocatore[turno[ordine_turni] - 1]);
                nuovo_giocatore[turno[ordine_turni] - 1] == NULL;
            }
            else
            {
                // menu porte
                printf("\n\n#\t------------GIOCATORE %s------------", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);

                switch (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_porta)
                {
                case 0:
                    scelta_porta = 1;
                    break;
                case 1:
                    do
                    {
                        scelta_porta = 0;
                        printf(ANSI_GREEN "\n#\tE' presente una porta normale, come vuoi procedere:");
                        printf("\n#\t1) Apri porta.");
                        printf("\n#\t2) Annulla.");
                        printf("\n#\t: ");
                        scanf("%d", &scelta_porta);
                        while ((getchar()) != '\n');
                        printf(ANSI_RESET);

                        if (scelta_porta < 1 || scelta_porta > 2)
                        {
                            printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 2:\n\n\n" ANSI_RESET);
                        }

                    } while (scelta_porta < 1 || scelta_porta > 2);
                    break;
                case 2:
                    do
                    {
                        scelta_porta = 0;
                        printf(ANSI_GREEN "\n#\tE' presente una porta da scassinare, come vuoi procedere:");
                        printf("\n#\t1) Scassina porta.");
                        printf("\n#\t2) Annulla.");
                        printf("\n#\t: ");
                        scanf("%d", &scelta_porta);
                        while ((getchar()) != '\n');
                        printf(ANSI_RESET);

                        if (opzione_comabattimento < 1 || opzione_comabattimento > 2)
                        {
                            printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 2:\n\n\n" ANSI_RESET);
                        }

                    } while (scelta_porta < 1 || scelta_porta > 2);
                    break;
                }

                if (scelta_porta == 1)
                {

                    apri_porta(turno, ordine_turni);
                }
                else if (scelta_porta == 2)
                {

                    printf("\n\n#\tOperazione annullata.");
                }

                printf(ANSI_RESET "\n#\t------------------------------------\n\n");
            }
        }
        else
        {
            // si continua
            pass = 1;

            // menu porte

            printf("\n\n#\t------------GIOCATORE %s------------", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);

            switch (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_porta)
            {
            case 0:
                scelta_porta = 3;
                break;
            case 1:
                do
                {
                    scelta_porta = 0;
                    printf(ANSI_GREEN "\n#\tE' presente una porta normale, come vuoi procedere:");
                    printf("\n#\t1) Apri porta.");
                    printf("\n#\t2) Annulla.");
                    printf("\n#\t: ");
                    scanf("%d", &scelta_porta);
                    while ((getchar()) != '\n');
                    printf(ANSI_RESET);

                    if (scelta_porta < 1 || scelta_porta > 2)
                    {
                        printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 2:\n\n\n" ANSI_RESET);
                    }

                } while (scelta_porta < 1 || scelta_porta > 2);
                break;
            case 2:
                do
                {
                    scelta_porta = 0;
                    printf(ANSI_GREEN "\n#\tE' presente una porta da scassinare, come vuoi procedere:");
                    printf("\n#\t1) Scassina porta.");
                    printf("\n#\t2) Annulla.");
                    printf("\n#\t: ");
                    scanf("%d", &scelta_porta);
                    while ((getchar()) != '\n');
                    printf(ANSI_RESET);

                    if (scelta_porta < 1 || scelta_porta > 2)
                    {
                        printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 2:\n\n\n" ANSI_RESET);
                    }

                } while (scelta_porta < 1 || scelta_porta > 2);
                break;
            }

            if (scelta_porta == 1)
            {

                apri_porta(turno, ordine_turni);
            }
            else if (scelta_porta == 2)
            {

                printf(ANSI_YELLOW "\n\n#\tOperazione annullata." ANSI_RESET);
            }
            else if (scelta_porta == 3)
            {
                printf(ANSI_YELLOW "\n#\tNon ci sono porte presenti, avanzerai senza alcun problema." ANSI_RESET);
            }

            printf(ANSI_RESET "\n#\t------------------------------------\n\n");
        }
    }
    else
    {
        // nascita abitante per forza, essendo l'ultima stanza
        printf("\n\n###################################################################\n");
        printf("\n\t\t\tULTIMA ZONA");
        printf("\n\tE' ORA DI COMBATTERE CONTRO L'ULTIMO ABITANTE");
        printf("\n\t\t   E PUNTARE A VINCERE!!!");
        printf("\n\n###################################################################\n");

        struct Giocatore *abitante = NULL;
        abitante = genera_abitante_segrete(turno, ordine_turni);
        printf(ANSI_YELLOW "\n#\tE' stato generato l'abitante %s,\nBUONA FORTUNA!\n\n" ANSI_RESET, abitante->nome_giocatore);

        do
        {
            do
            {
                opzione_comabattimento = 0;
                printf("\n#\t------------GIOCATORE %s------------", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);

                printf(ANSI_GREEN "\n#\t1) Valori giocatore.");
                printf("\n#\t2) Scappa.");
                printf("\n#\t3) Combatti.");

                printf("\n#\t: ");
                scanf("%d", &opzione_comabattimento);
                while ((getchar()) != '\n');
                printf("#\t------------------------------------\n\n" ANSI_RESET);
                if (opzione_comabattimento < 1 || opzione_comabattimento > 3)
                {

                    printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 3:\n\n\n" ANSI_RESET);
                }

            } while (opzione_comabattimento < 1 || opzione_comabattimento > 3);

            switch (opzione_comabattimento)
            {
            case 1:
                stampa_giocatore(turno, ordine_turni);
                break;
            case 2:
                if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione != pFirst){
                    chiusura_menu = scappa(turno, ordine_turni, abitante);
                }else{
                    printf(ANSI_RED"\n\n###\tImpossibile scappare in quanto ti trovi nella prima zona"ANSI_RESET);
                }
                break;
            case 3:
                chiusura_menu = combatti(turno, ordine_turni, abitante);
                break;
            }

        } while (chiusura_menu > 2 || chiusura_menu < 1);

        if (chiusura_menu == 2)
        {
            // il giocatore ha vinto
            printf("\n\n#\tIL GIOCATORE %s HA VINTOOOO!!!", nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
            termina_gioco();
        }
    }
}

int passa(int ordine_turni)
{
    ordine_turni++;
    pass = 0;
    return ordine_turni++;
}

static void indietreggia(int turno[], int ordine_turni)
{
    int possibilita_nascita = 0;
    possibilita_nascita = numero_casuale(1, 10);

    if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione == pFirst)
    {
        printf(ANSI_RED "\n\n\n###\tImpossibile indietreggiare, ti trovi nella prima zona.\n\n" ANSI_RESET);
    }
    else
    {
        nuovo_giocatore[turno[ordine_turni] - 1]->posizione = nuovo_giocatore[turno[ordine_turni] - 1]->posizione->zona_precedente;
        printf(ANSI_YELLOW "\n\n\n#\t%s e' indietreggiato di zona\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);

        if (possibilita_nascita >= 1 && possibilita_nascita <= 3)
        {
            // nascita abitante

            pass = 1;
            int opzione_comabattimento;
            int chiusura_menu = 0;
            int scelta_porta = 0;

            struct Giocatore *abitante = NULL;
            abitante = genera_abitante_segrete(turno, ordine_turni);
            printf(ANSI_YELLOW "\n\n#\tE' stato generato l'abitante %s\n\n" ANSI_RESET, abitante->nome_giocatore);
            do
            {
                do
                {
                    opzione_comabattimento = 0;
                    printf("\n#\t------------GIOCATORE %s------------", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);
                    printf(ANSI_GREEN "\n#\t1) Valori giocatore.");
                    printf("\n#\t2) Scappa.");
                    printf("\n#\t3) Combatti.");
                    printf("\n#\t: ");
                    scanf("%d", &opzione_comabattimento);
                    while ((getchar()) != '\n');
                    printf("#\t------------------------------------\n\n" ANSI_RESET);
                    if (opzione_comabattimento < 1 || opzione_comabattimento > 3)
                    {
                        printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 3:\n\n\n" ANSI_RESET);
                    }
                } while (opzione_comabattimento < 1 || opzione_comabattimento > 3);
                switch (opzione_comabattimento)
                {
                case 1:
                    stampa_giocatore(turno, ordine_turni);
                    break;
                case 2:
                    if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione != pFirst){
                        chiusura_menu = scappa(turno, ordine_turni, abitante);
                    }else{
                        printf(ANSI_RED"\n\n###\tImpossibile scappare in quanto ti trovi nella prima zona"ANSI_RESET);
                    }
                    break;
                case 3:
                    chiusura_menu = combatti(turno, ordine_turni, abitante);
                    break;
                }
            } while (chiusura_menu > 2 || chiusura_menu < 1);
            if (chiusura_menu == 2 && nuovo_giocatore[turno[ordine_turni] - 1]->p_vita == 0)
            { // E' morto il giocatore X
                free(nuovo_giocatore[turno[ordine_turni] - 1]);
                nuovo_giocatore[turno[ordine_turni] - 1] == NULL;
            }
            pass = 1;
        }
    }
}

static void stampa_giocatore(int turno[], int ordine_turni)
{

    printf(ANSI_CYAN "\n----------------------------------------------");

    printf("\n#\tGiocatore: %s", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);

    if (nuovo_giocatore[(turno[ordine_turni] - 1)]->classe_giocatore == 0)
    {

        printf("\n#\tClasse: Barbaro");
    }
    else if (nuovo_giocatore[(turno[ordine_turni] - 1)]->classe_giocatore == 1)
    {

        printf("\n#\tClasse: Nano");
    }
    else if (nuovo_giocatore[(turno[ordine_turni] - 1)]->classe_giocatore == 2)
    {

        printf("\n#\tClasse: Elfo");
    }
    else if (nuovo_giocatore[(turno[ordine_turni] - 1)]->classe_giocatore == 3)
    {

        printf("\n#\tClasse: Mago");
    }

    printf("\n#\tPunti vita: %d", nuovo_giocatore[(turno[ordine_turni] - 1)]->p_vita);
    printf("\n#\tDadi attacco: %d", nuovo_giocatore[(turno[ordine_turni] - 1)]->dadi_attacco);
    printf("\n#\tDadi difesa: %d", nuovo_giocatore[(turno[ordine_turni] - 1)]->dadi_difesa);
    printf("\n#\tMente: %d", nuovo_giocatore[(turno[ordine_turni] - 1)]->mente);
    printf("\n#\tPotere speciale: %d", nuovo_giocatore[(turno[ordine_turni] - 1)]->potere_speciale);
    printf("\n----------------------------------------------\n\n" ANSI_RESET);
}

static void stampa_zona(struct Zona_segrete *z)
{

    printf(ANSI_MAGENTA "\n\n-----------------------------------------------");
    printf("\n#\tZONA:");
    if (z->tipo_zona == 0)
    {
        printf("\n#\tTi trovi in CORRIDOIO");
    }
    else if (z->tipo_zona == 1)
    {
        printf("\n#\tTi trovi sulle SCALE");
    }
    else if (z->tipo_zona == 2)
    {
        printf("\n#\tTi trovi nella SALA DEL BANCHETTO");
    }
    else if (z->tipo_zona == 3)
    {
        printf("\n#\tTi trovi nel MAGAZZINO");
    }
    else if (z->tipo_zona == 4)
    {
        printf("\n#\tTi trovi nel GIARDINO");
    }
    else if (z->tipo_zona == 5)
    {
        printf("\n#\tTi trovi al POSTO DI GUARDIA");
    }
    else if (z->tipo_zona == 6)
    {
        printf("\n#\tTi trovi in PRIGIONE");
    }
    else if (z->tipo_zona == 7)
    {
        printf("\n#\tTi trovi in CUCINA");
    }
    else if (z->tipo_zona == 8)
    {
        printf("\n#\tTi trovi in ARMERIA");
    }
    else if (z->tipo_zona == 9)
    {
        printf("\n#\tTi trovi nel TEMPIO");
    }
    printf("\n#\tTESORO:");

    if (z->tipo_tesoro < 1)
    {
        printf("\n#\tNessun tesoro presente");
    }
    else
    {
        printf("\n#\tE' presente un tesoro");
    }

    printf("\n#\tPORTA:");
    if (z->tipo_porta == 0)
    {
        printf("\n#\tNessuna porta presente");
    }
    else
    {
        printf("\n#\tE' presente una porta");
    }

    printf("\n-----------------------------------------------\n\n" ANSI_RESET);
}

static void apri_porta(int turno[], int ordine_turni)
{
    int numero_random = 0;
    int faccia_dado = 0;

    int opzione_comabattimento;
    int chiusura_menu = 0;

    if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_porta == 0)
    {

        printf(ANSI_YELLOW "\n\n#\t|Non è presente alcuna porta in questa stanza.\n" ANSI_RESET);

        nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro = (numero_casuale(1, 4) - 1);

        nuovo_giocatore[turno[ordine_turni] - 1]->posizione = nuovo_giocatore[turno[ordine_turni] - 1]->posizione->zona_successiva;
        printf(ANSI_YELLOW "#\t|%s e' riuscito ad avanzare di zona\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
    }
    else if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_porta == 2)
    {
        numero_random = rand() % 100;
        printf(ANSI_YELLOW "\n\n#\tLancio del dado ..." ANSI_RESET);
        faccia_dado = lancio_dado(1, 6, faccia_dado);

        if (faccia_dado <= nuovo_giocatore[turno[ordine_turni] - 1]->mente)
        {

            printf(ANSI_YELLOW "\n\n#\t|La porta e' stata scassinata con successo.\n" ANSI_RESET);

            nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro = (numero_casuale(1, 4) - 1);

            nuovo_giocatore[turno[ordine_turni] - 1]->posizione = nuovo_giocatore[turno[ordine_turni] - 1]->posizione->zona_successiva;
            printf(ANSI_YELLOW "#\t|%s e' riuscito ad avanzare di zona\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
        }
        else
        {

            numero_random = numero_casuale(1, 10);
            if (numero_random == 1)
            {

                // Il giocatore torna alla prima zona
                nuovo_giocatore[turno[ordine_turni] - 1]->posizione = pFirst;
                printf(ANSI_YELLOW "\n\n#\t|Il giocatore %s non ha scassinato la porta,", nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
                printf("\n#\t|ed e' tornato alla prima zona.\n" ANSI_RESET);
            }
            else if (numero_random >= 2 && numero_random <= 6)
            {

                // Meno un punto vita
                nuovo_giocatore[turno[ordine_turni] - 1]->p_vita = nuovo_giocatore[turno[ordine_turni] - 1]->p_vita - 1;
                printf(ANSI_YELLOW "\n\n#\t|Il giocatore %s ha perso 1 di vita\n", nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
                printf("#\t|ed e' rimasto nella stessa zona.\n\n" ANSI_RESET);
            }
            else if (numero_random >= 7 && numero_random <= 10)
            {

                struct Giocatore *abitante = NULL;
                abitante = genera_abitante_segrete(turno, ordine_turni);
                printf(ANSI_YELLOW "\n\n#\tE' stato generato l'abitante %s\n\n" ANSI_RESET, abitante->nome_giocatore);

                do
                {
                    do
                    {
                        opzione_comabattimento = 0;
                        printf("\n#\t------------GIOCATORE %s------------", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);

                        printf(ANSI_GREEN "\n#\t1) Valori giocatore.");
                        printf("\n#\t2) Combatti.");

                        printf("\n#\t: ");
                        scanf("%d", &opzione_comabattimento);
                        while ((getchar()) != '\n');
                        printf("#\t------------------------------------\n\n" ANSI_RESET);
                        if (opzione_comabattimento < 1 || opzione_comabattimento > 2)
                        {

                            printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valori tra 1 e 2:\n\n\n" ANSI_RESET);
                        }

                    } while (opzione_comabattimento < 1 || opzione_comabattimento > 2);

                    switch (opzione_comabattimento)
                    {
                    case 1:
                        stampa_giocatore(turno, ordine_turni);
                        break;
                    case 2:
                        chiusura_menu = combatti(turno, ordine_turni, abitante);
                        break;
                    }

                } while (chiusura_menu != 2);

                if (chiusura_menu == 2 && nuovo_giocatore[turno[ordine_turni] - 1]->p_vita == 0)
                { // E' morto il giocatore X

                    free(nuovo_giocatore[turno[ordine_turni] - 1]);
                    nuovo_giocatore[turno[ordine_turni] - 1] == NULL;
                }
                else
                {

                    printf(ANSI_YELLOW "\n\n#\t|Il giocatore e' riuscito a sconfiggere l'abitante\n");
                    printf("#\t|ma non puo' ancora proseguire.\n\n" ANSI_RESET);
                }
            }
        }
    }
    else if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_porta == 1)
    {
        printf(ANSI_YELLOW "\n\n#\t|La porta e' stata aperta con successo.\n" ANSI_RESET);

        nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro = (numero_casuale(1, 4) - 1);

        nuovo_giocatore[turno[ordine_turni] - 1]->posizione = nuovo_giocatore[turno[ordine_turni] - 1]->posizione->zona_successiva;
        printf(ANSI_YELLOW "#\t|%s e' riuscito ad avanzare di zona\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
    }
}

static void prendi_tesoro(int turno[], int ordine_turni)
{

    if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro == 1)
    { // nessun tesoro

        printf(ANSI_YELLOW "\n\n\n#\tNon e' presente alcun tesoro\n\n" ANSI_RESET);
    }
    else if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro == 2)
    { // veleno

        if (nuovo_giocatore[turno[ordine_turni] - 1]->p_vita >= 2)
        {

            nuovo_giocatore[turno[ordine_turni] - 1]->p_vita = nuovo_giocatore[turno[ordine_turni] - 1]->p_vita - 2;
        }
        else
        {
            nuovo_giocatore[turno[ordine_turni] - 1]->p_vita = 0;
        }
        printf(ANSI_YELLOW "\n\n\n#\t%s e' stato AVVELENATO (-2 vita)\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
        nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro = 0;
    }
    else if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro == 3)
    { // guarigione

        nuovo_giocatore[turno[ordine_turni] - 1]->p_vita = nuovo_giocatore[turno[ordine_turni] - 1]->p_vita + 1;
        printf(ANSI_YELLOW "\n\n\n#\t%s ha GUADAGNATO (+1 vita)\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
        nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro = 0;
    }
    else if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro == 4)
    { // doppia guarigione

        nuovo_giocatore[turno[ordine_turni] - 1]->p_vita = nuovo_giocatore[turno[ordine_turni] - 1]->p_vita + 2;
        printf(ANSI_YELLOW "\n\n\n#\t%s ha GUADAGNATO (+2 vita)\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
        nuovo_giocatore[turno[ordine_turni] - 1]->posizione->tipo_tesoro = 0;
    }

    if (nuovo_giocatore[turno[ordine_turni] - 1]->p_vita == 0)
    { // E' morto il giocatore X

        printf(ANSI_YELLOW "#\tIl giocatore %s e' MORTO\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
        free(nuovo_giocatore[turno[ordine_turni] - 1]);
        nuovo_giocatore[turno[ordine_turni] - 1] == NULL;
    }
}

static int scappa(int turno[], int ordine_turni, struct Giocatore *abitante)
{
    int faccia_dado = 0;
    faccia_dado = lancio_dado(1, 6, faccia_dado);
    if (faccia_dado <= nuovo_giocatore[turno[ordine_turni] - 1]->mente)
    {

        nuovo_giocatore[turno[ordine_turni] - 1]->posizione = nuovo_giocatore[turno[ordine_turni] - 1]->posizione->zona_precedente;
        printf(ANSI_YELLOW "\n\n#\tIl giocatore %s e' riusito a scappare.\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
        return 1;
    }
    else
    {

        int attacco = 0;
        int conteggio;
        printf(ANSI_CYAN "\n\n#\tNon sei riuscito a scappare, ora combatterai\n#\tcon la meta' dei tuoi dadi difesa.\n");
        attacco = attacca(1, turno, ordine_turni, abitante);

        printf(ANSI_YELLOW "\n#\tTurno difesa del giocatore : %s\n", nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);

        int numero_dado_difesa[nuovo_giocatore[turno[ordine_turni] - 1]->dadi_difesa / 2];
        printf("#\tLancio del dado...\n");
        conteggio = 0;
        for (int i = 0; i < nuovo_giocatore[turno[ordine_turni] - 1]->dadi_difesa / 2; i++)
        {
            numero_dado_difesa[i] = numero_casuale(1, 6);

            switch (numero_dado_difesa[i])
            { // da 1 a 3 teschio, 4 e 5 scudo bianco e 6 scudo nero
            case 1:
                // Teschio
                break;
            case 2:
                // Teschio
                break;
            case 3:
                // Teschio
                break;
            case 4:
                // Scudo bianco
                conteggio++;
                break;
            case 5:
                conteggio++;
                break;
            case 6:
                // Scudo nero
                break;
            }
            printf("\n\n");
        }

        if ((attacco - conteggio) > 0)
        {
            nuovo_giocatore[turno[ordine_turni] - 1]->p_vita = nuovo_giocatore[turno[ordine_turni] - 1]->p_vita - (attacco - conteggio);
            printf(ANSI_YELLOW "\n\n#\tIl giocatore ha perso %d di vita\n" ANSI_RESET, (attacco - conteggio));
            return 1;
        }
        else
        {
            printf(ANSI_YELLOW "\n\n#\tIl giocatore %s e' riuscito a parare tutti gli attacchi\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
            return 1;
        }
    }
}

static int combatti(int turno[], int ordine_turni, struct Giocatore *abitante)
{

    int dado_giocatore = 0;
    int dado_abitante = 0;
    int sequenza = 0; // 0 = giocatore, 1 = abitante delle segrete
    int attacco_danni = 0;
    int option = 0;
    int death_or_live = 0; // 1 = vivo | 2 = morto

    do
    {
        option = 0;
        printf("\n#\t------------GIOCATORE %s------------", nuovo_giocatore[(turno[ordine_turni] - 1)]->nome_giocatore);

        printf(ANSI_GREEN "\n#\t1) Lancia dadi.");
        printf("\n#\t2) Usa potere speciale.");
        printf("\n#\t: ");
        scanf("%d", &option);
        while ((getchar()) != '\n');
        printf(ANSI_RESET "#\t------------------------------------\n\n" ANSI_RESET);
        if (option < 1 || option > 2)
        {

            printf(ANSI_RED "\n\n\n###\tErrore nell'inserimento, immettere solo valore 1 o 2:\n\n\n" ANSI_RESET);
        }
    } while (option < 1 || option > 2);

    switch (option)
    {
    case 1:

        printf(ANSI_YELLOW "\n\n#\tLancio del dado...\n\n");
        printf("#\tGiocatore: \n");
        dado_giocatore = lancio_dado(1, 6, dado_giocatore);
        printf(ANSI_YELLOW "\n\n#\tAbitante delle segrete: \n");
        dado_abitante = lancio_dado(1, 6, dado_abitante);
        printf("\n" ANSI_RESET);

        if (dado_giocatore >= dado_abitante)
        { // Inizia giocatore

            sequenza = 0;
            do // Ciclo per alternarsi nel combattimento
            {
                attacco_danni = attacca(sequenza, turno, ordine_turni, abitante);
                if (sequenza == 0)
                {
                    sequenza = 1;
                }
                else if (sequenza == 1)
                {
                    sequenza = 0;
                }
                death_or_live = difendi(attacco_danni, sequenza, turno, ordine_turni, abitante);

            } while (nuovo_giocatore[turno[ordine_turni] - 1]->p_vita != 0 && abitante->p_vita != 0);

            free(abitante);
            abitante = NULL;

            return death_or_live;
        }
        else
        { // Inizia abitante

            sequenza = 1;
            do
            {
                attacco_danni = attacca(sequenza, turno, ordine_turni, abitante); // 0 = giocatore, 1 = abitante delle segrete
                if (sequenza == 1)
                {
                    sequenza = 0;
                }
                else if (sequenza == 0)
                {
                    sequenza = 1;
                }
                death_or_live = difendi(attacco_danni, sequenza, turno, ordine_turni, abitante);

            } while (nuovo_giocatore[turno[ordine_turni] - 1]->p_vita != 0 && abitante->p_vita != 0);

            free(abitante);
            abitante = NULL;

            return death_or_live;
        }

        break;
    case 2:
        if ((int)nuovo_giocatore[(turno[ordine_turni] - 1)]->potere_speciale == 0)
        {

            printf(ANSI_YELLOW "\n\n#\tNon puoi usare il potere speciale in quanto ha raggiunto: %d" ANSI_RESET, nuovo_giocatore[(turno[ordine_turni] - 1)]->potere_speciale);
            return 0;
        }
        else
        {

            gioca_potere_speciale(turno, ordine_turni, abitante);
            return 1;
        }
        break;
    }
    return 0;
}

static int attacca(int sequenza, int turno[], int ordine_turni, struct Giocatore *abitante)
{

    int conteggio = 0;
    if (sequenza == 0)
    { // inizia giocatore
        printf(ANSI_YELLOW "\n\n#\tTurno attacco del giocatore: %s\n", nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);

        int numero_dado_attacco[nuovo_giocatore[turno[ordine_turni] - 1]->dadi_attacco];
        printf("#\tLancio del dado...\n" ANSI_RESET);
        conteggio = 0;
        for (int i = 0; i < nuovo_giocatore[turno[ordine_turni] - 1]->dadi_attacco; i++)
        {
            numero_dado_attacco[i] = numero_casuale(1, 6);

            switch (numero_dado_attacco[i])
            { // da 1 a 3 teschio, 4 e 5 scudo bianco e 6 scudo nero
            case 1:
                // Teschio
                conteggio++;
                break;
            case 2:
                // Teschio
                conteggio++;
                break;
            case 3:
                // Teschio
                conteggio++;
                break;
            case 4:
                // Scudo bianco
                break;
            case 5:
                // Scuso bianco
                break;
            case 6:
                // Scudo nero
                break;
            }
        }
        return conteggio;
    }
    else if (sequenza == 1)
    { // inizia abitante delle segrete

        printf(ANSI_YELLOW "\n\n#\tTurno attacco dell'abitante: %s\n", abitante->nome_giocatore);

        int numero_dado_attacco[abitante->dadi_attacco];
        printf(ANSI_YELLOW "#\tLancio del dado...\n\n" ANSI_RESET);
        conteggio = 0;
        for (int i = 0; i < abitante->dadi_attacco; i++)
        {
            numero_dado_attacco[i] = numero_casuale(1, 6);

            switch (numero_dado_attacco[i])
            { // da 1 a 3 teschio, 4 e 5 scudo bianco e 6 scudo nero
            case 1:
                // Teschio
                conteggio++;
                break;
            case 2:
                // Teschio
                conteggio++;
                break;
            case 3:
                // Teschio
                conteggio++;
                break;
            case 4:
                // Scudo bianco
                break;
            case 5:
                // Scudo bianco
                break;
            case 6:
                // Scudo nero
                break;
            }
        }
        return conteggio;
    }
    else
    {
        return 0;
    }
}

static int difendi(int attacco_danni, int sequenza, int turno[], int ordine_turni, struct Giocatore *abitante)
{
    int conteggio = 0;

    if (sequenza == 0)
    { // inizia giocatore
        printf(ANSI_YELLOW "\n#\tTurno difesa del giocatore : %s\n", nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);

        int numero_dado_difesa[nuovo_giocatore[turno[ordine_turni] - 1]->dadi_difesa];
        printf("#\tLancio del dado...\n");
        conteggio = 0;
        for (int i = 0; i < nuovo_giocatore[turno[ordine_turni] - 1]->dadi_difesa; i++)
        {
            numero_dado_difesa[i] = numero_casuale(1, 6);

            switch (numero_dado_difesa[i])
            { // da 1 a 3 teschio, 4 e 5 scudo bianco e 6 scudo nero
            case 1:
                // Teschio
                break;
            case 2:
                // Teschio
                break;
            case 3:
                // Teschio
                break;
            case 4:
                // Scudo bianco
                conteggio++;
                break;
            case 5:
                // Scudo bianco
                conteggio++;
                break;
            case 6:
                // Scudo nero
                break;
            }
            printf("\n\n");
        }
        if (attacco_danni < conteggio)
        {
            conteggio = attacco_danni;
        }
        if ((attacco_danni - conteggio) < nuovo_giocatore[turno[ordine_turni] - 1]->p_vita)
        {
            nuovo_giocatore[turno[ordine_turni] - 1]->p_vita = nuovo_giocatore[turno[ordine_turni] - 1]->p_vita - (attacco_danni - conteggio);
            printf(ANSI_YELLOW "#\tIl giocatore ha perso %d di vita\n\n" ANSI_RESET, (attacco_danni - conteggio));
            return 1; // vivo
        }
        else
        {
            if (nuovo_giocatore[turno[ordine_turni] - 1]->posizione != pLast)
            {
                printf(ANSI_YELLOW "#\tIl giocatore %s e' MORTO\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
            }
            else
            {
                printf(ANSI_YELLOW "#\tIl giocatore %s HA PERSO!!!\n\n" ANSI_RESET, nuovo_giocatore[turno[ordine_turni] - 1]->nome_giocatore);
            }
            nuovo_giocatore[turno[ordine_turni] - 1]->p_vita = 0;
            abitante->p_vita = 0;
            return 2; // morto
        }
    }
    else if (sequenza == 1)
    { // inizia abitante delle segrete

        printf(ANSI_YELLOW "\n#\tTurno difesa dell'abitante: %s\n", abitante->nome_giocatore);

        int numero_dado_difesa[abitante->dadi_difesa];
        printf("#\tLancio del dado...\n" ANSI_RESET);
        conteggio = 0;
        for (int i = 0; i < abitante->dadi_difesa; i++)
        {
            numero_dado_difesa[i] = numero_casuale(1, 6);

            switch (numero_dado_difesa[i])
            { // da 1 a 3 teschio, 4 e 5 scudo bianco e 6 scudo nero
            case 1:
                // Teschio
                break;
            case 2:
                // Teschio
                break;
            case 3:
                // Teschio
                break;
            case 4:
                // Scudo bianco
                break;
            case 5:
                // Scudo bianco
                break;
            case 6:
                // Scudo nero
                conteggio++;
                break;
            }
            printf("\n\n");
        }
        if (attacco_danni < conteggio)
        {
            conteggio = attacco_danni;
        }
        if ((attacco_danni - conteggio) < abitante->p_vita)
        {
            abitante->p_vita = abitante->p_vita - (attacco_danni - conteggio);
            printf(ANSI_YELLOW "#\tL'abitante ha perso %d di vita\n\n" ANSI_RESET, (attacco_danni - conteggio));
        }
        else
        {
            printf(ANSI_YELLOW "\n#\tL'abitante %s e' MORTO\n\n" ANSI_RESET, abitante->nome_giocatore);
            abitante->p_vita = 0;
            return 2;
        }
    }
}

static void gioca_potere_speciale(int turno[], int ordine_turni, struct Giocatore *abitante)
{

    if ((int)nuovo_giocatore[(turno[ordine_turni] - 1)]->potere_speciale - 1 > 0)
    {
        (int)nuovo_giocatore[(turno[ordine_turni] - 1)]->potere_speciale--;
        printf(ANSI_YELLOW "\n\n#\tHai usato il potere speciale e sconfitto l'abitante." ANSI_RESET);
    }
    else
    {
        nuovo_giocatore[(turno[ordine_turni] - 1)]->potere_speciale = 0;
    }

    printf(ANSI_YELLOW "\n\n#\tTi rimane di potere speciale: %d" ANSI_RESET, nuovo_giocatore[(turno[ordine_turni] - 1)]->potere_speciale);
}

int lancio_dado(int min, int max, int faccia_dado)
{
    faccia_dado = numero_casuale(min, max);

    if (faccia_dado > 0 && faccia_dado < 7)
    {

        switch (faccia_dado)
        {
        case 1:
            printf(ANSI_YELLOW "\n\n");
            printf("#\t+-----+\n");
            printf("#\t|     |\n");
            printf("#\t|  *  |\n");
            printf("#\t|     |\n");
            printf("#\t+-----+\n" ANSI_RESET);
            break;
        case 2:
            printf(ANSI_YELLOW "\n\n");
            printf("#\t+-----+\n");
            printf("#\t| *   |\n");
            printf("#\t|     |\n");
            printf("#\t|   * |\n");
            printf("#\t+-----+\n" ANSI_RESET);
            break;
        case 3:
            printf(ANSI_YELLOW "\n\n");
            printf("#\t+-----+\n");
            printf("#\t| *   |\n");
            printf("#\t|  *  |\n");
            printf("#\t|   * |\n");
            printf("#\t+-----+\n" ANSI_RESET);
            break;
        case 4:
            printf(ANSI_YELLOW "\n\n");
            printf("#\t+-----+\n");
            printf("#\t| * * |\n");
            printf("#\t|     |\n");
            printf("#\t| * * |\n");
            printf("#\t+-----+\n" ANSI_RESET);
            break;
        case 5:
            printf(ANSI_YELLOW "\n\n");
            printf("#\t+-----+\n");
            printf("#\t| * * |\n");
            printf("#\t|  *  |\n");
            printf("#\t| * * |\n");
            printf("#\t+-----+\n" ANSI_RESET);
            break;
        case 6:
            printf(ANSI_YELLOW "\n\n");
            printf("#\t+-----+\n");
            printf("#\t| * * |\n");
            printf("#\t| * * |\n");
            printf("#\t| * * |\n");
            printf("#\t+-----+\n" ANSI_RESET);
            break;
        default:
            printf(ANSI_RED "\n\n###\tErrore nel lancio del dado.\n\n" ANSI_RESET);
        }
        return faccia_dado;
    }
    else
    {
        printf(ANSI_RED "\n\n###\tRilevato errore nel lancio del dado\n" ANSI_RESET);
        return 0;
    }
}

static struct Giocatore *genera_abitante_segrete(int turno[], int ordine_turni)
{

    struct Giocatore *abitante_delle_segrete;
    char nome[10][20] = {
        "Agrus",
        "Boris",
        "Charlie",
        "Rusch",
        "Ex",
        "Puerto",
        "Show",
        "Hank",
        "Ivy",
        "Jack"};

    abitante_delle_segrete = NULL;

    abitante_delle_segrete = (struct Giocatore *)malloc(sizeof(struct Giocatore));

    strcpy(abitante_delle_segrete->nome_giocatore, nome[numero_casuale(0, 9)]);

    abitante_delle_segrete->posizione = nuovo_giocatore[turno[ordine_turni] - 1]->posizione;

    abitante_delle_segrete->p_vita = numero_casuale(1, 7);

    abitante_delle_segrete->dadi_attacco = numero_casuale(1, 5);

    abitante_delle_segrete->dadi_difesa = numero_casuale(2, 4);

    abitante_delle_segrete->mente = numero_casuale(2, 4);

    printf(ANSI_CYAN "\n--------------------------------------------------");
    printf("\n#\tAbitante delle segrete: %s", abitante_delle_segrete->nome_giocatore);
    printf("\n#\tPunti vita: %d", abitante_delle_segrete->p_vita);
    printf("\n#\tDadi attacco: %d", abitante_delle_segrete->dadi_attacco);
    printf("\n#\tDadi difesa: %d", abitante_delle_segrete->dadi_difesa);
    printf("\n#\tMente: %d", abitante_delle_segrete->mente);
    printf("\n--------------------------------------------------\n\n" ANSI_RESET);

    return abitante_delle_segrete;
}

void termina_gioco()
{

    printf(ANSI_CYAN "\n\n\n########################################" ANSI_RESET);
    printf("\n\tGrazie per aver giocato ;-)");
    for (int y = 0; y < numero_giocatori; y++)
    {
        printf("\n\t- %s", nuovo_giocatore_copy[y]->nome_giocatore);
    }
    printf(ANSI_CYAN "\n########################################\n\n\n" ANSI_RESET);
}
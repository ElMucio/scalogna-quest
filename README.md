# Scalogna Quest

> **EN** — Turn-based dungeon crawler for the terminal, written in C for a university course on procedural
> programming. 1 to 4 players pick a class, build a map stored as a doubly linked list of zones, then
> explore it: open or pick doors, collect treasures and fight the dungeon's inhabitants with dice rolls.

> Progetto finale del corso di **Programmazione Procedurale** — Università degli Studi di Perugia  
> Autore: **Luca Sonno**

Gioco di ruolo a turni da terminale.

## Come si gioca

**1. Imposta gioco**
- Da 1 a 4 giocatori, ognuno sceglie nome e classe: **Barbaro**, **Nano**, **Elfo** o **Mago**,
  ciascuna con punti vita, dadi di attacco e difesa, mente e potere speciale diversi.
- Si può scambiare un punto mente con un punto vita, o viceversa.
- Si costruisce la mappa: generazione casuale, inserimento e cancellazione di zone, stampa e chiusura
  (servono almeno 15 zone). Ogni zona ha un tipo (corridoio, armeria, tempio...), un tesoro e una porta.

**2. Gioca**
- A turno ogni giocatore può avanzare, indietreggiare, raccogliere il tesoro della zona
  (veleno, guarigione o doppia guarigione) e controllare le proprie statistiche.
- Le porte normali si aprono; quelle da scassinare richiedono un tiro di dado contro il valore di mente.
- Entrando in una zona può comparire un **abitante delle segrete**: si combatte a colpi di dado,
  si scappa nella zona precedente oppure si usa il potere speciale.
- Nell'**ultima zona** compare l'abitante finale: chi lo sconfigge vince.

## Compilazione

Il codice usa solo la libreria standard del C:

```
gcc -std=c11 -Wall -o scalogna main.c gamelib.c
./scalogna
```

I colori usano i codici ANSI, quindi serve un terminale che li supporti
(Linux, macOS, Windows Terminal).

## Struttura

```
main.c       menu principale
gamelib.h    strutture: giocatore e zona della mappa (lista doppiamente concatenata)
gamelib.c    impostazione, mappa, turni, porte, tesori e combattimenti
```

---

Licenza [MIT](LICENSE).

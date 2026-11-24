struct Giocatore
{

    char nome_giocatore[254];

    enum classe_giocatore
    {
        barbaro,
        nano,
        elfo,
        mago
    } classe_giocatore;

    struct Zona_segrete *posizione;

    unsigned char p_vita;

    unsigned char dadi_attacco;

    unsigned char dadi_difesa;

    unsigned char mente;

    unsigned char potere_speciale;
};

struct Zona_segrete
{

    struct Zona_segrete *zona_successiva;

    struct Zona_segrete *zona_precedente;

    enum Tipo_zona
    {
        corridoio,
        scala,
        sala_banchetto,
        magazzino,
        giardino,
        posto_guardia,
        prigione,
        cucina,
        armeria,
        tempio
    } tipo_zona;

    enum Tipo_tesoro
    {
        preso,
        nessun_tesoro,
        veleno,
        guarigione,
        doppia_guarigione
    } tipo_tesoro;

    enum Tipo_porta
    {
        nessuna_porta,
        porta_normale,
        porta_da_scassinare
    } tipo_porta;
};

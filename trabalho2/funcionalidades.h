#pragma once

    #include "header.h"
    #include "registro.h"
    #include "ioDados.h"
    #include "utils.h"

    #define CREATE_TABLE 1
    #define SELECT_FROM 2
    #define SELECT_WHERE 3
    #define RECUPERACAO_RRN 4
    #define CREATE_INDEX 5
    #define SELECT_WHERE_USING_INDEX 6
    #define DELETE 7
    #define INSERT_INTO 8
    #define UPDATE 9


    void CreateTable(char *arquivoEntrada, char *arquivoSaida);
    void SelectFrom(char *arquivoEntrada);
    void SelectWhere(char *arquivoEntrada, int nroBuscas);
    void RecuperacaoRRN(char *arquivoEntrada, int RRN);
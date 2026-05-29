#pragma once

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

    #define TAM_CABECALHO 17

    typedef struct {
        char status; // indica se o arquivo está consistente ('1') ou inconsistente ('0')
        int topo; // armazena o RRN de um registro logicamente removido ou '-1' caso não haja nenhum registro logicamente removido
        int proxRRN; // próximo RRN disponível
        int nroEstacoes; // indica a quantidade de estações diferentes no arquivo de dados
        int nroParesEstacao; // indica a quantidade de pares (codEstacao, codProxEstacao) 
    } Header;

    Header *InicializarCabecalho();

    void LerCabecalhoBIN(FILE *arquivoBIN, Header *cabecalho);
    void EscreverCabecalhoBIN(FILE* arquivoBIN, const Header* cabecalho);
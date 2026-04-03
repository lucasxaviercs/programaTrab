#include <stdio.h>
#include <stdlib.h>

#include "funcionalidades.h"

int main(int argc, char *argv[]) {
    // Lê o argumento da linha de comando para saber qual funcionalidade executar
    int funcionalidade = atoi(argv[1]);

    switch (funcionalidade) {
        case 1: 
            printf("Funcionalidade 1 selecionada.\n");
            break;
        case 2:
            printf("Funcionalidade 2 selecionada.\n");
            break;
        case 3:
            printf("Funcionalidade 3 selecionada.\n");
            break;
        case 4:
            int RRN = atoi(argv[3]);
            RecuperacaoRRN(argv[2], RRN);
            break;
        default:
            printf("Funcionalidade inválida.\n");
            break;
    }
}
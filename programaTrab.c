#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
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
            printf("Funcionalidade 4 selecionada.\n");
            break;
        default:
            printf("Funcionalidade inválida.\n");
            break;
    }
}
#include "arquivosLerEscrever.h"

#define SEPARADOR_CAMPOS ","


static int LerCampoFixo(char **linha){
    char *campo = strsep(linha, SEPARADOR_CAMPOS);

    // Os valores de campos nulos devem ser representados pelo valor -1
    if(campo == NULL || campo[0] == '\0') return -1;

    return (int)strtol(campo, NULL, 10);
}


static char *LerCampoVariavel(char **linha){
    char *campo = strsep(linha, SEPARADOR_CAMPOS);

    if(campo == NULL || campo[0] == '\0'){
        return NULL;
    }

    char *nome = malloc( (strlen(campo) + 1) * sizeof(char) );
    strcpy(nome, campo);
    return nome;
}


Header LerCabecalhoCSV(FILE *arquivoCSV, Header cabecalho){
    char *buffer =  malloc(256 * sizeof(char));
    if(buffer == NULL){
        printf("Erro na alocação de memória!\n");
        exit(1);
    }

    if(fgets(buffer, 256, arquivoCSV) == NULL){
        // MENSAGEM EXIGIDA quando houver falha no processamento de algum arquivo
        printf("Falha no processamento do arquivo.\n");
        free(buffer);
        buffer = NULL;
        exit(1);
    }
    
    free(buffer);
    buffer = NULL;

    // Inicialização do cabeçalho do arquivo binário
    cabecalho.status = '0'; // Para quando abrir para escrita estar inconsistente
    cabecalho.topo = -1; // Não há registros logicamente removidos
    cabecalho.proxRRN = 0; // O próximo RRN disponível deve ser iniciado com o valor 0
    cabecalho.nroEstacoes = 0; // Indica a quantidade de estações
    cabecalho.nroParesEstacao = 0; // Indica a quantidade de pares

    return h;
}

Registro LerRegistroCSV(FILE *arquivoCSV, Registro registroDados){
    char *buffer = malloc(512 * sizeof(char));
    if(buffer == NULL){
        printf("Erro na alocação de memória!\n);
    }

    if(fgets(buffer, 512, arquivoCSV) == NULL){
        registroDados.removido = '1'
        free(buffer);
        buffer = NULL;
        return registroDados;sd
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    char *linha = buffer;

    registroDados.removido = '0';
    registroDados.proximo = -1;
    registroDados.codEstacao = LerCampoFixo(&linha);
    registroDados.codLinha = LerCampoFixo(&linha);
    registroDados.codProxEstacao = LerCampoFixo(&linha);
    registroDados.distProxEstacao = LerCampoFixo(&linha);
    registroDados.codLinhaIntegra = LerCampoFixo(&linha);
    registroDados.codEstIntegra = LerCampoFixo(&linha);
    registroDados.nomeEstacao = LerCampoVariavel(&linha);
    registroDados.tamNomeEstacao = (registroDados.nomeEstacao == NULL) ? 0 : strlen(registroDados.nomeEstacao);
    registroDados.nomeLinha = LerCampoVariavel(&linha);
    registroDados.tamNomeLinha = (registroDados.nomeLinha) ? 0 : strlen(registroDados.nomeLinha);

    free(buffer);
    buffer = NULL;

    return registroDados;
}
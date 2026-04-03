#include "funcionalidades.h"


void CreateTable(char *arquivoEntrada, char *arquivoSaida){
    // Abrindo arquivo CSV para leitura
    FILE *arquivoCSV = fopen(arquivoEntrada, "r");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo CSV
    if(arquivoCSV == NULL){
        MensagemErro();
        fclose(arquivoCSV);
        return;
    }

    // Abrindo arquivo BIN para escrita binária
    FILE *arquivoBIN = fopen(arquivoSaida, "wb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }

    // INICIALIZAÇÃO DE ESTRUTURAS NA MEMÓRIA PRIMÁRIA
    // inicialiamos os valores do cabeçalho
    Header cabecalho = InicializarCabecalho(arquivoCSV);

    // inicializamos estruturas auxiliares que nos vão ajudar a determinar nroEstacoes e nroParesEstacoes
    ControleEstacoes *controleEstacoes = InicializarControleEstacoes();
    ControlePares *controlePares = InicializarControlePares();

    // "RESERVAMOS" OS PRIMEIROS 17 BYTES (0-16) DO ARQUIVO BINÁRIAO PARA O CABEÇALHO
    EscreverCabecalhoBIN(arquivoBIN, &cabecalho);


    // LOOP PARA PROCESSAR OS DADOS DO ARQUIVO DE ENTRADA
    // vamos obter os registros de dados do arquivo CSV e escrever eles no arquivo binário
    while(VerificaEOF(arquivoCSV)){

        Registro registroDados = LerRegistroCSV(arquivoCSV, registroDados);
        if(registroDados.removido == '1'){ // se o registro está logicamente removido
            LiberarStringRegistro(&registroDados);
            break;
        }
        
        RegistrarEstacaoUnica(controleEstacoes, registroDados.nomeEstacao);
        RegistrarParUnico(controlePares, registroDados.codEstacao, registroDados.codProxEstacao);

        EscreverRegistroBIN(arquivoBIN, &registroDados);

        cabecalho.proxRRN++;

        LiberarStringRegistro(&registroDados);
    }

    //ATUALIZACAO FINAL DO CABECALHO
    cabecalho.nroEstacoes = controleEstacoes->totalEstacoesUnicas;
    cabecalho.nroParesEstacao = controlePares->totalParesUnicos;
    cabecalho.status = '1'; // Finalizando o uso do arquivo
    EscreverCabecalhoBIN(arquivoBIN, &cabecalho); // O fseek dentro da função nos garante que irá sobrescrever os primeiro 17 bytes que é referente ao cabeçalho mesmo

    //REALIZANDO OS ÚLTIMOS DESALOCAMENTO DE MEMÓRIA E FECHANDO OS ARQUIVOS
    LiberarControleEstacoes(controleEstacoes);
    LiberarControlePares(controlePares);

    fclose(arquivoCSV);
    fclose(arquivoBIN);

    // Como exigido no PDF do trabalho
    BinarioNaTela(arquivoSaida);

}

void RecuperacaoRRN(char *arquivoEntrada, int RRN){
    // Abertura do arquivo BIN para leitura
    FILE *arquivoBIN = fopen(arquivoEntrada, "rb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        return;
    }

    // Leitura do cabeçalho para obter o próximo RRN disponível
    Header cabecalho;
    cabecalho = LerCabecalhoBIN(arquivoBIN, cabecalho);

    // Se o RRN for inválido (negativo ou além dos registros existentes), aborta
    if(RRN < 0 || RRN >= cabecalho.proxRRN){
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }

    // Cálculo do byte offset do registro para dar fseek
    int byteoffset = TAM_CABECALHO + RRN * TAM_REGISTRO;
    fseek(arquivoBIN, byteoffset, SEEK_SET);

    // Leitura do registro na posição calculada
    Registro registroDados;
    registroDados.nomeEstacao = NULL;
    registroDados.nomeLinha = NULL;
    registroDados = LerRegistroBIN(arquivoBIN, registroDados);

    // Se o registro estiver logicamente removido, não deve ser exibido
    if(registroDados.removido == '1'){
        MensagemRegistroNaoEncontrado();
        LiberarStringRegistro(&registroDados);
        fclose(arquivoBIN);
        return;
    }

    // Chama imprimirRegistro para exibir os campos do registro lido
    ImprimirRegistro(&registroDados);

    LiberarStringRegistro(&registroDados);
    fclose(arquivoBIN);
}
#include "funcionalidades.h"

/*Lê os registros de um arquivo CSV e constrói a "tabela" no formato de um arquivo binário.
Tendo todo o controle de inicializar, contabilizar, atualizar os dados do cabeçalho e dos registros
para realizar a gravação no arquivo binário*/
void CreateTable(char *arquivoEntrada, char *arquivoSaida){
    // Abrindo arquivo CSV para leitura
    FILE *arquivoCSV = fopen(arquivoEntrada, "r");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo CSV
    if(arquivoCSV == NULL){
        MensagemErro();
        return;
    }

    // Abrindo arquivo BIN para escrita binária
    FILE *arquivoBIN = fopen(arquivoSaida, "wb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        return;
    }

    // INICIALIZAÇÃO DE ESTRUTURAS NA MEMÓRIA PRIMÁRIA
    // inicialiamos os valores do cabeçalho
    Header *cabecalho = InicializarCabecalho();

    // inicializamos estruturas auxiliares que nos vão ajudar a determinar nroEstacoes e nroParesEstacoes
    ControleEstacoes *controleEstacoes = InicializarControleEstacoes();
    ControlePares *controlePares = InicializarControlePares();

    // "RESERVAMOS" OS PRIMEIROS 17 BYTES (0-16) DO ARQUIVO BINÁRIO PARA O CABEÇALHO
    fseek(arquivoBIN, TAM_CABECALHO, SEEK_SET);

    // DESCARTAMOS A LINHA ZERO DO CSV QUE CONTÉM APENAS AS NOMENCLATURAS DAS COLUNAS
    IgnorarLinhaZeroCSV(arquivoCSV);

    Registro registroDados; // Utilizaremos como registro auxiliar para não trazer tudo do disco deuma vez pra memória primária 

    // LOOP PARA PROCESSAR OS DADOS DO ARQUIVO DE ENTRADA
    // vamos obter os registros de dados do arquivo CSV e escrever eles no arquivo binário
    while(VerificaEOF(arquivoCSV)){

        LerRegistroCSV(arquivoCSV, &registroDados);
        if(registroDados.removido == '1'){ // se o registro está logicamente removido
            LiberarStringRegistro(&registroDados);
            break;
        }
        
        // Registra os nomes únicos e pares únicos das estações para contagem do nroEstacoes e nroParesEstacao
        RegistrarEstacaoUnica(controleEstacoes, registroDados.nomeEstacao);
        RegistrarParUnico(controlePares, registroDados.codEstacao, registroDados.codProxEstacao);

        EscreverRegistroBIN(arquivoBIN, &registroDados);

        cabecalho->proxRRN++;

        LiberarStringRegistro(&registroDados);
    }

    //ATUALIZACAO FINAL DO CABECALHO
    cabecalho->nroEstacoes = controleEstacoes->totalEstacoesUnicas;
    cabecalho->nroParesEstacao = controlePares->totalParesUnicos;
    cabecalho->status = '1'; // Finalizando o uso do arquivo

    EscreverCabecalhoBIN(arquivoBIN, cabecalho); // O fseek dentro da função nos garante que irá sobrescrever os primeiro 17 bytes que é refente ao próprio cabeçalho

    //REALIZANDO OS ÚLTIMOS DESALOCAMENTO DE MEMÓRIA E FECHANDO OS ARQUIVOS
    LiberarControleEstacoes(controleEstacoes);
    LiberarControlePares(controlePares);

    free(cabecalho);
    cabecalho = NULL;

    fclose(arquivoCSV);
    fclose(arquivoBIN);

    // Como exigido no PDF do trabalho
    BinarioNaTela(arquivoSaida);

}

/*Lê o arquivo binário do começo ao fim para mostrar tudo para o usuário ao imprimir na tela.*/
void SelectFrom(char *arquivoEntrada){
    // Abrindo arquivo BIN para leitura binária
    FILE *arquivoBIN = fopen(arquivoEntrada, "rb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        return;
    }    

    // Leitura do cabecalho do arquivo binário
    Header cabecalho;
    LerCabecalhoBIN(arquivoBIN, &cabecalho);

    if(cabecalho.status == '0'){ //arquivo inconsistente 
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }

    if(cabecalho.proxRRN == 0){ // não há nenhum dado gravado, apenas o arquivo foi criado
        MensagemRegistroNaoEncontrado();
        fclose(arquivoBIN);
        return;
    }

    int contadorImpressos = 0;
    Registro registroDados;

    // LOOP PARA PROCESSAR OS DADOS DO REGISTRO
    for(int i = 0; i < cabecalho.proxRRN; i++){

        // Fazemos a leitura do registro no arquivo (disco), salvamos na RAM
        // e fazemos a verificação se o registro está removido ou não
        LerRegistroBIN(arquivoBIN, &registroDados);

        // Só iremos imprimir se não estiver logicamente removido
        // '1' == LOGICAMENTE REMOVIDO e '0' == NÃO ESTÁ MARCADO COMO REMOVIDO
        if(registroDados.removido != '1'){ // apenas uma segurança a mais para confirmar que NÃO ESTÁ REMOVIDO
            ImprimirRegistro(&registroDados);
            contadorImpressos++;
        }

        // Libera as strings alocadas pelo RegistroBIN
        LiberarStringRegistro(&registroDados);
    }

    // Se o contador de registros impressos for ZERO ao final do loop,
    // significa que todos estavam marcados como logicamente removidos
    if(contadorImpressos == 0){
        MensagemRegistroNaoEncontrado();
    }

    fclose(arquivoBIN);
}

/*Faz uma busca personalizada usando critérios de busca (FILTROS) que o usuário passou.
Verificando linha por linha e apenas imprimindo na tela se o registro convergir com os critérios exigidos.*/
void SelectWhere(char *arquivoEntrada, int nroBuscas){
    // Abrindo arquivo BIN para leitura binária
    FILE *arquivoBIN = fopen(arquivoEntrada, "rb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        return;
    }    

    // Leitura do cabecalho do arquivo binário
    Header cabecalho;
    LerCabecalhoBIN(arquivoBIN, &cabecalho); // Para sabermos o status e o proxRRN

    if(cabecalho.status == '0'){ //arquivo inconsistente 
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }    

    // Executa o número de buscas solicitadas
    for(int buscaAtual = 0; buscaAtual < nroBuscas; buscaAtual++){
        // Lê quantos filtros serão aplicados nesta busca específica
        // Quantidade de vezes que o par de critério pode repetir em uma busca
        int m_nroCriterios;
        if(scanf("%d", &m_nroCriterios) != 1) break;

        // Armazeno em uma array de struct o par de filtro que serão aplicados
        CriterioBusca criterios[m_nroCriterios];

        // Lemos os critérios de busca passados (FILTRO PASSADOS)
        LerCriteriosBusca(criterios, m_nroCriterios);

        // Verificamos se a busca atual contém o campo codEstacao
        int buscaID = 0;
        for(int c = 0; c < m_nroCriterios; c++){
            if(strcmp(criterios[c].nomeDoCampo, "codEstacao") == 0){
                buscaID = 1;
                break;
            }
        }

        // Posicionamos o ponteiro do arquivo para o início dos dados (IGNORANDO O CABEÇALHO = 17 Bytes)
        fseek(arquivoBIN, TAM_CABECALHO, SEEK_SET);

        Registro registroDados;
        int registroEncontrado = 0; // Flag de controle ( 0 = false, 1 = true)

        for(int rrnAtual = 0; rrnAtual < cabecalho.proxRRN; rrnAtual++){
            // "Zera" os ponteiros para prevenir lixo da memoria na leitura
            registroDados.nomeEstacao = NULL;
            registroDados.nomeLinha = NULL;

            // Preenche a struct com os dados lido do arquvio e verifica se não está removido
            LerRegistroBIN(arquivoBIN, &registroDados);
            
            // Ignoramos todos os registros inválidos
            if(registroDados.removido != '1'){ // apenas uma segurança a mais para confirmar que NÃO ESTÁ REMOVIDO
                int atendeTodosCriterios = 1; // Assumimos que registro serve (true), até que se prove o contrário

                // Verifica o registro atual a partir dos criterios de busca que foram passados
                for(int criterioAtual = 0; criterioAtual < m_nroCriterios; criterioAtual++){
                    // Retornaremos 1 se o campo atende ao critério buscado
                    if(VerificaCriterioBusca(&registroDados, criterios[criterioAtual].nomeDoCampo, criterios[criterioAtual].valorBuscado) != 1){
                        atendeTodosCriterios = 0; // Falhou em dos pares de criterios (filtro) passados
                        break; // Descartamos
                    }
                }
                
                // Se passou pelas verificações, imprimimos na tela
                if(atendeTodosCriterios == 1){
                    ImprimirRegistro(&registroDados);
                    registroEncontrado = 1; // Sinalizamos que encontramos um que atende ao filtro passado

                    if(buscaID == 1){ // Caso o ID da busca (codEstacao) bater, não ficamos percorrendo os demais registro
                        LiberarStringRegistro(&registroDados);
                        break;
                    }
                }
            } 

            // Desaloco as strings alocadas pelo LerRegistroBIN
            LiberarStringRegistro(&registroDados);
        }

        // Passei por todo o arquivo e a flag não se alterou, continuou 0, avisa o usuário que o registro não foi encontrado
        if(registroEncontrado == 0){
            MensagemRegistroNaoEncontrado();
        }

        if(buscaAtual < nroBuscas - 1){
            printf("\n");
        }
    }

    fclose(arquivoBIN);
}

/*Por meio de cálculos envolvendo o RRN, é capaz de acessar direto o registro desejado.*/
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
    LerCabecalhoBIN(arquivoBIN, &cabecalho);

    // Verificação se o arquivo está consistente
    if(cabecalho.status == '0'){
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }

    // Se o RRN for inválido (negativo ou além dos registros existentes), aborta
    if(RRN < 0 || RRN >= cabecalho.proxRRN){
        MensagemRegistroNaoEncontrado();
        fclose(arquivoBIN);
        return;
    }

    // Cálculo do byte offset do registro para dar fseek
    int byteoffset = TAM_CABECALHO + (RRN * TAM_REGISTRO);
    fseek(arquivoBIN, byteoffset, SEEK_SET);

    // Leitura do registro na posição calculada
    Registro registroDados;
    registroDados.nomeEstacao = NULL;
    registroDados.nomeLinha = NULL;
    LerRegistroBIN(arquivoBIN, &registroDados);

    // Se o registro estiver logicamente removido, não deve ser exibido
    if(registroDados.removido == '1'){
        MensagemRegistroNaoEncontrado();
        LiberarStringRegistro(&registroDados);
        fclose(arquivoBIN);
        return;
    }

    // Chama ImprimirRegistro para exibir os campos do registro lido
    ImprimirRegistro(&registroDados);

    LiberarStringRegistro(&registroDados);
    fclose(arquivoBIN);
}

void CriarIndex(FILE *arquivoDados, FILE* arquivoIndex){
    // abre o arquivo de dados para leitura, se deu erro aborta
    FILE *arquivoDadosBIN = fopen(arquivoDados, "rb");
    if (arquivoDadosBIN == NULL) {
        mensagemErro();
        return;
    }

    // verifica a consistência do arquivo de dados, se tiver inconsistente aborta
    Header cabecalhoDados;
    LerCabecalhoBIN(arquivoDadosBIN, &cabecalhoDados);
    if (cabecalhoDados.status == '0'){
        MensagemErro();
        fclose(arquivoDadosBIN);
        return;
    }

    // abre o arquivo de índice para escrita, se deu erro aborta
    FILE *arquivoIndexBIN = fopen(arquivoIndex, "wb+");
    if (arquivoIndexBIN == NULL) {
        mensagemErro();
        fclose(arquivoDadosBIN);
        return;
    }

    // marca como inconsistente durante o processo de criação do índice
    IndexHeader cabecalhoIndex;
    cabecalhoIndex.status = '0';
    fwrite(&cabecalhoIndex, sizeof(IndexHeader), 1, arquivoIndexBIN);

    // cria um array para armazenar os registros do índice conforme lê o arquivo de dados
    // ao final, o arquivo de índice será construído reutilizando a função de reescrita que coloca o índice da RAM para o disco
    IndexRegistro *registros = NULL;
    int totalRegsIndex = 0;
    Registro regDados;

    fseek(arquivoDadosBIN, TAM_CABECALHO, SEEK_SET);

    for (int i = 0; i < cabecalhoDados.proxRRN; i++) {
        LerRegistroBIN(arquivoDadosBIN, &regDados);

        if (regDados.removido != '1') {
            InserirRegistroIndex(&registros, regDados.codEstacao, i, &totalRegsIndex);
        }

        LiberarStringRegistro(&regDados);
    }

    ReescritaIndex(arquivoIndexBIN, registros, totalRegsIndex);

    free(registros);
    fclose(arquivoDadosBIN);
    fclose(arquivoIndexBIN);
}

void InsertInto(char *arquivoDados, char *arquivoIndex, int nroInsercoes){

    FILE *arquivoDadosBIN = fopen(arquivoDados, "rb+");
    if (arquivoDadosBIN == NULL) {
        mensagemErro();
        return;
    }

    FILE *arquivoIndexBIN = fopen(arquivoIndex, "rb+");
    if (arquivoIndexBIN == NULL) {
        mensagemErro();
        return;
    }

    Header cabecalho;
    LerCabecalhoBIN(arquivoDadosBIN, &cabecalho);
    if (cabecalho.status == '0') {
        MensagemErro();
        fclose(arquivoDadosBIN);
        fclose(arquivoIndexBIN);
        return;
    }

    // marcar como inconsistente
    IndexHeader cabecalhoIndex;
    cabecalhoIndex.status = '0';
    fwrite(&cabecalhoIndex, sizeof(IndexHeader), 1, arquivoIndexBIN);

    cabecalho.status = '0';
    EscreverCabecalhoBIN(arquivoDadosBIN, &cabecalho);

    // carregar index em RAM
    IndexRegistro *registrosIndex = NULL;
    int totalRegs = 0;
    CarregarIndex(arquivoIndexBIN, &registrosIndex, &totalRegs);

    for(int i = 0; i < nroInsercoes; i++){

        Registro novoRegistro;
        novo.removido = '0';
        novo.proximo = -1;

        scanf("%d", &novo.codEstacao);

        char bufferLeitura[100]; // buffer para armazenar as leituras feitas com ScanQuoteString

        ScanQuoteString(bufferLeitura);
        if (bufStr[0] == '\0'){
            novo.nomeEstacao = NULL;
            novo.tamNomeEstacao = 0;
        } else {
            novo.nomeEstacao = strdup(bufferLeitura);
            novo.tamNomeEstacao = strlen(bufferLeitura);
        }

        scanf("%d", &novo.codLinha);

        ScanQuoteString(bufferLeitura);
        if (bufStr[0] == '\0'){
            novo.nomeLinha = NULL;
            novo.tamNomeLinha = 0;
        } else {
            novo.nomeLinha = strdup(bufferLeitura);
            novo.tamNomeLinha = strlen(bufferLeitura);
        }

        // Campos numéricos que podem ser nulo
        scanf("%s", bufferLeitura);
        if (strcmp(bufferLeitura, "NULO") == 0) {
            novo.codProxEstacao = -1;
        } else {
            novo.codProxEstacao = atoi(bufferLeitura);
        }

        scanf("%s", bufferLeitura);
        if (strcmp(bufferLeitura, "NULO") == 0) {
            novo.distProxEstacao = -1;
        } else {
            novo.distProxEstacao = atoi(bufferLeitura);
        }

        scanf("%s", bufferLeitura);
        if (strcmp(bufferLeitura, "NULO") == 0) {
            novo.codLinhaIntegra = -1;
        } else {
            novo.codLinhaIntegra = atoi(bufferLeitura);
        }

        scanf("%s", bufferLeitura);
        if (strcmp(bufferLeitura, "NULO") == 0) {
            novo.codEstIntegra = -1;
        } else {
            novo.codEstIntegra = atoi(bufferLeitura);
        }

        int rrnNovoRegistro;
        if (cabecalho.topo != -1) {
            rrnNovoRegistro = cabecalho.topo;

            int byteoffset = TAM_CABECALHO + (rrnNovoRegistro * TAM_REGISTRO);
            fseek(arquivoDadosBIN, byteoffset, SEEK_SET);

            Registro removido;
            removido.nomeEstacao = NULL;
            removido.nomeLinha = NULL;
            LerRegistroBIN(arquivoDadosBIN, &removido);

            cabecalho.topo = removido.proximo; // atualiza o topo com o último removido da pilha de removidos
            LiberarStringRegistro(&removido);
        } else {
            rrnNovoRegistro = cabecalho.proxRRN;
            cabecalho.proxRRN++;
        }

        int byteoffset = TAM_CABECALHO + (rrnNovoRegistro * TAM_REGISTRO);
        fseek(arquivoDadosBIN, byteoffset, SEEK_SET);
        EscreverRegistroBIN(arquivoDadosBIN, &novo);

        InserirRegistroIndex(&registrosIndex, novo.codEstacao, rrnNovoRegistro, &totalRegs);
        LiberarStringRegistro(&novo);
    }

    ReescritaIndex(arquivoIndexBIN, registrosIndex, totalRegs);
    free(registrosIndex);

    // reescreve o cabecalho dos arquivos marcando como consistente ao final da inserção
    cabecalhoIndex.status = '1';
    fwrite(&cabecalhoIndex, sizeof(IndexHeader), 1, arquivoIndexBIN);
    cabecalho.status = '1';
    EscreverCabecalhoBIN(arquivoDadosBIN, &cabecalho);   
    
    fclose(arquivoDadosBIN);
    fclose(arquivoIndexBIN);

    BinarioNaTela(arquivoDados);
    BinarioNaTela(arquivoIndex); 
}

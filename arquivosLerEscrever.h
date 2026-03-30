#pragma once

#include "structs.h"


Header LerCabecalhoCSV(FILE *arquivoCSV, Header cabecalho);

Registro LerRegistroCSV(FILE *arquivoCSV, Registro registroDados);

void EscreverCabecalhoBIN(FILE* arquivo, const Header* cabecalho);

void EscreverRegistroBIN(FILE* arquivo, const Reg* registroDados);
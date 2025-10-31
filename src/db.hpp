#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>
#include <string>
#include <exception>

using namespace std;
// =========================================
// CONFIGURAÇÃO DO BANCO DE DADOS
// =========================================
soci::session &db();

// =========================================
/* FUNÇÕES AUXILIARES DE INTERFACE */
// =========================================
void logError(const exception &e, const string &where);

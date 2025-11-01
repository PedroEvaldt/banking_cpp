#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>
#include <string>
#include <exception>

// =========================================
// CONFIGURAÇÃO DO BANCO DE DADOS
// =========================================
soci::session &db();


#include "db.hpp"
#include <fstream>
#include <ctime>
#include <typeinfo>
using namespace std;

// =========================================
// CONFIGURAÇÃO DO BANCO DE DADOS
// =========================================
soci::session &db(){
    static soci::session sql;
    if(!sql.is_connected()){
        const char *host = getenv("PGHOST");
        const char *user = getenv("PGUSER");
        const char *pass = getenv("PGPASSWORD");
        const char *dbname = getenv("PGDATABASE");
        std::string conn = "dbname=" + std::string(dbname ? dbname : "banktdah") +
                           " user=" + std::string(user ? user : "postgres") +
                           " password=" + std::string(pass ? pass : "") +
                           " host=" + std::string(host ? host : "localhost");
        sql.open(soci::postgresql, conn);
        }
        return sql;
    }


// =========================================
/* FUNÇÕES AUXILIARES DE INTERFACE */
// =========================================

void logError(const exception &e, const string &where){
    ofstream f("errors.log", ios::app);
    if (!f.is_open()) return;
    time_t now = time(0);
    string t = ctime(&now);
    t.pop_back();

    f << "[" << t << "] " << where << " | " << typeid(e).name()
      << " | " << e.what() << "\n";
}

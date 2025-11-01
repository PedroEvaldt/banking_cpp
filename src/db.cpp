#include "db.hpp"
#include <fstream>
#include <ctime>
#include <typeinfo>
using namespace std;

// =========================================
// CONFIGURAÇÃO DO BANCO DE DADOS
// =========================================
soci::session &db(){
    static soci::session sql(soci::postgresql,
    "dbname=banktdah user=postgres password=971325 host=postgres-bank");
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

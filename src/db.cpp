#include "db.hpp"
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>
#include <iostream>
#include <sstream>
#include <cstring> // Para strlen
#include <cstdlib> // Para getenv

soci::session &db() {
    static soci::session sql;

    if (!sql.is_connected()) {
        const char *host = getenv("PGHOST");
        const char *user = getenv("PGUSER");
        const char *pass = getenv("PGPASSWORD");
        const char *dbname = getenv("PGDATABASE");

        // Define padrões simples que serão usados se as variáveis de ambiente
        // não estiverem definidas (ex: ao rodar localmente).
        const char *default_host = "localhost";
        const char *default_dbname = "banktdah";
        const char *default_user = "postgres";
        const char *default_pass = "971325";

        std::ostringstream conn;
        // Usa a variável de ambiente se ela existir e não estiver vazia,
        // caso contrário, usa o padrão.
        conn << "host=" << (host && strlen(host) ? host : default_host)
             << " dbname=" << (dbname && strlen(dbname) ? dbname : default_dbname)
             << " user=" << (user && strlen(user) ? user : default_user)
             << " password=" << (pass && strlen(pass) ? pass : default_pass)
             << " connect_timeout=3";

        std::cerr << "🔗 Tentando conectar com: " << conn.str() << std::endl;

        try {
            sql.open(soci::postgresql, conn.str());
            std::cerr << "✅ Conectado com sucesso!";
        } catch (const std::exception &e) {
            std::cerr << "❌ Erro ao conectar: " << e.what() << std::endl;
            throw;
        }
    }

    return sql;
}
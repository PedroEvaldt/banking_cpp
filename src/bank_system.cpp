#include "bank_system.hpp"
#include "db.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>
#include <fstream>
#include <ctime>
#include <string>
#include <stdexcept>
#include "logger.hpp"


using namespace std;
Logger logger("logger.txt");

Account& BankSystem::findAccount(int id) {
    auto it = accounts.find(id);
    if (it == accounts.end()) {
        throw ContaInexistente("ID " + std::to_string(id) + " não existe.");
    }
    return it->second;
}


// ===============================
// SALVAR e CARREGAR CONTAS
// ===============================
void BankSystem::loadAccounts() {
    try {
        accounts.clear();
        soci::rowset<soci::row> rs = (db().prepare <<
            "SELECT id, owner, amount, to_char(created_at, 'YYYY-MM-DD HH24:MI:SS') "
            "FROM accounts ORDER BY id ASC");
        for (auto const &r : rs){
            int id = r.get<int>(0);
            string owner = r.get<string>(1);
            int amount = r.get<int>(2);
            string created = r.get<string>(3);
            accounts.emplace(id, Account{id, owner, amount, created});
        }
        cout << C_GRN "Contas carregada: " << accounts.size() << C_RST"\n";
    } catch (const exception &e){
        (e, "loadAccounts/DB");
        cerr << C_RED "Falha ao carregar do banco: " << e.what() << C_RST "\n";
    }
}

// =========================================
// FUNÇÕES PRINCIPAIS
// =========================================
void BankSystem::createAccount() {
cout << C_CYN "Digite o nome do dono da conta: \033[0m";
string owner;
if (cin.peek() == '\n') cin.ignore();
getline(cin, owner);

int amount = ui::read_int(C_CYN "Digite o valor para criação da conta: \033[0m");

if (owner.empty()) throw ValorInvalido("Nome do proprietário está vazio.");
if (amount <= 0) throw ValorInvalido("Saldo insuficiente para inicializar a conta.");

try {
    soci::transaction tr(db()); // BEGIN
    int id = 0;
    string created;

    db() << "INSERT INTO accounts(owner, amount) VALUES(:o, :a) "
            "RETURNING id, to_char(created_at,'YYYY-MM-DD HH24:MI:SS')",
            soci::use(owner, "o"), soci::use(amount, "a"),
            soci::into(id), soci::into(created);

    db() << "INSERT INTO operations(account_id, kind, amount) "
            "VALUES(:id, 'create', :amt)",
            soci::use(id), soci::use(amount);

    tr.commit(); // COMMIT

    accounts.emplace(id, Account{id, owner, amount, created});

    string msg = "Conta criada para " + owner + " (id " + to_string(id) + ") com saldo R$" + to_string(amount);
    logger.info(msg);
    cout << "\033[1;32mConta criada! ID: " << id << "\033[0m\n";
} catch (const std::exception &e) {
    logger.error("createAccount/DB", e);
    throw; // cai nos seus catchs do menu
}
}

void BankSystem::deposit() {

    int id = ui::read_int(string(C_CYN) + "Digite o ID da conta: " + C_RST);
    int amount = ui::read_int(string(C_CYN) + "Digite o valor a depositar: " + C_RST);
    
    if (amount <= 0) throw ValorInvalido("Depósito deve ser > 0.");

    Account &acc = findAccount(id);
    
    try{
        soci::transaction tr(db());
        db() << "UPDATE accounts SET amount = amount + :a WHERE id = :i",
                soci::use(amount, "a"), soci::use(id, "i");
        db() << "INSERT INTO operations(account_id, kind, amount)"
                "VALUES(:i, 'deposit', :a)",
                soci::use(id, "i"), soci::use(amount, "a");
        tr.commit();

        acc.deposit(amount);
        string msg = "Depósito de R$" + to_string(amount) + " na conta " + to_string(id);
        logger.info(msg);
        cout << C_GRN "Depósito realizado!" << C_RST "\n";
    } catch (const exception &e){
        logger.error("deposit/DB", e);
        throw runtime_error("Falha ao processar depósito: " + string(e.what()));
    }

    
}

void BankSystem::withdraw() {

    int id = ui::read_int(string(C_CYN) + "Digite o ID da conta: " + C_RST);
    int amount = ui::read_int(string(C_CYN) + "Digite o valor a sacar: " + C_RST);

    if (amount <= 0) throw ValorInvalido("Saque deve ser > 0.");
    
    Account &acc = findAccount(id);
    
    if (!acc.withdraw(amount)){
        throw SaldoInsuficiente("Saldo insuficiente para sacar R$" + to_string(amount) + ".");
    }

    try {
        soci::transaction tr(db());
        // valida saldo no banco também (evita corrida)
        int updated = 0;
        db() << "UPDATE accounts SET amount = amount - :v "
                "WHERE id = :id AND amount >= :v",
            soci::use(amount), soci::use(id), soci::use(amount);
        // confirmar se de fato atualizou 1 linha:
        db() << "SELECT COUNT(*) FROM accounts WHERE id=:id AND amount >= 0",
            soci::use(id), soci::into(updated);

        if (updated == 0) {
            tr.rollback();
            // desfaz cache local:
            acc.deposit(amount);
            throw SaldoInsuficiente("Saldo insuficiente (conflito concorrente).");
        }

        db() << "INSERT INTO operations(account_id, kind, amount) "
                "VALUES(:id, 'withdraw', :v)",
            soci::use(id), soci::use(amount);
        tr.commit();

        string msg = "Saque de R$" + to_string(amount) + " da conta " + to_string(id);
        logger.info(msg);
        cout << "\033[1;32mSaque realizado!\033[0m\n";
        } catch (const std::exception &e) {
            logger.error("withdraw/DB", e);
            throw;
    }
}

void BankSystem::transfer() {
    int from = ui::read_int(string(C_CYN) + "Conta origem: " + C_RST);
    int to = ui::read_int(string(C_CYN) + "Conta destino: " + C_RST);
    int value = ui::read_int(string(C_CYN) + "Valor: " + C_RST);

    if (value <= 0) throw ValorInvalido("Transferência deve ser > 0.");

    Account &src = findAccount(from);
    Account &dst = findAccount(to);

    if (!src.withdraw(value)) {
        throw SaldoInsuficiente("Saldo insuficiente na conta " + to_string(from) + ".");
    }
    dst.deposit(value);
    try{
        soci::transaction tr(db());
        int updated = 0;
        db() << "UPDATE accounts SET amount = amount - :v "
            "WHERE id = :id AND amount >= :v",
            soci::use(value), soci::use(from), soci::use(value);
        db() << "SELECT COUNT(*) FROM accounts WHERE id=:id AND amount >= 0",
            soci::use(from), soci::into(updated);
        if (updated == 0) {
            tr.rollback();
            // reverte cache local
            src.deposit(value);
            dst.withdraw(value);
            throw SaldoInsuficiente("Saldo insuficiente (conflito concorrente).");
        }

        // credita destino
        db() << "UPDATE accounts SET amount = amount + :v WHERE id = :id",
            soci::use(value), soci::use(to);

        // registra histórico (duas pontas)
        db() << "INSERT INTO operations(account_id, kind, amount) "
                "VALUES(:id, 'transfer_out', :v)",
            soci::use(from), soci::use(value);
        db() << "INSERT INTO operations(account_id, kind, amount) "
                "VALUES(:id, 'transfer_in', :v)",
            soci::use(to), soci::use(value);

        tr.commit();

        string msg = "Transferência de R$" + to_string(value) +
                    " da conta " + to_string(from) + " para " + to_string(to);
        logger.info(msg);
        cout << "\033[1;32mTransferência realizada!\033[0m\n";
    } catch (const std::exception &e) {
        logger.error("transfer/DB", e);
        // reverte cache em caso de erro
        src.deposit(value);
        dst.withdraw(value);
        throw;
    }
}

void BankSystem::showBalance() {
    int id = ui::read_int(string(C_CYN) + "Digite o ID da conta: " + C_RST);
    auto it = accounts.find(id);
    if (it == accounts.end()){
        throw ContaInexistente("ID " + to_string(id) + " não existe.");
    }
    else {
        it->second.showInfo();
    }
}

vector<Operation> BankSystem::loadHistory(int idAccount){
    vector<Operation> out;
    try{
        soci::rowset<soci::row> rs=(db().prepare <<
            "SELECT kind, amount, to_char(created_at, 'YYYY-MM-DD HH24:MI:SS')"
            "FROM operations WHERE account_id = :id ORDER BY id ASC",
        soci::use(idAccount));
        for (auto &r : rs){
            Operation op;
            op.kind = r.get<string>(0);
            op.amount = r.get<int>(1);
            op.createdAt = r.get<string>(2);
            out.push_back(op);
        }
    }catch (const exception &e){
        logger.error("loadHistory/DB", e);
        throw;
    }
    return out;
}

void BankSystem::showHistory(){
    int id = ui::read_int(std::string(C_CYN) + "Digite o ID da conta: " + C_RST);
    auto &acc = findAccount(id);

    auto history = loadHistory(id);
    if(history.empty()){
        cout << C_YEL << "Nenhuma operação para esta conta." << C_RST << "\n";
        return;
    }
    cout << "\n" << C_CYN << "====== HISTÓRICO DA CONTA " << id << " ======" << C_RST << "\n";
    for (const auto &h : history){
        cout << " [" << h.createdAt << "] "
             << h.kind << " R$" << h.amount << "\n";
    }
}

// ===============================
// MENU PRINCIPAL
// ===============================
void BankSystem::run(){
    loadAccounts();
    int escolha;
    while(true){
        ui::clearScreen();
        cout << C_CYN << "\n=============================================" << C_RST << endl;
        ui::showTitle();
        cout << C_CYN << "\n=============================================" << C_RST << endl;
        cout << C_WHT << "Escolha uma opção:" << C_RST << "\n";
        cout << "  " << C_GRN << "[1]" << C_RST << " Criar uma conta\n";
        cout << "  " << C_GRN << "[2]" << C_RST << " Fazer um depósito\n";
        cout << "  " << C_GRN << "[3]" << C_RST << " Fazer um saque\n";
        cout << "  " << C_GRN << "[4]" << C_RST << " Realizar uma transferência\n";
        cout << "  " << C_GRN << "[5]" << C_RST << " Ver o saldo\n";
        cout << "  " << C_GRN << "[6]" << C_RST << " Histórico da\n";
        cout << "  " << C_RED << "[7]" << C_RST << " Sair\n\n";
        cout << "→ ";
        cin >> escolha;

        ui::clearScreen();

        try{

    switch (escolha) {
        case 1: createAccount(); break;
        case 2: deposit(); break;
        case 3: withdraw(); break;
        case 4: transfer(); break;
        case 5: showBalance(); break;
        case 6: showHistory(); break;
        case 7:
            ui::loadingAnimation();
            cout << "\033[1;36mVolte sempre ao Banco TDAH!\033[0m\n";
            return;
        default:
            cout << "\033[1;31mOpção inválida!\033[0m\n";
        }
    }
    catch (const ContaInexistente &e){
        logger.error("Menu/Operacao", e);
        cout << C_RED "Erro: " << e.what() << C_RST "\n";
    }
    catch (const ValorInvalido &e){
        logger.error("Menu/Operacao", e);
        cout << C_RED "Valor inválido: " << e.what() << C_RST "\n";
    }
    catch (const SaldoInsuficiente &e){
        logger.error("Menu/Operacao", e);
        cout << C_RED "Operação negada: " << e.what() << C_RST "\n";
    }
    catch (const exception &e){
        logger.error("Menu/Operacao", e);
        cout << C_RED "Falha inesperada: " << e.what() << C_RST "\n";
    }
        ui::pauseScreen();
    }
}

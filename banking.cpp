#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <limits>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <typeinfo>
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>

using namespace std;

// =========================================
// DEFINIÇÕES DE CORES (ANSI)
// =========================================
#define C_RED   "\033[1;31m"
#define C_GRN   "\033[1;32m"
#define C_YEL   "\033[1;33m"
#define C_CYN   "\033[1;36m"
#define C_WHT   "\033[1;37m"
#define C_RST   "\033[0m"

// =========================================
// CONFIGURAÇÃO DO BANCO DE DADOS
// =========================================
soci::session &db(){
    static soci::session sql(soci::postgresql,
    "dbname=banktdah user=postgres password=971325 host=localhost");
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

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\n" << C_WHT << "Pressione ENTER para continuar..." << C_RST;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void loadingAnimation() {
    cout << "\n" << C_YEL << "Carregando";
    for (int i = 0; i < 3; ++i) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(400));
    }
    cout << C_RST << "\n";
}

void showTitle() {
    string title = "BANCO TDAH CENTRAL";
    cout << setw(14) << "";
    for (char c : title) {
        cout << C_YEL << c << C_RST << flush;
        this_thread::sleep_for(chrono::milliseconds(80));
    }
}

int read_int(string mensagem) {
    int valor;
    while (true) {
        cout << mensagem;
        cin >> valor;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << C_RED << "Entrada inválida! Digite um número inteiro." << C_RST << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
    }
}

// Tratamento de erros
class ContaInexistente : public invalid_argument {
public:
    using invalid_argument::invalid_argument;
};

class SaldoInsuficiente : public runtime_error {
public:
    using runtime_error::runtime_error;
};

class ValorInvalido : public invalid_argument{
public:
    using invalid_argument::invalid_argument;
};

class Account{
private:
    int ID;
    string owner;
    int amount;
    string createdAt;

public:
    Account(int id, string owner, int amount, string createdAt)
    : ID(id), owner(owner), amount(amount), createdAt(std::move(createdAt)) {}


    int getId() const {return ID;}
    string getOwner() const {return owner;}
    int getAmount() const {return amount;}
    string getTime() const {return createdAt;}

    void deposit(int value) {amount += value; }
    bool withdraw (int value) {
        if(value <= amount){
            amount -= value;
            return true;
        }
        return false;
    }

    void showInfo(){
        cout << "\n" << C_CYN << "-------------------------------------------" << C_RST << "\n";
        cout << "Conta ID: " << C_YEL << ID << C_RST << "\n";
        cout << "Dono: " << C_YEL << owner << C_RST << "\n";
        cout << "Saldo: " << C_GRN << "R$" << amount << C_RST << "\n";
        cout << "Criada em: " << createdAt << endl;
        cout << C_CYN << "-------------------------------------------" << C_RST << "\n";
    }
};

class BankSystem{
private:
    vector<Account> accounts;
    vector<string> historico;

    // Função interna que registra o log do sistema
    void logOperation(const string &msg){
        ofstream log("log.txt", ios::app);
        if(!log.is_open()){
            cerr << C_RED << "Erro ao abrir o arquivo." << C_RST << endl;
        }
        else{
            time_t now = time(0);
            string time = ctime(&now);
            time.pop_back();
            log << "[" << time << "] " << msg << endl;
            log.close();
        }
    }

    Account& findAccount(int id){
        if (id <= 0 || id > static_cast<int>(accounts.size())){
            throw ContaInexistente("ID " + to_string(id) + " não existe.");
        }
        return accounts[id - 1];
    }

public:

    // ===============================
    // SALVAR e CARREGAR CONTAS
    // ===============================
    void loadAccounts() {
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
                accounts.emplace_back(id, owner, amount, created);
            }
            cout << C_GRN "Contas carregada: " << accounts.size() << C_RST"\n";
        } catch (const exception &e){
            logError(e, "loadAccounts/DB");
            cerr << C_RED "Falha ao carregar do banco: " << e.what() << C_RST "\n";
        }
    }

    // =========================================
    // FUNÇÕES PRINCIPAIS
    // =========================================
    void createAccount() {
        cout << C_CYN "Digite o nome do dono da conta: " << C_RST;
        string owner;
        if (cin.peek() == '\n') cin.ignore();
        getline(cin, owner);

        int amount = read_int(string(C_CYN) + "Digite o valor para criação da conta: " + C_RST);

        if (owner.empty()) throw ValorInvalido("Nome do proprietário está vazio.");
        if (amount <= 0) throw ValorInvalido("Saldo insuficiente para inicializar a conta.");

        try {
            soci::transaction tr(db());
            int id = 0;
            string created;
            
            db() << "INSERT INTO accounts(owner, amount) VALUES(:o, :a) "
                    "RETURNING id, to_char(created_at, 'YYYY-MM-DD HH24:MI:SS')",
                    soci::use(owner, "o"), soci::use(amount, "a"),
                    soci::into(id), soci::into(created);
        }
        
        string msg = "Conta criada para " + owner + " com saldo R$" + to_string(amount);
        historico.push_back(msg);
        logOperation(msg);
    }

    void deposit() {

        int id = read_int(string(C_CYN) + "Digite o ID da conta: " + C_RST);
        int amount = read_int(string(C_CYN) + "Digite o valor a depositar: " + C_RST);
        
        if (amount <= 0) throw ValorInvalido("Depósito deve ser > 0.");

        Account &acc = findAccount(id);
        acc.deposit(amount);

        string msg = "Depósito de R$" + to_string(amount) + " na conta " + to_string(id);
        historico.push_back(msg);
        logOperation(msg);
    }

    void withdraw() {

        int id = read_int(string(C_CYN) + "Digite o ID da conta: " + C_RST);
        int amount = read_int(string(C_CYN) + "Digite o valor a sacar: " + C_RST);

        if (amount <= 0) throw ValorInvalido("Saque deve ser > 0.");
        
        Account &acc = findAccount(id);
        
        if (acc.withdraw(amount)){
            string msg = "Saque de R$" + to_string(amount) + " da conta " + to_string(id);
            historico.push_back(msg);
            logOperation(msg);

            cout << C_GRN << "Saque realizado!" << C_RST << "\n";
        } else {
            throw SaldoInsuficiente("Saldo insuficiente para sacar R$" + to_string(amount) + ".");
        }
    }

    void transfer() {
        int from = read_int(string(C_CYN) + "Conta origem: " + C_RST);
        int to = read_int(string(C_CYN) + "Conta destino: " + C_RST);
        int value = read_int(string(C_CYN) + "Valor: " + C_RST);

        if (value <= 0) throw ValorInvalido("Transferência deve ser > 0.");

        Account &src = findAccount(from);
        Account &dst = findAccount(to);

        if (src.withdraw(value)) {
            dst.deposit(value);
            string msg = "Transferência de R$" + to_string(value) +
                         " da conta " + to_string(from) + " para " + to_string(to);
            historico.push_back(msg);
            logOperation(msg);
            cout << C_GRN << "Transferência realizada!" << C_RST << "\n";
        } else {
            throw SaldoInsuficiente("Saldo insuficiente na conta " + to_string(from) + ".");
        }
    }

    void showBalance() {
        int id = read_int(string(C_CYN) + "Digite o ID da conta: " + C_RST);
        if (id > 0 && id <= (int)accounts.size()) {
            accounts[id - 1].showInfo();
        } else {
            cout << "  " << C_RED << "ID inválido, digite um ID válido." << C_RST << "\n";
        }
    }

    void showHistory() {
        if (historico.empty()) {
            cout << C_YEL << "Nenhuma operação registrada ainda." << C_RST << "\n";
            return;
        }
        cout << "\n" << C_CYN << "====== HISTÓRICO DE OPERAÇÕES ======" << C_RST << "\n";
        for (size_t i = 0; i < historico.size(); ++i) {
            cout << C_WHT << "[" << i + 1 << "]" << C_RST << " " << historico[i] << endl;
        }
    }

    // ===============================
    // MENU PRINCIPAL
    // ===============================
    void run(){
        loadAccounts();
        int escolha;
        while(true){
            clearScreen();
            cout << C_CYN << "\n=============================================" << C_RST << endl;
            showTitle();
            cout << C_CYN << "\n=============================================" << C_RST << endl;
            cout << C_WHT << "Escolha uma opção:" << C_RST << "\n";
            cout << "  " << C_GRN << "[1]" << C_RST << " Criar uma conta\n";
            cout << "  " << C_GRN << "[2]" << C_RST << " Fazer um depósito\n";
            cout << "  " << C_GRN << "[3]" << C_RST << " Fazer um saque\n";
            cout << "  " << C_GRN << "[4]" << C_RST << " Realizar uma transferência\n";
            cout << "  " << C_GRN << "[5]" << C_RST << " Ver o saldo\n";
            cout << "  " << C_GRN << "[6]" << C_RST << " Histórico de Operações\n";
            cout << "  " << C_RED << "[7]" << C_RST << " Sair\n\n";
            cout << "→ ";
            cin >> escolha;

            clearScreen();

            try{

        switch (escolha) {
            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: transfer(); break;
            case 5: showBalance(); break;
            case 6: showHistory(); break;
            case 7:
                loadingAnimation();
                cout << "\033[1;36mVolte sempre ao Banco TDAH!\033[0m\n";
                return;
            default:
                cout << "\033[1;31mOpção inválida!\033[0m\n";
            }
        }
        catch (const ContaInexistente &e){
            logError(e, "Menu/Operacao");
            cout << C_RED "Erro: " << e.what() << C_RST "\n";
        }
        catch (const ValorInvalido &e){
            logError(e, "Menu/Operacao");
            cout << C_RED "Valor inválido: " << e.what() << C_RST "\n";
        }
        catch (const SaldoInsuficiente &e){
            logError(e, "Menu/Operacao");
            cout << C_RED "Operação negada: " << e.what() << C_RST "\n";
        }
        catch (const exception &e){
            logError(e, "Menu/Operacao");
            cout << C_RED "Falha inesperada: " << e.what() << C_RST "\n";
        }
            pauseScreen();
        }
    }

};


int main() {
    BankSystem banco;
    banco.run();
    return 0;
}
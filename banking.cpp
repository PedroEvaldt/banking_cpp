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

using namespace std;

// =========================================
// FUNÇÕES AUXILIARES DE INTERFACE
// =========================================

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\n\033[1;37mPressione ENTER para continuar...\033[0m";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void loadingAnimation() {
    cout << "\n\033[1;33mCarregando";
    for (int i = 0; i < 3; ++i) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(400));
    }
    cout << "\033[0m\n";
}

void showTitle() {
    string title = "BANCO TDAH CENTRAL";
    cout << setw(14) << "";
    for (char c : title) {
        cout << "\033[1;33m" << c << "\033[0m" << flush;
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
            cout << "\033[1;31mEntrada inválida! Digite um número inteiro.\033[0m" << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
    }
}

class Account{
private:
    static int nextID;
    int ID;
    string owner;
    int amount;
    string createdAt;

public:
    Account(string owner, int amount)
    : ID(nextID++), owner(owner), amount(amount)
    {
        time_t now = time(0);
        string t = ctime(&now);
        t.pop_back();
        createdAt = t;
    }


    int getId() {return ID;}
    string getOwner() {return owner;}
    int getAmount() {return amount;}
    string getTime() {return createdAt;}

    void deposit(int value) {amount += value; }
    bool withdraw (int value) {
        if(value <= amount){
            amount -= value;
            return true;
        }
        return false;
    }

    void showInfo(){
        cout << "\n\033[1;36m-------------------------------------------\033[0m\n";
        cout << "Conta ID: \033[1;33m" << ID << "\033[0m\n";
        cout << "Dono: \033[1;33m" << owner << "\033[0m\n";
        cout << "Saldo: \033[1;32mR$" << amount << "\033[0m\n";
        cout << "Criada em: " << createdAt << endl;
        cout << "\033[1;36m-------------------------------------------\033[0m\n";
    }
};
int Account::nextID = 1;

class BankSystem{
private:
    vector<Account> accounts;
    vector<string> historico;

    // Função interna que registra o log do sistema
    void logOperation(const string &msg){
        ofstream log("log.txt", ios::app);
        if(!log.is_open()){
            cerr << "\033[1;31mErro ao abrir o arquivo.\033[0m" << endl;
        }
        else{
            time_t now = time(0);
            string time = ctime(&now);
            time.pop_back();
            log << "[" << time << "] " << msg << endl;
            log.close();
        }
    }

public:

    // ===============================
    // SALVAR e CARREGAR CONTAS
    // ===============================
    void loadAccounts() {
        ifstream arquivo("account.txt"); // mesmo nome usado em create_account()
        if (!arquivo.is_open()) {
            cerr << "\033[1;31mErro ao abrir o arquivo.\033[0m" << endl;
            return;
        }

        string linha;
        while (getline(arquivo, linha)) {
            if (linha.empty()) continue;

            size_t created_pos = linha.find("Created at: ");
            size_t id_pos = linha.find("ID: ");
            size_t owner_pos = linha.find("Owner: ");
            size_t amount_pos = linha.find("amount: "); // minúsculo igual ao create_account()

            if (id_pos == string::npos || owner_pos == string::npos || amount_pos == string::npos)
                continue;

            // ===== Extrair Owner =====
            string owner_str = linha.substr(owner_pos + 7, amount_pos - (owner_pos + 7));
            // remove espaços e vírgulas finais 
            owner_str.erase(owner_str.find_last_not_of(" ,") + 1);

            // ===== Extrair Amount =====
            string amount_str = linha.substr(amount_pos + 8);
            amount_str.erase(remove_if(amount_str.begin(), amount_str.end(), ::isspace), amount_str.end());
            int amount = stoi(amount_str);

            // ===== Criar conta e inserir =====
            accounts.emplace_back(owner_str, amount);
        }
        cout << "\033[1;32mContas carregadas: " << accounts.size() << "\033[0m\n";
        arquivo.close();
    }

    void saveAccounts(){
        ofstream arquivo("account.txt", ios::trunc);
        if(!arquivo.is_open()){
            cerr << "\033[1;31mErro ao abrir o arquivo.\033[0m" << endl;
        }
        for (auto &acc : accounts){
            arquivo << "Created at: " << acc.getTime()
                    << "  ID: " << acc.getId()
                    << ", Owner: " << acc.getOwner() 
                    << ", amount: " << acc.getAmount() << "\n";
            if(arquivo.fail()){
                cerr << "\033[1;31mErro ao carregar as conta no arquivo.\033[0m" << endl;
            }
        }
        arquivo.close();
    }
    

    // =========================================
    // FUNÇÕES PRINCIPAIS
    // =========================================
    void createAccount() {
        cout << "\033[1;34mDigite o nome do dono da conta: \033[0m";
        string owner;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, owner);

        int amount = read_int("\033[1;34mDigite o valor para criação da conta: \033[0m");

        accounts.emplace_back(owner, amount);
        
        string msg = "Conta criada para " + owner + " com saldo R$" + to_string(amount);
        historico.push_back(msg);
        logOperation(msg);
    }

    void deposit() {

        int id = read_int("\033[1;34mDigite o ID da conta: \033[0m");
        int amount = read_int("\033[1;34mDigite o valor a depositar: \033[0m");
        
        if (id <= 0 || id > (int)accounts.size()){
            cout << "\033[1;31mID inválido.\033[0m\n";
            return;
        }

        accounts[id - 1].deposit(amount);

        string msg = "Depósito de R$" + to_string(amount) + " na conta " + to_string(id);
        historico.push_back(msg);
        logOperation(msg);
    }

    void withdraw() {

        int id = read_int("\033[1;34mDigite o ID da conta: \033[0m");
        int amount = read_int("\033[1;34mDigite o valor a sacar: \033[0m");

        if (id <= 0 || id > (int)accounts.size()){
            cout << "\033[1;31mID inválido.\033[0m\n";
            return;
        }

        if (accounts[id - 1].withdraw(amount)){

            string msg = "Saque de R$" + to_string(amount) + " da conta " + to_string(id);
            historico.push_back(msg);
            logOperation(msg);

            cout << "\033[1;32mSaque realizado!\033[0m\n";
        } else {
            cout << "\033[1;31mSaldo insuficiente!\033[0m\n";
        }
    }

        void transfer() {
        int from = read_int("\033[1;34mConta origem: \033[0m");
        int to = read_int("\033[1;34mConta destino: \033[0m");
        int value = read_int("\033[1;34mValor: \033[0m");

        if (from <= 0 || from > (int)accounts.size() || to <= 0 || to > (int)accounts.size()) {
            cout << "\033[1;31mIDs inválidos.\033[0m\n";
            return;
        }

        if (accounts[from - 1].withdraw(value)) {
            accounts[to - 1].deposit(value);
            string msg = "Transferência de R$" + to_string(value) +
                         " da conta " + to_string(from) + " para " + to_string(to);
            historico.push_back(msg);
            logOperation(msg);
            cout << "\033[1;32mTransferência realizada!\033[0m\n";
        } else {
            cout << "\033[1;31mSaldo insuficiente!\033[0m\n";
        }
    }

    void showBalance() {
        int id = read_int("\033[1;34mDigite o ID da conta: \033[0m");
        if (id > 0 && id <= accounts.size()) {
            accounts[id - 1].showInfo();
        } else {
            cout << "  \033[1;31mID inválido, digite um ID válido.\033[0m\n";
        }
    }

    void showHistory() {
        if (historico.empty()) {
            cout << "\033[1;33mNenhuma operação registrada ainda.\033[0m\n";
            return;
        }
        cout << "\n\033[1;36m====== HISTÓRICO DE OPERAÇÕES ======\033[0m\n";
        for (size_t i = 0; i < historico.size(); ++i) {
            cout << "\033[1;37m[" << i + 1 << "]\033[0m " << historico[i] << endl;
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
        cout << "\033[1;36m\n=============================================\033[0m" << endl;
        showTitle();
        cout << "\033[1;36m\n=============================================\033[0m" << endl;
        cout << "\033[1;37mEscolha uma opção:\033[0m\n";
        cout << "  \033[1;32m[1]\033[0m Criar uma conta\n";
        cout << "  \033[1;32m[2]\033[0m Fazer um depósito\n";
        cout << "  \033[1;32m[3]\033[0m Fazer um saque\n";
        cout << "  \033[1;32m[4]\033[0m Realizar uma transferência\n";
        cout << "  \033[1;32m[5]\033[0m Ver o saldo\n";
        cout << "  \033[1;32m[6]\033[0m Histórico de Operações\n";
        cout << "  \033[1;31m[7]\033[0m Sair\n\n";
        cout << "→ ";
        cin >> escolha;

        clearScreen();
        int id, amount, from, to;
        string owner, msg_id, msg_amnt, msg_from, msg_to, msg;

        switch (escolha) {
            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: transfer(); break;
            case 5: showBalance(); break;
            case 6: showHistory(); break;
            case 7:
                cout << "\n\033[1;32mSalvando dados...\033[0m\n";
                saveAccounts();
                loadingAnimation();
                cout << "\033[1;36mVolte sempre ao Banco TDAH!\033[0m\n";
                return;
            default:
                cout << "\033[1;31mOpção inválida!\033[0m\n";
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
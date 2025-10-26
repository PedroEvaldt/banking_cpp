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

typedef struct {
    int ID;
    string owner;
    int amount;
    string time;
} Baccount;

// Protótipos
void showBalance(int id, vector<Baccount> &accounts);
void withdraw(int id, int amount, vector<Baccount> &accounts);
void deposit(int id, int amount, vector<Baccount> &accounts);
void transfer(int from, int id, int amount, vector<Baccount> &accounts);
void create_account(string owner, int amount, vector<Baccount> &accounts);
void mostrar_historico(vector<string> &historico);
void load_accounts(vector<Baccount> &accounts);
void reload_accounts(vector<Baccount> &accounts);
void clearScreen();
void pauseScreen();
void loadingAnimation();
void showTitle();
int read_int(string mensagem);
void logOperation(string msg);

int ID = 1;

// =========================================
// FUNÇÃO PRINCIPAL
// =========================================
int main() {
    int escolha;
    vector<Baccount> accounts;
    vector<string> historico;
    load_accounts(accounts);

    while (true) {
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
        case 1: {
            msg_amnt = "\033[1;34mDigite o valor para criação da conta: \033[0m";
            amount = read_int(msg_amnt);
            cout << "\033[1;34mDigite o nome do dono da conta: \033[0m";
            getline(cin, owner);
            create_account(owner, amount, accounts);
            msg = "Conta de ID " + to_string(ID) + " criada com sucesso";
            logOperation(msg);
            historico.push_back(msg);
            ID++;
            pauseScreen();
            break;
        }
        case 2: {
            msg_id = "\033[1;34mDigite o ID da conta: \033[0m";
            id = read_int(msg_id);
            msg_amnt = "\033[1;34mDigite o valor a depositar: \033[0m";
            amount = read_int(msg_amnt);
            deposit(id, amount, accounts);
            msg = "Depósito de R$" + to_string(amount) + " na conta " + to_string(id);
            logOperation(msg);
            historico.push_back(msg);
            pauseScreen();
            break;
        }
        case 3: {
            msg_id = "\033[1;34mDigite o ID da conta: \033[0m";
            id = read_int(msg_id);
            msg_amnt = "\033[1;34mDigite o valor a sacar: \033[0m";
            amount = read_int(msg_amnt);
            withdraw(id, amount, accounts);
            msg = "Saque de R$" + to_string(amount) + " da conta " + to_string(id);
            logOperation(msg);
            historico.push_back(msg);
            pauseScreen();
            break;
        }
        case 4: {
            msg_from = "\033[1;34mDigite o ID da conta que vai realizar o pagamento: \033[0m";
            from = read_int(msg_from);
            msg_amnt = "\033[1;34mDigite o valor da transferência: \033[0m";
            amount = read_int(msg_amnt);
            msg_to = "\033[1;34mDigite o ID da conta que vai receber o pagamento: \033[0m";
            to = read_int(msg_to);
            transfer(from, to, amount, accounts);
            msg = "Transferência de R$" + to_string(amount) + " da conta " + to_string(from) + " para a conta " + to_string(to);
            logOperation(msg);
            historico.push_back(msg);
            pauseScreen();
            break;
        }
        case 5: {
            msg_id = "\033[1;34mDigite o ID da conta: \033[0m";
            id = read_int(msg_id);
            showBalance(id, accounts);
            pauseScreen();
            break;
        }
        case 6: {
            mostrar_historico(historico);
            pauseScreen();
            break;
        }
        case 7:
            cout << "\n\033[1;32mSaindo do sistema...\033[0m\n";
            loadingAnimation();
            cout << "\033[1;36mVolte sempre ao Banco TDAH!\033[0m\n";
            reload_accounts(accounts);
            return 0;
        default:
            cout << "\033[1;31mOpção inválida! Tente novamente.\033[0m\n";
            pauseScreen();
        }
    }
    return 0;
}

// =========================================
// FUNÇÕES AUXILIARES
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

// =========================================
// FUNÇÕES PRINCIPAIS DO SISTEMA
// =========================================
void create_account(string owner, int amount, vector<Baccount> &accounts) {
    time_t now = time(0);
    string time = ctime(&now);
    time.pop_back();
    Baccount new_acc = {ID, owner, amount, time};
    accounts.push_back(new_acc);
    ofstream arquivo("account.txt", ios::app);
    if(!(arquivo.is_open())){
        cerr << "\033[1;31mErro ao abrir o arquivo.\033[0m" << endl;
    }
    else{
        arquivo << "Created at: " << time << "  ID: " << ID << ", Owner: " << owner << ", amount: " << amount << "\n";
        if(arquivo.fail()){
            cerr << "\033[1;31mErro ao criar a conta.\033[0m" << endl;
        }
        else{
            cout << "\033[1;32mConta criada com sucesso!\033[0m\n";
            cout << "ID da conta: \033[1;33m" << ID << "\033[0m\n";
            cout << "Saldo inicial: \033[1;32mR$" << amount << "\033[0m\n";
        }
    }
    
}

void deposit(int id, int amount, vector<Baccount> &accounts) {
    if (id > 0 && id <= accounts.size()) {
        accounts[id - 1].amount += amount;
        cout << "\033[1;32m+" << amount << " adicionados à conta ID " << id << "\033[0m\n";
    } else {
        cout << "  \033[1;31mID inválido, digite um ID válido.\033[0m\n";
    }
}

void withdraw(int id, int amount, vector<Baccount> &accounts) {
    if (id > 0 && id <= accounts.size()) {
        if (accounts[id - 1].amount >= amount) {
            accounts[id - 1].amount -= amount;
            cout << "\033[1;32m-" << amount << " retirados com sucesso!\033[0m\n";
        } else {
            cout << "\033[1;31mSaldo insuficiente!\033[0m Reveja o valor no balanço.\n";
        }
    } else {
        cout << "  \033[1;31mID inválido, digite um ID válido.\033[0m\n";
    }
}

void transfer(int from, int to, int amount, vector<Baccount> &accounts) {
    if (from > 0 && from <= accounts.size() && to > 0 && to <= accounts.size()) {
        if (accounts[from - 1].amount >= amount) {
            accounts[from - 1].amount -= amount;
            accounts[to - 1].amount += amount;
            cout << "\033[1;32mTransferência de R$" << amount
                 << " da conta " << from
                 << " para a conta " << to << " realizada com sucesso!\033[0m\n";
        } else {
            cout << "\033[1;31mSaldo insuficiente para transferência.\033[0m\n";
        }
    } else {
        cout << "  \033[1;31mID inválido, digite IDs válidos.\033[0m\n";
    }
}

void showBalance(int id, vector<Baccount> &accounts) {
    if (id > 0 && id <= accounts.size()) {
        cout << "\n\033[1;36m-------------------------------------------\033[0m\n";
        cout << "Conta ID: \033[1;33m" << id << "\033[0m\n";
        cout << "Dono da conta: \033[1;33m" << accounts[id - 1].owner << "\033[0m\n";
        cout << "Saldo atual: \033[1;32mR$" << accounts[id - 1].amount << "\033[0m\n";
        cout << "\033[1;36m-------------------------------------------\033[0m\n";
    } else {
        cout << "  \033[1;31mID inválido, digite um ID válido.\033[0m\n";
    }
}

void mostrar_historico(vector<string> &historico) {
    if (historico.empty()) {
        cout << "\033[1;33mNenhuma operação registrada ainda.\033[0m\n";
        return;
    }
    cout << "\n\033[1;36m====== HISTÓRICO DE OPERAÇÕES ======\033[0m\n";
    for (size_t i = 0; i < historico.size(); ++i) {
        cout << "\033[1;37m[" << i + 1 << "]\033[0m " << historico[i] << endl;
    }
}

void logOperation(string msg){
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

void load_accounts(vector<Baccount> &accounts) {
    ifstream arquivo("account.txt"); // mesmo nome usado em create_account()
    if (!arquivo.is_open()) {
        cerr << "\033[1;31mErro ao abrir o arquivo.\033[0m" << endl;
        return;
    }

    string linha;
    int maxID = 0; // para atualizar o ID global depois

    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;

        size_t created_pos = linha.find("Created at: ");
        size_t id_pos = linha.find("ID: ");
        size_t owner_pos = linha.find("Owner: ");
        size_t amount_pos = linha.find("amount: "); // minúsculo igual ao create_account()

        if (id_pos == string::npos || owner_pos == string::npos || amount_pos == string::npos)
            continue;

        // ===== Extrair Time =====
        string time_str = linha.substr(created_pos + 12, id_pos - (created_pos + 12));
        time_str.erase(time_str.find_last_not_of(" ,") + 1);
        
        // ===== Extrair ID =====
        string id_str = linha.substr(id_pos + 4, owner_pos - (id_pos + 4));
        id_str.erase(id_str.find_last_not_of(" ,") + 1);
        int id = stoi(id_str);

        // ===== Extrair Owner =====
        string owner_str = linha.substr(owner_pos + 7, amount_pos - (owner_pos + 7));
        // remove espaços e vírgulas finais 
        owner_str.erase(owner_str.find_last_not_of(" ,") + 1);

        // ===== Extrair Amount =====
        string amount_str = linha.substr(amount_pos + 8);
        amount_str.erase(remove_if(amount_str.begin(), amount_str.end(), ::isspace), amount_str.end());
        int amount = stoi(amount_str);

        // ===== Criar conta e inserir =====
        Baccount acc = {id, owner_str, amount, time_str};
        accounts.push_back(acc);
        if (id > maxID) maxID = id;
    }

    arquivo.close();

    // Atualiza o ID global para continuar a partir do último
    ID = maxID + 1;

    cout << "\033[1;32mContas carregadas com sucesso (" 
         << accounts.size() << " contas). Próximo ID: " << ID << "\033[0m\n";
}

void reload_accounts(vector<Baccount> &accounts){
    ofstream arquivo("account.txt", ios::trunc);
    if(!arquivo.is_open()){
        cerr << "\033[1;31mErro ao abrir o arquivo.\033[0m" << endl;
    }
    for (const auto &acc : accounts){
        arquivo << "Created at: " << acc.time 
                << "  ID: " << acc.ID 
                << ", Owner: " << acc.owner 
                << ", amount: " << acc.amount << "\n";
        if(arquivo.fail()){
            cerr << "\033[1;31mErro ao carregar as conta no arquivo.\033[0m" << endl;
        }
    }
    arquivo.close();
}
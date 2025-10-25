#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <limits>
#include <ctime>

using namespace std;

typedef struct {
    int ID;
    string owner;
    int amount;
} Baccount;

// Protótipos
void showBalance(int id, vector<Baccount> &accounts);
void withdraw(int id, int amount, vector<Baccount> &accounts);
void deposit(int id, int amount, vector<Baccount> &accounts);
void transfer(int from, int id, int amount, vector<Baccount> &accounts);
void create_account(string owner, int amount, vector<Baccount> &accounts);
void mostrar_historico(vector<string> &historico);
void clearScreen();
void pauseScreen();
void loadingAnimation();
void showTitle();
int read_int(string mensagem);

int ID = 1;

// =========================================
// FUNÇÃO PRINCIPAL
// =========================================
int main() {
    int escolha;
    vector<Baccount> accounts;
    vector<string> historico;

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
        string owner, msg_id, msg_amnt, msg_from, msg_to;

        switch (escolha) {
        case 1: {
            msg_amnt = "\033[1;34mDigite o valor para criação da conta: \033[0m";
            amount = read_int(msg_amnt);
            cout << "\033[1;34mDigite o nome do dono da conta: \033[0m";
            getline(cin, owner);
            create_account(owner, amount, accounts);
            historico.push_back("Conta de ID " + to_string(ID) + " criada com sucesso");
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
            historico.push_back("Depósito de R$" + to_string(amount) + " na conta " + to_string(id));
            pauseScreen();
            break;
        }
        case 3: {
            msg_id = "\033[1;34mDigite o ID da conta: \033[0m";
            id = read_int(msg_id);
            msg_amnt = "\033[1;34mDigite o valor a sacar: \033[0m";
            amount = read_int(msg_amnt);
            withdraw(id, amount, accounts);
            historico.push_back("Saque de R$" + to_string(amount) + " da conta " + to_string(id));
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
            historico.push_back("Transferência de R$" + to_string(amount) + " da conta " + to_string(from) + " para a conta " + to_string(to));
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
    Baccount new_acc = {ID, owner, amount};
    accounts.push_back(new_acc);
    ofstream arquivo("account.txt", ios::app);
    if(!(arquivo.is_open())){
        cerr << "\033[1;31mErro ao abrir o arquivo.\033[0m" << endl;
    }
    else{
        time_t now = time(0);
        arquivo << "Created at: " << ctime(&now) << "ID: " << ID << ", Owner: " << owner << ", amount: " << amount << "\n";
        cout << "\033[1;32mConta criada com sucesso!\033[0m\n";
        cout << "ID da conta: \033[1;33m" << ID << "\033[0m\n";
        cout << "Saldo inicial: \033[1;32mR$" << amount << "\033[0m\n";
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
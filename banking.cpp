#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <thread>
#include <chrono>
using namespace std;

typedef struct{
    int ID;
    string owner;
    int amount;
}Baccount;

void showBalance(int id, vector<Baccount> &accounts);
void withdraw(int id, int amount, vector<Baccount> &accounts);
void deposit(int id, int amount, vector<Baccount> &accounts);
void transfer(int from, int id, int amount, vector<Baccount> &accounts);
void create_account(string owner, int amount, vector<Baccount> &accounts);
void clearScreen();
void pauseScreen();
void loadingAnimation();

int ID = 1;

// =========================================
// FUNÇÃO PRINCIPAL
// =========================================
int main() {
    int escolha;
    vector<Baccount> accounts;

    while (true) {
        clearScreen();

        cout << "\033[1;36m=============================================\033[0m\n";
        cout << setw(44)<< "\033[1;33mBANCO TDAH CENTRAL\033[0m\n";
        cout << "\033[1;36m=============================================\033[0m\n\n";

        cout << "\033[1;37mEscolha uma opção:\033[0m\n";
        cout << "  \033[1;32m[1]\033[0m Criar uma conta\n";
        cout << "  \033[1;32m[2]\033[0m Fazer um depósito\n";
        cout << "  \033[1;32m[3]\033[0m Fazer um saque\n";
        cout << "  \033[1;32m[4]\033[0m Realizar uma transferencia\n";
        cout << "  \033[1;32m[5]\033[0m Ver o saldo\n";
        cout << "  \033[1;31m[6]\033[0m Sair\n\n";
        cout << "→ ";
        cin >> escolha;

        clearScreen();
        
        int id, amount, from, to;
        string owner;
        switch (escolha) {
        case 1: {
            cout << "\033[1;34mDigite o valor para criação da conta: \033[0m";
            cin >> amount;
            cout << "\033[1;34mDigite o nome do dono da conta: \033[0m";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, owner);
            create_account(owner, amount, accounts);
            ID++;
            pauseScreen();
            break;
        }
        case 2: {
            cout << "\033[1;34mDigite o ID da conta: \033[0m";
            cin >> id;
            cout << "\033[1;34mDigite o valor a depositar: \033[0m";
            deposit(id, amount, accounts);
            pauseScreen();
            break;
        }
        case 3: {
            cout << "\033[1;34mDigite o ID da conta: \033[0m";
            cin >> id;
            cout << "\033[1;34mDigite o valor a sacar: \033[0m";
            cin >> amount;
            withdraw(id, amount, accounts);
            pauseScreen();
            break;
        }
        case 4:{
            cout << "\033[1;34mDigite o ID da conta que vai realizar o pagamento: \033[0m";
            cin >> from;
            cout << "\033[1;34mDigite o valor da transferencia: \033[0m";
            cin >> amount;
            cout << "\033[1;34mDigite o ID da conta que vai receber o pagamento: \033[0m";
            cin >> to;
            transfer(from, to, amount, accounts);
            pauseScreen();
            break;
        }
        case 5: {
            cout << "\033[1;34mDigite o ID da conta: \033[0m";
            cin >> id;
            showBalance(id, accounts);
            pauseScreen();
            break;
        }
        case 6:
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
    cin.ignore();
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

// =========================================
// FUNÇÕES PRINCIPAIS DO SISTEMA
// =========================================
void create_account(string owner, int amount, vector<Baccount> &accounts) {
    Baccount new_acc = {ID, owner, amount};
    accounts.push_back(new_acc);
    cout << "\033[1;32mConta criada com sucesso!\033[0m\n";
    cout << "ID da conta: \033[1;33m" << ID << "\033[0m\n";
    cout << "Saldo inicial: \033[1;32mR$" << amount << "\033[0m\n";
}

void deposit(int id, int amount, vector<Baccount> &accounts) {
    if(id > 0 && id <= ID){
    accounts[id - 1].amount += amount;
    cout << "\033[1;32m+" << amount << " adicionados à conta ID " << id << "\033[0m\n";}
    else{
        cout << "  \033[1;31mId inválido, digite um id válido.\033[0m\n";
    }
}

void withdraw(int id, int amount, vector<Baccount> &accounts) {
    if (id > 0 && id <= ID){
        if (accounts[id - 1].amount >= amount) {
            accounts[id - 1].amount -= amount;
            cout << "\033[1;32m-" << amount << " retirados com sucesso!\033[0m\n";
        } else {
            cout << "\033[1;31mSaldo insuficiente!\033[0m Reveja o valor no balanço.\n";
        }}
    else{
        cout << "  \033[1;31mId inválido, digite um id válido.\033[0m\n";
    }
}

void transfer(int from, int to, int amount, vector<Baccount> &accounts){
    if (from > 0 && from <= ID && to > 0 && to <= ID){
    if (accounts[from - 1].amount >= amount){
        accounts[from - 1].amount -= amount;
        accounts[to - 1].amount += amount;
        cout << "MENSAGEM DE SUCESSO DE TRANSFERENCIA";
    }
    else{
        cout << "MENSAGEM DE FALHA DE TRANSFERENCIA";
    }}
    else{
        cout << "  \033[1;31mId inválido, digite um id válido.\033[0m\n";
    }
}

void showBalance(int id, vector<Baccount> &accounts) {
    if (id > 0 && id <= ID){
    cout << "\n\033[1;36m-------------------------------------------\033[0m\n";
    cout << "Conta ID: \033[1;33m" << id << "\033[0m\n";
    cout << "Dono da conta: \033[1;33m" << accounts[id - 1].owner << "\033[0m\n";
    cout << "Saldo atual: \033[1;32mR$" << accounts[id - 1].amount << "\033[0m\n";
    cout << "\033[1;36m-------------------------------------------\033[0m\n";}
    else{
        cout << "  \033[1;31mId inválido, digite um id válido.\033[0m\n";
    }
}

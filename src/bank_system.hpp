#include "account.hpp"
#include "ui_utils.hpp"
#include <vector>
#include <string>

using namespace std;
class BankSystem{
private:
    vector<Account> accounts;
    vector<string> historico;

    // Função interna que registra o log do sistema
    void logOperation(const string &msg);
    Account& findAccount(int id);

public:

    // ===============================
    // SALVAR e CARREGAR CONTAS
    // ===============================
    void loadAccounts();

    // =========================================
    // FUNÇÕES PRINCIPAIS
    // =========================================
    void createAccount();
    void deposit();
    void withdraw();
    void transfer();
    void showBalance();
    void showHistory();
    // ===============================
    // MENU PRINCIPAL
    // ===============================
    void run();
};
#include "account.hpp"
#include "ui_utils.hpp"
#include "operations.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class BankSystem{
private:
    std::unordered_map<int, Account> accounts;
    std::vector<Operation> loadHistory(int accountId);
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
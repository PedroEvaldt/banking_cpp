#include "account.hpp"
#include "ui_utils.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

Account::Account(int id, string owner, int amount, string createdAt)
    : ID(id), owner(owner), amount(amount), createdAt(std::move(createdAt)) {}


int Account::getId() const {return ID;}
string Account::getOwner() const {return owner;}
int Account::getAmount() const {return amount;}
string Account::getTime() const {return createdAt;}

void Account::deposit(int value) {amount += value; }

bool Account::withdraw(int value) {
        if(value <= amount){
            amount -= value;
            return true;
        }
        return false;
    }

void Account::showInfo(){
    cout << "\n" << C_CYN << "-------------------------------------------" << C_RST << "\n";
    cout << "Conta ID: " << C_YEL << ID << C_RST << "\n";
    cout << "Dono: " << C_YEL << owner << C_RST << "\n";
    cout << "Saldo: " << C_GRN << "R$" << amount << C_RST << "\n";
    cout << "Criada em: " << createdAt << endl;
    cout << C_CYN << "-------------------------------------------" << C_RST << "\n";
}
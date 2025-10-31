#pragma once
#include <string>

using namespace std;
class Account{
private:
    int ID;
    string owner;
    int amount;
    string createdAt;

public:
    Account(int id, string owner, int amount, string createdAt);
    int getId() const;
    string getOwner() const;
    int getAmount() const;
    string getTime() const;
    void deposit(int value);
    bool withdraw(int value);
    void showInfo();
};
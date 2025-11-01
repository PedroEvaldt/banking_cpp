#pragma once
#include <string>

class Account{
private:
    int ID;
    std::string owner;
    int amount;
    std::string createdAt;

public:
    Account(int id, std::string owner, int amount, std::string createdAt);
    int getId() const;
    std::string getOwner() const;
    int getAmount() const;
    std::string getTime() const;
    void deposit(int value);
    bool withdraw(int value);
    void showInfo();
};
#include <stdexcept>


// Tratamento de erros
class ContaInexistente : public std::invalid_argument {
public:
    using std::invalid_argument::invalid_argument;
};

class SaldoInsuficiente : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class ValorInvalido : public std::invalid_argument{
public:
    using std::invalid_argument::invalid_argument;
};


#include <stdexcept>
using namespace std;

// Tratamento de erros
class ContaInexistente : public invalid_argument {
public:
    using invalid_argument::invalid_argument;
};

class SaldoInsuficiente : public runtime_error {
public:
    using runtime_error::runtime_error;
};

class ValorInvalido : public invalid_argument{
public:
    using invalid_argument::invalid_argument;
};


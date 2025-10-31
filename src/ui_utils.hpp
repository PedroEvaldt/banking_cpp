#include <string>
using namespace std;
// =========================================
// DEFINIÇÕES DE CORES (ANSI)
// =========================================
#define C_RED   "\033[1;31m"
#define C_GRN   "\033[1;32m"
#define C_YEL   "\033[1;33m"
#define C_CYN   "\033[1;36m"
#define C_WHT   "\033[1;37m"
#define C_RST   "\033[0m"

void clearScreen();
void pauseScreen();
void loadingAnimation();
void showTitle();
int read_int(string mensagem);



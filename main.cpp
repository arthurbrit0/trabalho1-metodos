#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>   // Para escrever em arquivo
#include <locale>
#include <sstream>   // Para transformar double em string customizada

using namespace std;

//------------------------------------------------------------------------------
// Converte número double (com ponto) para string com vírgula no decimal
// Exemplo: 1.105164 -> "1,105164"
string doubleToBrazilString(double valor, int precision = 6) {
    ostringstream oss;
    oss << fixed << setprecision(precision) << valor; // gera algo como "1.105164"
    string s = oss.str();
    // Substituir '.' por ',' na string
    for (char &c : s) {
        if (c == '.') c = ',';
    }
    return s;
}

//------------------------------------------------------------------------------
// Função original: f(d) = a*d - d*ln(d)
double f(double d, double a) {
    // Garantir que d > 0, pois há ln(d).
    return a * d - d * log(d);
}

//------------------------------------------------------------------------------
// Derivada Analítica: f'(d) = a - ln(d) - 1
double df_analitica(double d, double a) {
    return a - log(d) - 1.0;
}

//------------------------------------------------------------------------------
// Derivada Numérica (diferenças finitas centrais)
double df_numerica(double (*func)(double, double), double d, double a, double h = 1e-7) {
    // f'(x) ~ [f(x+h) - f(x-h)] / (2*h)
    // Garantir x-h > 0
    double d_minus = (d - h > 0) ? (d - h) : (d + h);
    double fPh = func(d + h, a);
    double fMh = func(d_minus, a);
    return (fPh - fMh) / (2.0 * h);
}

//------------------------------------------------------------------------------
// Tentar encontrar intervalo [xLow, xHigh] que contenha a raiz
bool findBracket(double &xLow, double &xHigh, double a) {
    const int MAX_EXPANSOES = 50;

    if (xLow <= 0) xLow = 1e-3;

    if (f(xLow, a) * f(xHigh, a) < 0.0) {
        return true;
    }

    // Tentar expandir
    for (int i = 0; i < MAX_EXPANSOES; i++) {
        if (fabs(xLow) < fabs(xHigh)) {
            xHigh *= 2.0; 
        } else {
            xLow /= 2.0;
            if (xLow <= 0) xLow = 1e-3; 
        }

        if (f(xLow, a) * f(xHigh, a) < 0.0) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
// Método da Bisseção
pair<double,int> bisseccao(double xLow, double xHigh, double a, double eps, int maxIter) {
    double fLow = f(xLow, a);
    double root = xLow;
    int iter = 0;
    
    for (; iter < maxIter; iter++) {
        root = 0.5 * (xLow + xHigh);
        double fMid = f(root, a);

        if (fabs(fMid) < eps) break;

        if (fLow * fMid > 0) {
            xLow = root;
            fLow = fMid;
        } else {
            xHigh = root;
        }

        if (fabs(xHigh - xLow) < eps) break;
    }
    return make_pair(root, iter+1);
}

//------------------------------------------------------------------------------
// Método da Posição Falsa
pair<double,int> posicaoFalsa(double xLow, double xHigh, double a, double eps, int maxIter) {
    double fLow = f(xLow, a);
    double fHigh = f(xHigh, a);
    double root = xLow;
    int iter = 0;

    for (; iter < maxIter; iter++) {
        root = (xLow * fHigh - xHigh * fLow) / (fHigh - fLow);
        double fRoot = f(root, a);

        if (fabs(fRoot) < eps) break;

        if (fLow * fRoot < 0) {
            xHigh = root;
            fHigh = fRoot;
        } else {
            xLow = root;
            fLow = fRoot;
        }

        if (fabs(xHigh - xLow) < eps) break;
    }
    return make_pair(root, iter+1);
}

//------------------------------------------------------------------------------
// Método de Newton-Raphson
pair<double,int> newtonRaphson(double x0, double a, double eps, int maxIter, bool usarDerivadaNumerica) {
    double x = x0;
    int iter = 0;

    for (; iter < maxIter; iter++) {
        double fx = f(x, a);
        double fpx = 0.0;

        if (usarDerivadaNumerica) {
            fpx = df_numerica(f, x, a);
        } else {
            fpx = df_analitica(x, a);
        }

        if (fabs(fpx) < 1e-14) {
            // Derivada ~ zero, parar
            break;
        }

        double xNext = x - fx / fpx;

        if (fabs(xNext - x) < eps && fabs(fx) < eps) {
            x = xNext;
            break;
        }

        x = xNext;
        if (x <= 0) {
            // Evita d <= 0
            x = 1e-3;
        }
    }
    return make_pair(x, iter+1);
}

//------------------------------------------------------------------------------
int main() {
    // Ajuste do locale (opcional, se quiser forçar algo no console)
    locale::global(std::locale::classic());

    int n;              // Quantidade de foguetes
    double eps;         // Precisão
    int maxIter = 20;   // Máximo de iterações
    
    cout << "Digite a quantidade de foguetes (n): ";
    cin >> n;
    cout << "Digite a precisao (epsilon): ";
    cin >> eps;

    // Vetores que guardarão resultados
    vector<double> valoresA(n);
    vector<double> raizBissec(n), raizPosFalsa(n);
    vector<double> raizNewtonAnalit(n), raizNewtonNumer(n);
    vector<int> iterBissec(n), iterPosFalsa(n);
    vector<int> iterNewtonAnalit(n), iterNewtonNumer(n);

    // --------------------------------------------------------------
    // Opcao de como definir 'a'
    // --------------------------------------------------------------
    cout << "\nComo deseja definir os valores de 'a'?\n";
    cout << "1 - Gerar automaticamente (ponto inicial e passo)\n";
    cout << "2 - Digitar manualmente cada valor\n";
    cout << "Digite a opcao: ";
    int opc;
    cin >> opc;

    if (opc == 1) {
        // Geração automática
        double aStart, passo;
        cout << "Digite o ponto inicial de a: ";
        cin >> aStart;
        cout << "Digite a variacao de a (passo): ";
        cin >> passo;

        // Preenche os valores de a
        for (int i = 0; i < n; i++) {
            valoresA[i] = aStart + i * passo;
        }
    }
    else if (opc == 2) {
        // Digitação manual
        for (int i = 0; i < n; i++) {
            cout << "Digite o valor de a para o foguete " << (i+1) << ": ";
            cin >> valoresA[i];
        }
    }
    else {
        cerr << "Opcao invalida. Encerrando.\n";
        return 1;
    }

    // --------------------------------------------------------------
    // Cálculo das raízes
    // --------------------------------------------------------------
    for (int i = 0; i < n; i++) {
        double a = valoresA[i];

        // Tentar achar intervalo [xLow, xHigh]
        double xLow = 1.0;
        double xHigh = 2.0;
        bool achou = findBracket(xLow, xHigh, a);

        if (!achou) {
            cerr << "\nNao encontrou intervalo para a = " << a << endl;
            raizBissec[i]         = NAN;  iterBissec[i]         = 0;
            raizPosFalsa[i]       = NAN;  iterPosFalsa[i]       = 0;
            raizNewtonAnalit[i]   = NAN;  iterNewtonAnalit[i]   = 0;
            raizNewtonNumer[i]    = NAN;  iterNewtonNumer[i]    = 0;
            continue;
        }

        // Bissecao
        auto [rB, itB] = bisseccao(xLow, xHigh, a, eps, maxIter);
        raizBissec[i] = rB;  iterBissec[i] = itB;

        // Posicao Falsa
        auto [rF, itF] = posicaoFalsa(xLow, xHigh, a, eps, maxIter);
        raizPosFalsa[i] = rF;  iterPosFalsa[i] = itF;

        // Newton-Raphson (analitico)
        double x0 = 0.5 * (xLow + xHigh);
        auto [rNA, itNA] = newtonRaphson(x0, a, eps, maxIter, false);
        raizNewtonAnalit[i] = rNA;  iterNewtonAnalit[i] = itNA;

        // Newton-Raphson (numerico)
        x0 = 0.5 * (xLow + xHigh);
        auto [rNN, itNN] = newtonRaphson(x0, a, eps, maxIter, true);
        raizNewtonNumer[i] = rNN;   iterNewtonNumer[i] = itNN;
    }

    // --------------------------------------------------------------
    // Impressao no console (para conferência)
    // --------------------------------------------------------------
    cout << fixed << setprecision(6);
    cout << "\nResultados (console):\n";
    cout << "---------------------------------------------------------------------------------------------\n";
    cout << setw(10) << "a"
         << setw(15) << "BissecRaiz"
         << setw(12) << "BissecIter"
         << setw(15) << "PosFalsaRaiz"
         << setw(12) << "PosFalsaIter"
         << setw(15) << "NR_An_Raiz"
         << setw(12) << "NR_An_Iter"
         << setw(15) << "NR_Num_Raiz"
         << setw(12) << "NR_Num_Iter"
         << endl;
    cout << "---------------------------------------------------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        cout << setw(10) << valoresA[i]
             << setw(15) << raizBissec[i]
             << setw(12) << iterBissec[i]
             << setw(15) << raizPosFalsa[i]
             << setw(12) << iterPosFalsa[i]
             << setw(15) << raizNewtonAnalit[i]
             << setw(12) << iterNewtonAnalit[i]
             << setw(15) << raizNewtonNumer[i]
             << setw(12) << iterNewtonNumer[i]
             << endl;
    }

    // --------------------------------------------------------------
    // Geração do CSV
    // --------------------------------------------------------------
    ofstream csvFile("resultado.csv");
    if (!csvFile.is_open()) {
        cerr << "\nErro ao criar arquivo CSV.\n";
        return 1;
    }

    // Cabeçalho
    csvFile << "a;"
            << "BissecRaiz;BissecIter;"
            << "PosFalsaRaiz;PosFalsaIter;"
            << "NewtonRaizAnalit;NewtonIterAnalit;"
            << "NewtonRaizNum;NewtonIterNum\n";

    // Aqui vamos imprimir valores com vírgula como decimal, p/ Excel PT-BR:
    for (int i = 0; i < n; i++) {
        // Converte double -> string "pt-BR" => "1,105164"
        string aStr   = doubleToBrazilString(valoresA[i]);
        string bRaiz  = doubleToBrazilString(raizBissec[i]);
        string pfRaiz = doubleToBrazilString(raizPosFalsa[i]);
        string nRaizA = doubleToBrazilString(raizNewtonAnalit[i]);
        string nRaizN = doubleToBrazilString(raizNewtonNumer[i]);

        // Iterações continuam como inteiros
        int bIter  = iterBissec[i];
        int pfIter = iterPosFalsa[i];
        int nIterA = iterNewtonAnalit[i];
        int nIterN = iterNewtonNumer[i];

        csvFile << aStr << ";"
                << bRaiz << ";" << bIter << ";"
                << pfRaiz << ";" << pfIter << ";"
                << nRaizA << ";" << nIterA << ";"
                << nRaizN << ";" << nIterN << "\n";
    }

    csvFile.close();
    cout << "\nArquivo 'resultado.csv' gerado com sucesso!\n";

    return 0;
}

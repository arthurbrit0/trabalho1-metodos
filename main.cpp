#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <locale>
#include <sstream>

// Inclui nosso cabeçalho com as funções
#include "metodos.h"

using namespace std;

//------------------------------------------------------------------------------
// Converte número double (com ponto) para string com vírgula no decimal
// Exemplo: 1.105164 -> "1,105164"
string doubleToBrazilString(double valor, int precision = 6) {
    ostringstream oss;
    oss << fixed << setprecision(precision) << valor; 
    string s = oss.str();
    // Substituir '.' por ',' 
    for (char &c : s) {
        if (c == '.') c = ',';
    }
    return s;
}

//------------------------------------------------------------------------------
// Função para retornar se "explode?" => True (raiz>2), False (raiz<=2) ou null (NaN)
string explodeStatus(double root) {
    if (std::isnan(root)) {
        return "null";
    }
    else if (root > 2.0) {
        return "True";
    }
    else {
        return "False";
    }
}

//------------------------------------------------------------------------------
int main() {
    // Ajuste do locale para usar '.' no console (opcional)
    locale::global(std::locale::classic());

    int n;       // quantidade de foguetes
    double eps;  // precisão
    int maxIter = 20; // número máximo de iterações

    cout << "Digite a quantidade de foguetes (n): ";
    cin >> n;
    cout << "Digite a precisao (epsilon): ";
    cin >> eps;

    // Vetores
    vector<double> valoresA(n);
    vector<double> raizBissec(n), raizPosFalsa(n);
    vector<double> raizNewtonAnalit(n), raizNewtonNumer(n);
    vector<int>    iterBissec(n), iterPosFalsa(n);
    vector<int>    iterNewtonAnalit(n), iterNewtonNumer(n);

    // --------------------------------------------------------------
    // Escolher como definir 'a'
    // --------------------------------------------------------------
    cout << "\nComo deseja definir os valores de 'a'?\n";
    cout << "1 - Gerar automaticamente (ponto inicial e passo)\n";
    cout << "2 - Digitar manualmente cada valor\n";
    cout << "Digite a opcao: ";
    int opc;
    cin >> opc;

    if (opc == 1) {
        double aStart, passo;
        cout << "Digite o ponto inicial de a: ";
        cin >> aStart;
        cout << "Digite a variacao de a (passo): ";
        cin >> passo;

        for (int i = 0; i < n; i++) {
            valoresA[i] = aStart + i*passo;
        }
    }
    else if (opc == 2) {
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
    // Calcular as raízes
    // --------------------------------------------------------------
    for (int i = 0; i < n; i++) {
        double a = valoresA[i];

        // Isolamento
        double xLow = 1.0;
        double xHigh = 2.0;
        bool achou = findBracket(xLow, xHigh, a);
        if (!achou) {
            cerr << "Nao encontrou intervalo para a = " << a << endl;
            raizBissec[i]       = NAN; iterBissec[i]       = 0;
            raizPosFalsa[i]     = NAN; iterPosFalsa[i]     = 0;
            raizNewtonAnalit[i] = NAN; iterNewtonAnalit[i] = 0;
            raizNewtonNumer[i]  = NAN; iterNewtonNumer[i]  = 0;
            continue;
        }

        // Bissecao
        auto [rB, itB] = bisseccao(xLow, xHigh, a, eps, maxIter);
        raizBissec[i] = rB; 
        iterBissec[i] = itB;

        // PosFalsa
        auto [rF, itF] = posicaoFalsa(xLow, xHigh, a, eps, maxIter);
        raizPosFalsa[i] = rF;
        iterPosFalsa[i] = itF;

        // Newton Analitico
        double x0 = 0.5*(xLow + xHigh);
        auto [rNA, itNA] = newtonRaphson(x0, a, eps, maxIter, false);
        raizNewtonAnalit[i] = rNA;
        iterNewtonAnalit[i] = itNA;

        // Newton Numerico
        x0 = 0.5*(xLow + xHigh);
        auto [rNN, itNN] = newtonRaphson(x0, a, eps, maxIter, true);
        raizNewtonNumer[i] = rNN;
        iterNewtonNumer[i] = itNN;
    }

    // --------------------------------------------------------------
    // Impressao no console (13 colunas)
    // a, BissRaiz, BissIter, BissExp,
    // PFRaiz, PFIter, PFExp,
    // NRA_Raiz, NRA_Iter, NRA_Exp,
    // NRN_Raiz, NRN_Iter, NRN_Exp
    // --------------------------------------------------------------
    cout << fixed << setprecision(6);
    cout << "\nResultados (console):\n";
    cout << "--------------------------------------------------------------------------------------------------------------------------------\n";
    cout << setw(10) << "a"
         << setw(12) << "BissRaiz"
         << setw(10) << "BissIter"
         << setw(10) << "BissExp"
         << setw(12) << "PFRaiz"
         << setw(10) << "PFIter"
         << setw(10) << "PFExp"
         << setw(12) << "NRA_Raiz"
         << setw(10) << "NRA_Iter"
         << setw(10) << "NRA_Exp"
         << setw(12) << "NRN_Raiz"
         << setw(10) << "NRN_Iter"
         << setw(10) << "NRN_Exp"
         << endl;
    cout << "--------------------------------------------------------------------------------------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        double a     = valoresA[i];
        double rB    = raizBissec[i];
        double rF    = raizPosFalsa[i];
        double rNA   = raizNewtonAnalit[i];
        double rNN   = raizNewtonNumer[i];

        // explode
        string eB  = explodeStatus(rB);
        string eF  = explodeStatus(rF);
        string eNA = explodeStatus(rNA);
        string eNN = explodeStatus(rNN);

        cout << setw(10) << a
             << setw(12) << rB
             << setw(10) << iterBissec[i]
             << setw(10) << eB
             << setw(12) << rF
             << setw(10) << iterPosFalsa[i]
             << setw(10) << eF
             << setw(12) << rNA
             << setw(10) << iterNewtonAnalit[i]
             << setw(10) << eNA
             << setw(12) << rNN
             << setw(10) << iterNewtonNumer[i]
             << setw(10) << eNN
             << endl;
    }

    // --------------------------------------------------------------
    // Geração do CSV
    // 13 colunas:
    // a, BissecRaiz, BissecIter, BissecExp,
    // PosFalsaRaiz, PosFalsaIter, PosFalsaExp,
    // NewtonRaizAnalit, NewtonIterAnalit, NewtonExplodeAnalit,
    // NewtonRaizNum, NewtonIterNum, NewtonExplodeNum
    // --------------------------------------------------------------
    ofstream csvFile("resultado.csv");
    if (!csvFile.is_open()) {
        cerr << "\nErro ao criar arquivo CSV.\n";
        return 1;
    }

    csvFile << "a;"
            << "BissecRaiz;BissecIter;BissecExplode;"
            << "PosFalsaRaiz;PosFalsaIter;PosFalsaExplode;"
            << "NewtonRaizAnalit;NewtonIterAnalit;NewtonExplodeAnalit;"
            << "NewtonRaizNum;NewtonIterNum;NewtonExplodeNum\n";

    for (int i = 0; i < n; i++) {
        double a   = valoresA[i];
        double rB  = raizBissec[i];
        double rF  = raizPosFalsa[i];
        double rNA = raizNewtonAnalit[i];
        double rNN = raizNewtonNumer[i];

        string aStr   = doubleToBrazilString(a);
        string bRaiz  = doubleToBrazilString(rB);
        string fRaiz  = doubleToBrazilString(rF);
        string naRaiz = doubleToBrazilString(rNA);
        string nnRaiz = doubleToBrazilString(rNN);

        // Explode status
        string eB  = explodeStatus(rB);
        string eF  = explodeStatus(rF);
        string eNA = explodeStatus(rNA);
        string eNN = explodeStatus(rNN);

        csvFile << aStr    << ";"
                << bRaiz   << ";" << iterBissec[i]       << ";" << eB  << ";"
                << fRaiz   << ";" << iterPosFalsa[i]     << ";" << eF  << ";"
                << naRaiz  << ";" << iterNewtonAnalit[i] << ";" << eNA << ";"
                << nnRaiz  << ";" << iterNewtonNumer[i]  << ";" << eNN
                << "\n";
    }

    csvFile.close();
    cout << "\nArquivo 'resultado.csv' gerado com sucesso!\n";

    return 0;
}

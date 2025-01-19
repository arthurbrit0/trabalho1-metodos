#ifndef METODOS_H
#define METODOS_H

#include <utility> // std::pair
#include <cmath>   // std::fabs, std::log

// Declarações das funções

// f(d) = a*d - d*ln(d)
double f(double d, double a);

// Derivadas analítica e numérica
double df_analitica(double d, double a);
double df_numerica(double (*func)(double, double), double d, double a, double h=1e-7);

// Função para encontrar intervalo que contenha a raiz
bool findBracket(double &xLow, double &xHigh, double a);

// Métodos Numéricos
std::pair<double,int> bisseccao(double xLow, double xHigh, double a, double eps, int maxIter);
std::pair<double,int> posicaoFalsa(double xLow, double xHigh, double a, double eps, int maxIter);
std::pair<double,int> newtonRaphson(double x0, double a, double eps, int maxIter, bool usarDerivadaNumerica);

#endif // METODOS_H

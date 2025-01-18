#ifndef METODOS_H
#define METODOS_H

#include <utility> // for std::pair

double f(double d, double a);
bool   findBracket(double &xLow, double &xHigh, double a);
std::pair<double,int> bisseccao(double xLow, double xHigh, double a, double eps, int maxIter);
std::pair<double,int> posicaoFalsa(double xLow, double xHigh, double a, double eps, int maxIter);
std::pair<double,int> newtonRaphson(double x0, double a, double eps, int maxIter, bool usarDerivadaNumerica);

#endif // METODOS_H

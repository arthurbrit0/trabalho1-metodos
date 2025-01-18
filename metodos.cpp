#include "metodos.h"
#include <cmath>
#include <cfloat>

// Exemplo do f(d)
double f(double d, double a) {
    if (d <= 0.0) return 1.0e10; // penaliza
    return a*d - d*std::log(d);
}

static double df_analitica(double d, double a) {
    return a - std::log(d) - 1.0;
}

// Derivada numérica (dif. finitas centrais)
static double df_numerica(double d, double a) {
    double h = 1e-7;
    double d_minus = (d - h > 0 ? d - h : d + h);
    double fPh = f(d + h, a);
    double fMh = f(d_minus, a);
    return (fPh - fMh) / (2.0*h);
}

bool findBracket(double &xLow, double &xHigh, double a) {
    if (xLow <= 0) xLow = 1e-3;
    const int MAX_EXPAND = 50;

    double fLow = f(xLow,a);
    double fHigh = f(xHigh,a);
    if (fLow * fHigh < 0.0) return true;

    for (int i = 0; i < MAX_EXPAND; i++){
        if (fabs(xLow) < fabs(xHigh)) xHigh *= 2.0;
        else {
            xLow /= 2.0;
            if (xLow <= 0) xLow = 1e-3;
        }
        fLow  = f(xLow,a);
        fHigh = f(xHigh,a);
        if (fLow * fHigh < 0.0) return true;
    }
    return false;
}

// Bissecao
std::pair<double,int> bisseccao(double xLow, double xHigh, double a, double eps, int maxIter){
    double fLow = f(xLow,a);
    double root = xLow;
    bool convergiu = false;
    int iter = 0;

    for (; iter < maxIter; iter++){
        root = 0.5*(xLow + xHigh);
        double fMid = f(root,a);

        // Critério de parada
        if (std::fabs(fMid) < eps) {
            convergiu = true;
            break;
        }
        if (fLow * fMid > 0) {
            xLow = root;
            fLow = fMid;
        } else {
            xHigh = root;
        }
        if (std::fabs(xHigh - xLow) < eps) {
            convergiu = true;
            break;
        }
    }

    // Se chegou ao maxIter sem break, nao convergiu
    if (!convergiu) {
        root = NAN;
    }
    return {root, iter+1};
}

// Posição Falsa
std::pair<double,int> posicaoFalsa(double xLow, double xHigh, double a, double eps, int maxIter){
    double fLow = f(xLow,a);
    double fHigh= f(xHigh,a);
    double root = xLow;
    bool convergiu = false;
    int iter = 0;

    for (; iter < maxIter; iter++){
        root = (xLow * fHigh - xHigh * fLow)/(fHigh - fLow);
        double fR = f(root,a);

        if (std::fabs(fR) < eps) {
            convergiu = true;
            break;
        }
        if (fLow * fR < 0){
            xHigh = root;
            fHigh = fR;
        } else {
            xLow = root;
            fLow = fR;
        }
        if (std::fabs(xHigh - xLow) < eps) {
            convergiu = true;
            break;
        }
    }

    if (!convergiu) {
        root = NAN;
    }
    return {root, iter+1};
}

// Newton-Raphson
std::pair<double,int> newtonRaphson(double x0, double a, double eps, int maxIter, bool usarDerivadaNumerica){
    double x = x0;
    bool convergiu = false;
    int iter = 0;

    for (; iter < maxIter; iter++){
        double fx = f(x,a);
        double fpx = (usarDerivadaNumerica ? df_numerica(x,a) : df_analitica(x,a));

        if (std::fabs(fpx) < 1e-14) {
            break; // derivada quase zero => paramos
        }

        double xNext = x - fx/fpx;

        if (std::fabs(xNext - x) < eps && std::fabs(fx) < eps) {
            x = xNext;
            convergiu = true;
            break;
        }
        x = xNext;
        if (x<=0) x=1e-3;
    }

    if (!convergiu) {
        x = NAN;
    }
    return {x, iter+1};
}


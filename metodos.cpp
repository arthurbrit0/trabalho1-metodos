#include "metodos.h"

// Implementações

double f(double d, double a) {
    // Evitar d <= 0, pois ln(d) é indefinido
    if (d <= 0.0) return 1.0e10; 
    return a*d - d*std::log(d);
}

double df_analitica(double d, double a) {
    // f'(d) = a - ln(d) - 1
    return a - std::log(d) - 1.0;
}

double df_numerica(double (*func)(double, double), double d, double a, double h) {
    // f'(x) ~ [f(x+h) - f(x-h)] / (2*h)
    double d_minus = (d - h > 0 ? d - h : d + h);
    double fPh = func(d + h, a);
    double fMh = func(d_minus, a);
    return (fPh - fMh)/(2.0*h);
}

// Tenta encontrar [xLow, xHigh] com sinais opostos
bool findBracket(double &xLow, double &xHigh, double a) {
    const int MAX_EXPAND = 50;

    if (xLow <= 0) xLow = 1e-3;
    double fLow  = f(xLow,a);
    double fHigh = f(xHigh,a);

    if (fLow*fHigh < 0.0) {
        return true;
    }

    for (int i=0; i<MAX_EXPAND; i++) {
        if (std::fabs(xLow) < std::fabs(xHigh)) {
            xHigh *= 2.0;
        } else {
            xLow /= 2.0;
            if (xLow <= 0) xLow = 1e-3;
        }
        fLow  = f(xLow, a);
        fHigh = f(xHigh,a);
        if (fLow*fHigh < 0.0) {
            return true;
        }
    }
    return false; // não encontrou mudança de sinal
}

// Bisseção
std::pair<double,int> bisseccao(double xLow, double xHigh, double a, double eps, int maxIter) {
    double fLow = f(xLow,a);
    double root = xLow;
    int iter = 0;

    for(; iter < maxIter; iter++) {
        root = 0.5*(xLow + xHigh);
        double fMid = f(root,a);
        if (std::fabs(fMid) < eps) break;

        if (fLow*fMid > 0) {
            xLow = root;
            fLow = fMid;
        } else {
            xHigh = root;
        }
        if (std::fabs(xHigh - xLow) < eps) break;
    }
    return {root, iter+1};
}

// Posição Falsa
std::pair<double,int> posicaoFalsa(double xLow, double xHigh, double a, double eps, int maxIter) {
    double fLow  = f(xLow,a);
    double fHigh = f(xHigh,a);
    double root = xLow;
    int iter = 0;

    for(; iter < maxIter; iter++) {
        root = (xLow*fHigh - xHigh*fLow)/(fHigh - fLow);
        double fR = f(root,a);
        if (std::fabs(fR) < eps) break;

        if (fLow*fR < 0) {
            xHigh = root;
            fHigh = fR;
        } else {
            xLow  = root;
            fLow  = fR;
        }
        if (std::fabs(xHigh - xLow) < eps) break;
    }
    return {root, iter+1};
}

// Newton-Raphson
std::pair<double,int> newtonRaphson(double x0, double a, double eps, int maxIter, bool usarDerivadaNumerica) {
    double x = x0;
    int iter = 0;

    for(; iter < maxIter; iter++) {
        double fx = f(x,a);
        double fpx = (usarDerivadaNumerica ? df_numerica(f, x, a) : df_analitica(x,a));

        if (std::fabs(fpx) < 1e-14) {
            // deriva ~ 0 => paramos
            break;
        }
        double xNext = x - fx/fpx;

        if (std::fabs(xNext - x) < eps && std::fabs(fx) < eps) {
            x = xNext;
            break;
        }
        x = xNext;
        if (x <= 0) {
            x = 1e-3;
        }
    }
    return {x, iter+1};
}

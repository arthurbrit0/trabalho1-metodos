#include "posicao_falsa.h"

double f(double d, double a) {
    return a*d - d*log(d);
}

double posicaoFalsa(double a, double x0, double x1, double e) {
    if (f(x0, a) * f(x1, a) > 0) return NAN;

    double xNovo = 0.0;
    double erro = 0.0;
    const int maxIteracoes = 1000;
    int iteracoes = 0;

    while (true) {
        double f_x0 = f(x0, a);
        double f_x1 = f(x1, a);
        xNovo = (x0 * f_x1 - x1 * f_x0)/(f_x1 - f_x0);
        
        erro = fabs(f(xNovo, a));

        if (erro < e || iteracoes >= maxIteracoes) break;

        if (f_x0 * f(xNovo, a) < 0) {
            x1 = xNovo;
        } else {
            x0 = xNovo;
        }

        iteracoes++;
    }

    return iteracoes < maxIteracoes ? xNovo : NAN;
}

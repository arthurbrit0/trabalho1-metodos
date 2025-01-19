# Documentação do Sistema de Cálculo de Deslocamento de Foguetes

## **Introdução**
Este sistema foi desenvolvido para calcular o deslocamento da extremidade de um foguete espacial ao entrar na atmosfera terrestre. A equação de deslocamento é dada por:

$f(d) = a \cdot d - d \cdot \ln(d) $

Onde:
- \(d\): deslocamento (em cm)
- \(a\): parâmetro de ajuste

O objetivo é garantir que o deslocamento \(d\) não ultrapasse 2 cm, evitando explosões e prejuízos significativos. Para isso, foram implementados os seguintes métodos numéricos:
- Método da Bissecção
- Método da Posição Falsa
- Método de Newton-Raphson

A seguir, detalhamos a implementação, testes e resultados obtidos.

---

## **Implementação**

### **1. Método da Bissecção**
#### Descrição:
- Divide o intervalo \($x_{low}, x_{high}$\) ao meio em cada iteração.
- Verifica se a raiz está na metade inferior ou superior do intervalo.
- Repete até atingir a precisão $\varepsilon$ .

#### Código:
```cpp
std::pair<double,int> bisseccao(double xLow, double xHigh, double a, double eps, int maxIter);
```

### **2. Método da Posição Falsa**
#### Descrição:
- Usa uma fórmula de interpolação linear para aproximar a raiz.
- Refina o intervalo com base na posição da raiz aproximada.
- Mais eficiente que a Bissecção em muitos casos.

#### Código:
```cpp
std::pair<double,int> posicaoFalsa(double xLow, double xHigh, double a, double eps, int maxIter);
```

### **3. Método de Newton-Raphson**
#### Descrição:
- Utiliza a derivada da função para encontrar aproximações iterativas para a raiz.
- Implementado com derivada analítica e numérica.

#### Código:
```cpp
std::pair<double,int> newtonRaphson(double x0, double a, double eps, int maxIter, bool usarDerivadaNumerica);
```

### **4. Funções Auxiliares no `metodos.cpp`**

#### **f(double d, double a)**
- Calcula o valor da função $f(d) = a \cdot d - d \cdot \ln(d)$
- Penaliza valores \$d \leq 0$ retornando um valor muito alto.

#### Código:
```cpp
double f(double d, double a);
```

#### **df_analitica(double d, double a)**
- Calcula a derivada analítica da função $f(d)$:
  $f'(d) = a - \ln(d) - 1$

#### Código:
```cpp
static double df_analitica(double d, double a);
```

#### **df_numerica(double d, double a)**
- Calcula a derivada numérica de \(f(d)\) usando diferenças finitas centrais:
  $f'(d) \approx \frac{f(d + h) - f(d - h)}{2h}$
- \(h\): passo pequeno $(1 \times 10^{-7}$.

#### Código:
```cpp
static double df_numerica(double d, double a);
```

#### **findBracket(double &xLow, double &xHigh, double a)**
- Tenta isolar uma raiz da função no intervalo inicial $[xLow, xHigh]$.
- Expande o intervalo progressivamente até encontrar uma mudança de sinal em \(f(x)\).
- Retorna `true` se encontrar um intervalo válido, caso contrário `false`.

#### Código:
```cpp
bool findBracket(double &xLow, double &xHigh, double a);
```

#### **bisseccao**
- Implementa o método da bissecção para encontrar a raiz em um intervalo.
- Parâmetros:
  - `xLow`, `xHigh`: intervalo inicial.
  - `a`: parâmetro da equação.
  - `eps`: precisão desejada.
  - `maxIter`: número máximo de iterações.
- Retorna a raiz encontrada e o número de iterações realizadas.

#### **posicaoFalsa**
- Implementa o método da posição falsa.
- Similar ao método da bissecção, mas usa interpolação linear para aproximação da raiz.
- Parâmetros e retorno semelhantes à bissecção.

#### **newtonRaphson**
- Implementa o método de Newton-Raphson para encontrar a raiz.
- Parâmetros:
  - `x0`: chute inicial.
  - `a`: parâmetro da equação.
  - `eps`: precisão desejada.
  - `maxIter`: número máximo de iterações.
  - `usarDerivadaNumerica`: define se será usada derivada analítica ou numérica.
- Retorna a raiz encontrada e o número de iterações realizadas.

---

## **Requisitos do Sistema**

- **Entradas**:
  - \(n\): número de foguetes a serem analisados.
  - \(a\): valores do parâmetro de ajuste para cada foguete.
  - $\varepsilon$: precisão desejada.
- **Saídas**:
  - Resultados para cada método, incluindo:
    - Raiz encontrada \(d\).
    - Número de iterações.
    - Verificação se o foguete "explode" $d > 2$.

---

## **Testes Realizados**

### **Configurações dos Testes**
- Parâmetros fixos:
  - $a = 1.0$
  - Intervalo inicial: $[2, 3]$
  - Precisão $\varepsilon = 10^{-5}$

### **Resultados (Exemplo)**
| Método          | Raiz \(d\)   | Iterações | Explosão |
|-----------------|--------------|-----------|----------|
| Bissecção       | -------      | --        | ---      |
| Posição Falsa   | -------      | -         | ---      |
| Newton-Raphson  | -------      | -         | ---      |

---

## **Análise de Variação de \(a\)**

### **Configuração:**
- Intervalo de \(a\): $[0.5, 2.0]$, passo de 0.5

### **Quadro Resumo:**
| \(a\)  | Método          | Raiz \(d\)   | Iterações | Explosão |
|--------|-----------------|--------------|-----------|----------|
| 0.5    | Bissecção       | -------      | ---       | ----     |
| 0.5    | Posição Falsa   | -------      | -         | ---      |
| 0.5    | Newton-Raphson  | -------      | -         | ---      |
| 1.0    | Bissecção       | -------      | --        | ---      |
| 1.0    | Posição Falsa   | -------      | -         | ---      |
| 1.0    | Newton-Raphson  | -------      | -         | ---      |
| 2.0    | Bissecção       | -------      | --        | ---      |
| 2.0    | Posição Falsa   | -------      | --        | ---      |
| 2.0    | Newton-Raphson  | -------      | -         | ---      |

---

## **Conclusão**
- **Eficiência**:
  - Newton-Raphson geralmente requer menos iterações.
  - A Posição Falsa é mais rápida que a Bissecção, mas pode apresentar oscilações.
- **Segurança**:
  - O sistema detecta condições críticas (explosões) com alta precisão.
- **Variação de \(a\)**:
  - Valores maiores de \(a\) tendem a aumentar o deslocamento, exigindo maior cuidado no projeto.

---

## **Execução do Código**

### **Compilação e Execução**
1. Compile os arquivos:
   ```bash
   g++ -o foguete main.cpp metodos.cpp mainwindow.cpp -std=c++17 -lQtWidgets
   ```
2. Execute o programa:
   ```bash
   ./foguete
   ```

---

## **Funcionalidades Extras**
- **Exportação para CSV**: Os resultados podem ser exportados para análise externa.
- **Interface Gráfica**: Desenvolvida em Qt, permite configuração de parâmetros e visualização dos resultados em tabelas interativas.

---

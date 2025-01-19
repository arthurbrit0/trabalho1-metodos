#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "metodos.h"   // Declarações das funções f, bisseccao, etc.
#include <QInputDialog>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configurações iniciais
    ui->r->setChecked(true);
    on_radioAuto_toggled(true); // habilita aStart/passso, desabilita manual
    // Preparar tabela
    // Vamos ter 14 colunas no total
    ui->tableWidget->setColumnCount(14);

    // Define o cabeçalho (14 itens)
    QStringList headers;
    headers << "a"
            << "Isolamento"
            << "BissecRaiz" << "BissecIter" << "BissecExp"
            << "PosFalsaRaiz" << "PosFalsaIter" << "PosFalsaExp"
            << "NR_An_Raiz" << "NR_An_Iter" << "NR_An_Exp"
            << "NR_Num_Raiz" << "NR_Num_Iter" << "NR_Num_Exp";

    ui->tableWidget->setHorizontalHeaderLabels(headers);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_radioAuto_toggled(bool checked)
{

    ui->lineAstart->setEnabled(checked);
    ui->linePasso->setEnabled(checked);


}

void MainWindow::on_btnCalcular_clicked()
{
    // Ler n e epsilon
    int n = ui->spinN->value();
    double eps = ui->lineEps->text().toDouble();

    if (n <= 0) {
        QMessageBox::warning(this, "Erro", "Quantidade de foguetes invalida (n <= 0)");
        return;
    }
    if (eps <= 0.0) {
        QMessageBox::warning(this, "Erro", "Precisao deve ser > 0");
        return;
    }


    std::vector<double> valoresA(n);

    if (ui->r->isChecked()) {
        // Geração automática
        double aStart = ui->lineAstart->text().toDouble();
        double passo  = ui->linePasso->text().toDouble();

        for (int i = 0; i < n; i++) {
            valoresA[i] = aStart + i*passo;
        }
    } else {

        for (int i = 0; i < n; i++) {
            bool ok;
            double val = QInputDialog::getDouble(this,
                                                 QString("Foguete %1").arg(i+1),
                                                 "Digite o valor de a:",
                                                 0.0, -1.0e9, 1.0e9, 6, &ok);
            if (!ok) {
                QMessageBox::warning(this, "Cancelado", "Entrada manual cancelada.");
                return;
            }
            valoresA[i] = val;
        }
    }

    // Agora calculamos os resultados para cada a
    // Vamos limpar a tableWidget e inserir n linhas
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(n);

    for (int i = 0; i < n; i++)
    {
        double a = valoresA[i];

        // 1) Isolar
        double xLow = 1.0;
        double xHigh = 2.0;
        bool achou = findBracket(xLow, xHigh, a);


        QString isoStr;
        if (!achou) {
            isoStr = "[N/A, N/A]";
        } else {
            isoStr = QString("[%1, %2]")
            .arg(xLow, 0, 'f', 6)
                .arg(xHigh, 0, 'f', 6);
        }


        double rBissec = NAN, rPosFalsa = NAN, rNewtonA = NAN, rNewtonN = NAN;
        int itB = 0, itF = 0, itNA = 0, itNN = 0;

        if (achou) {
            // 2) Métodos Numéricos
            auto [rB, itb] = bisseccao(xLow, xHigh, a, eps, 20);
            rBissec = rB; itB = itb;

            auto [rF, itf] = posicaoFalsa(xLow, xHigh, a, eps, 20);
            rPosFalsa = rF; itF = itf;

            double x0 = 0.5*(xLow + xHigh);
            auto [rNA, itna] = newtonRaphson(x0, a, eps, 20, false);
            rNewtonA = rNA; itNA = itna;

            x0 = 0.5*(xLow + xHigh);
            auto [rNN_, itnn] = newtonRaphson(x0, a, eps, 20, true);
            rNewtonN = rNN_; itNN = itnn;
        }

        // 3) Calcular se "explode?" => True/False/null
        auto explodeFn = [&](double raiz) {
            if (std::isnan(raiz)) return QString("null");
            else if (raiz > 2.0) return QString("True");
            else return QString("False");
        };

        QString expB = explodeFn(rBissec);
        QString expF = explodeFn(rPosFalsa);
        QString expNA= explodeFn(rNewtonA);
        QString expNN= explodeFn(rNewtonN);

        // 4) Preenchendo a tabela (14 colunas)
        // col 0 -> a
        ui->tableWidget->setItem(i, 0,
                                 new QTableWidgetItem(QString::number(a, 'f', 6)));

        // col 1 -> isolamento
        ui->tableWidget->setItem(i, 1,
                                 new QTableWidgetItem(isoStr));

        // Bissec -> cols 2 (raiz), 3 (iter), 4 (explode)
        ui->tableWidget->setItem(i, 2,
                                 new QTableWidgetItem(QString::number(rBissec, 'f', 6)));
        ui->tableWidget->setItem(i, 3,
                                 new QTableWidgetItem(QString::number(itB)));
        ui->tableWidget->setItem(i, 4,
                                 new QTableWidgetItem(expB));

        // PosFalsa -> cols 5,6,7
        ui->tableWidget->setItem(i, 5,
                                 new QTableWidgetItem(QString::number(rPosFalsa, 'f', 6)));
        ui->tableWidget->setItem(i, 6,
                                 new QTableWidgetItem(QString::number(itF)));
        ui->tableWidget->setItem(i, 7,
                                 new QTableWidgetItem(expF));

        // Newton Analitico -> cols 8,9,10
        ui->tableWidget->setItem(i, 8,
                                 new QTableWidgetItem(QString::number(rNewtonA, 'f', 6)));
        ui->tableWidget->setItem(i, 9,
                                 new QTableWidgetItem(QString::number(itNA)));
        ui->tableWidget->setItem(i, 10,
                                 new QTableWidgetItem(expNA));

        // Newton Numerico -> cols 11,12,13
        ui->tableWidget->setItem(i, 11,
                                 new QTableWidgetItem(QString::number(rNewtonN, 'f', 6)));
        ui->tableWidget->setItem(i, 12,
                                 new QTableWidgetItem(QString::number(itNN)));
        ui->tableWidget->setItem(i, 13,
                                 new QTableWidgetItem(expNN));
    }

}

void MainWindow::on_r_toggled(bool checked)
{
    ui->lineAstart->setVisible(checked);
    ui->linePasso->setVisible(checked);
}


void MainWindow::on_btnSalvarCSV_clicked()
{
    // 1) Dialog para escolher o nome do arquivo
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Salvar CSV"),
        "resultado.csv",          // nome default
        tr("CSV Files (*.csv)")   // filtro
        );

    // Se usuário cancelar ou fechar
    if (fileName.isEmpty()) {
        return;
    }

    // 2) Abre o arquivo para escrita
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erro", "Não foi possível abrir o arquivo para escrita.");
        return;
    }

    QTextStream out(&file);

    const QChar sep = ';';

    // 3) Escrever cabeçalho CSV
    out << "a" << sep
        << "BissecRaiz" << sep << "BissecIter" << sep << "BissecExplode" << sep
        << "PosFalsaRaiz" << sep << "PosFalsaIter" << sep << "PosFalsaExplode" << sep
        << "NR_An_Raiz" << sep << "NR_An_Iter" << sep << "NR_An_Explode" << sep
        << "NR_Num_Raiz" << sep << "NR_Num_Iter" << sep << "NR_Num_Explode"
        << "\n";

    // 4) Percorrer a QTableWidget e salvar cada linha
    int rowCount = ui->tableWidget->rowCount();
    int colCount = ui->tableWidget->columnCount();

    for (int r = 0; r < rowCount; r++) {
        QStringList rowData;
        for (int c = 0; c < colCount; c++) {
            QTableWidgetItem* item = ui->tableWidget->item(r, c);
            if (item) {
                // Lê o texto da célula
                rowData << item->text();
            } else {
                rowData << "";
            }
        }
        // Junta os valores da linha separados por ";"
        out << rowData.join(sep) << "\n";
    }

    file.close();

    QMessageBox::information(this, "Sucesso", "CSV salvo em:\n" + fileName);
}


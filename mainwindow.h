#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_radioAuto_toggled(bool checked);
    void on_btnCalcular_clicked();
    void on_r_toggled(bool checked);

    void on_btnSalvarCSV_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

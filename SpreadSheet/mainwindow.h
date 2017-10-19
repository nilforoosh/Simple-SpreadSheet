#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets\QMainWindow>
#include <QtWidgets\QTableWidget>
#include "cell.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int rowCount;
    int columnCount;

private:
    Ui::MainWindow *ui;

    QTableWidget *table;
    void setTable();
    Cell *cell(int,int);
    bool okToContinue();

public slots:
    bool writeFile();
    bool readFile();
    bool closeFile();
    void clear();
    void about();
    void setFormula();
    bool newRow();
    bool newColumn();
    void showFormula(int,int);
};

#endif // MAINWINDOW_H

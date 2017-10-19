#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cell.h"
#include <QtCore\QFile>
#include <QtWidgets\QMessageBox>
#include <QtWidgets\QFileDialog>
#include <QtCore\QDataStream>
#include <QtCore\QString>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    rowCount=10;
    columnCount=10;
    ui->setupUi(this);
    table=ui->tableWidget;
    setTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setTable(){
    table->setItemPrototype(new Cell);
    table->setSelectionMode(QAbstractItemView::ContiguousSelection);
    ui->action_3->setIcon(QIcon(":/icons/download.png"));
    ui->action_6->setIcon(QIcon(":/icons/next.png"));
    ui->action_4->setIcon(QIcon(":/icons/Delete1.png"));
    ui->action_2->setIcon(QIcon(":/icons/Save.png"));
    ui->action->setIcon(QIcon(":/icons/Rip Icon 7.png"));
    ui->action_5->setIcon(QIcon(":/icons/Get Info.png"));
    ui->mainToolBar->addAction(ui->action);
    ui->mainToolBar->addAction(ui->action_2);
    ui->mainToolBar->addAction(ui->action_3);
    ui->mainToolBar->addAction(ui->action_6);
    ui->mainToolBar->insertSeparator(ui->action_3);
    connect(ui->action_2, &QAction::triggered, this, &MainWindow::writeFile);
    connect(ui->action, &QAction::triggered, this, &MainWindow::readFile);
    connect(ui->action_4, &QAction::triggered, this, &MainWindow::closeFile);
    connect(ui->action_5, &QAction::triggered, this, &MainWindow::about);
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &MainWindow::setFormula);
    connect(ui->action_3, &QAction::triggered, this, &MainWindow::newRow);
    connect(ui->action_6, &QAction::triggered, this, &MainWindow::newColumn);
    connect(table, &QTableWidget::cellClicked, this, &MainWindow::showFormula);
}

bool MainWindow::writeFile(){
    QString fileName = QFileDialog::getSaveFileName
                      (this,"Save Spreadsheet",".","spreadsheet files(*.sp)");
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
        tr("Cannot write file %1:\n%2.")
        .arg(file.fileName())
        .arg(file.errorString()));
        return false;
    }
    QDataStream out(&file);
    out <<quint32(0x66666666);
    out <<quint32(rowCount);
    out <<quint32(columnCount);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < table->rowCount(); ++row) {
        for (int column = 0; column < table->columnCount(); ++column) {
            Cell *c=cell(row,column);
            if(c!=NULL){
                QString str =c->formula();
                if (!str.isEmpty())
                    out << quint16(row) << quint16(column)<< str;
            }
        }
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool MainWindow::readFile(){
    QString fileName = QFileDialog::getOpenFileName
                       (this,tr("Open Spreadsheet"), ".",tr("Spreadsheet files (*.sp)"));
    if(fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
        tr("Cannot read file %1:\n%2.")
        .arg(file.fileName())
        .arg(file.errorString()));
        return false;
    }
    QDataStream in(&file);
    quint32 magic,x,y;
    in >> magic;
    in >> x;
    in >> y;
    rowCount=int(x);
    columnCount=int(y);
    clear();
    if (magic != 0x66666666) {
        QMessageBox::warning(this, tr("Spreadsheet"),
        tr("The file is not a Spreadsheet file."));
        return false;
    }
    quint16 row;
    quint16 column;
    QString str;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (!in.atEnd()) {
        in >> row >> column >> str;
        Cell* c=cell(row,column);
        if(!c)
            table->setItem(row,column,c=new Cell);
        c->setFormula(str);
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool MainWindow::closeFile(){
    if(okToContinue())
        return close();
    else
        return false;
}

Cell * MainWindow::cell(int row, int column){
    return static_cast<Cell *>(table->item(row,column));
}

bool MainWindow::okToContinue()
{
    int r = QMessageBox::warning(this,("Spreadsheet"),
        ("The document has been modified.\n"
        "Do you want to save your changes?"),
        QMessageBox::Yes | QMessageBox::No
        | QMessageBox::Cancel);
    if (r == QMessageBox::Yes)
        return writeFile();
    else if (r == QMessageBox::Cancel)
        return false;
return true;
}

void MainWindow::clear(){
    table->setRowCount(0);
    table->setColumnCount(0);
    table->setRowCount(rowCount);
    table->setColumnCount(columnCount);
    for (int i=0;i<columnCount;i++){
        Cell *c = new Cell;
        c->setText(QString(QChar('A' + i)));
        table->setHorizontalHeaderItem(i, c);
    }
    table->setCurrentCell(0,0);
    setTable();
}

void MainWindow::about()
{
    QMessageBox::about(this,"About maker",
    "<h2>IN THE NAME OF ALLAH</h2>"
    "<p>Name : Mohammad Reza"
    "<p>Fmaily Name : Nilforoosh"
    "<p>Birthday : 1372"
    "<p>Field : HardWare Engineering");
}

void MainWindow::setFormula(){
    QString str=ui->lineEdit->text();
    int row=table->currentRow();
    int column=table->currentColumn();
    Cell* c=cell(row,column);
    if(!c)
        table->setItem(row,column,c=new Cell);
    c->setFormula(str);
}

bool MainWindow::newRow(){
    QString fileName = "temp.txt";

    QFile file1(fileName);
    if (!file1.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file1);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < rowCount; row++) {
        for (int column = 0; column < columnCount; column++) {
            Cell *c=cell(row,column);
            if(c!=NULL){
                QString str =c->formula();
                if (!str.isEmpty())
                    out << quint16(row) << quint16(column)<< str;
            }
        }
    }
    QApplication::restoreOverrideCursor();
    file1.close();
    rowCount++;
    table->setRowCount(0);
    table->setColumnCount(0);
    table->setRowCount(rowCount);
    table->setColumnCount(columnCount);
    for (int i=0;i<columnCount;i++){
        Cell *c = new Cell;
        c->setText(QString(QChar('A' + i)));
        table->setHorizontalHeaderItem(i, c);
    }

    QFile file2(fileName);
    if (!file2.open(QIODevice::ReadOnly))
        return false;
    QDataStream in(&file2);
    quint16 row;
    quint16 column;
    QString str;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (!in.atEnd()) {
        in >> row >> column >> str;
        Cell* c=cell(row,column);
        if(!c)
            table->setItem(row,column,c=new Cell);
        c->setFormula(str);
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool MainWindow::newColumn(){
    QString fileName = "temp.txt";

    QFile file1(fileName);
    if (!file1.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file1);
    out <<quint32(0x66666666);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            Cell *c=cell(row,column);
            if(c!=NULL){
                QString str =c->formula();
                if (!str.isEmpty())
                    out << quint16(row) << quint16(column)<< str;
            }
        }
    }
    QApplication::restoreOverrideCursor();
    file1.close();
    columnCount++;
    table->setRowCount(0);
    table->setColumnCount(0);
    table->setRowCount(rowCount);
    table->setColumnCount(columnCount);
    for (int i=0;i<columnCount;i++){
        Cell *c = new Cell;
        c->setText(QString(QChar('A' + i)));
        table->setHorizontalHeaderItem(i, c);
    }

    QFile file2(fileName);
    if (!file2.open(QIODevice::ReadOnly))
        return false;
    QDataStream in(&file2);
    quint32 magic;
    in >> magic;
    quint16 row;
    quint16 column;
    QString str;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (!in.atEnd()) {
        in >> row >> column >> str;
        Cell* c=cell(row,column);
        if(!c)
            table->setItem(row,column,c=new Cell);
        c->setFormula(str);
    }
    QApplication::restoreOverrideCursor();
    return true;
}

void MainWindow::showFormula(int row,int column){
    Cell * c=cell(row,column);
    if (c)
        ui->lineEdit->setText(c->formula());
}

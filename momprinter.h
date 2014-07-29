#ifndef MOMPRINTER_H
#define MOMPRINTER_H

#include <QMainWindow>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QtSql>
#include <QMessageBox>
#include <QSqlRelationalTableModel>

namespace Ui {
class momPrinter;
}

class momPrinter : public QMainWindow
{
    Q_OBJECT

public:
    explicit momPrinter(QWidget *parent = 0);
    ~momPrinter();

private slots:
    void on_addButton_clicked();

    void on_commitButton_clicked();

    void on_cancelButton_clicked();

    void on_deleteButton_clicked();

    void on_printButton_clicked();

private:
    Ui::momPrinter *ui;
    QSqlRelationalTableModel *model;
    QSqlDatabase db;
};

class content
{
public:
    int x;
    int y;
    QString text;

    explicit content();
    explicit content(int x, int y, QString text);
};

//define width and height, all in Millimeter
#define WIDTH 240
#define HEIGHT 150
#define ID_X 45
#define ID_Y 42
#define YEAR_X 100
#define MONTH_X 114
#define DAY_X 128
#define DATE_Y 34
#define TABLE_X 25
#define TABLE_Y 60
#define CELL_X 53
#define ROW_Y 5

#endif // MOMPRINTER_H

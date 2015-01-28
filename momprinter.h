#ifndef MOMPRINTER_H
#define MOMPRINTER_H

#include <tuple>
#include <QMainWindow>
#include <QSettings>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QtSql>
#include <QMessageBox>
#include <QSqlTableModel>

using namespace std;
typedef tuple<int, int, QString> Content;

bool isMust5(QString intString);
QString toChineseNum(int num);

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
    void on_printButton_clicked();
    void updateSum();

private:
    Ui::momPrinter *ui;
    QSqlTableModel *model;
    QSqlDatabase db;
};

//define width and height, all in Millimeter
#define WIDTH 240
#define HEIGHT 150
#define ID_X 45
#define ID_Y 24
#define YEAR_X 100
#define MONTH_X 114
#define DAY_X 128
#define DATE_Y 16
#define TABLE_X 25
#define TABLE_Y 42
#define CELL_X 53
#define ROW_Y 8
#define SUM_ZH_X 40
#define SUM_ZH_Y 107

#endif // MOMPRINTER_H

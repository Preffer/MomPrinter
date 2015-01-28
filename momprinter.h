#ifndef MOMPRINTER_H
#define MOMPRINTER_H

#include <tuple>
#include <QMainWindow>
#include <QtSql>

using namespace std;
typedef tuple<int, int, QString> Content;

bool isMust5(QString intString);
QString toChineseNum(int num);

namespace Ui {
    class MomPrinter;
}

class MomPrinter : public QMainWindow
{
    Q_OBJECT

public:
    explicit MomPrinter(QWidget *parent = 0);
    ~MomPrinter();

private slots:
    void on_printButton_clicked();
    void updateSum();

private:
    Ui::MomPrinter *ui;
    QSqlTableModel *model;
    QSqlDatabase db;

    //define width and height, all in Millimeter
    int WIDTH;
    int HEIGHT;
    int ID_X;
    int ID_Y;
    int YEAR_X;
    int MONTH_X;
    int DAY_X;
    int DATE_Y;
    int TABLE_X;
    int TABLE_Y;
    int CELL_X;
    int ROW_Y;
    int SUM_ZH_X;
    int SUM_ZH_Y;
    int FONT_SIZE;
};

#endif // MOMPRINTER_H

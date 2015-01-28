#include "momprinter.h"
#include "ui_momprinter.h"
#include <QDebug>

momPrinter::momPrinter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::momPrinter)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName (":memory:");
    if(!db.open()){
        QMessageBox::critical(this, "Can not open SQLITE database", db.lastError().text());
    } else{
        //create tables
        QSharedPointer<QSqlQuery> query = (QSharedPointer<QSqlQuery>) new QSqlQuery();
        query->exec("CREATE TABLE content (typeName VARCHAR(64) NOT NULL, num INTEGER NOT NULL, typePrice INTEGER NOT NULL)");
        //insert values
        query->exec("INSERT INTO content VALUES ('壹元票', 0, 1)");
        query->exec("INSERT INTO content VALUES ('贰元票', 0, 2)");
        query->exec("INSERT INTO content VALUES ('伍元票', 0, 5)");
        query->exec("INSERT INTO content VALUES ('拾元票', 0, 10)");
        query->exec("INSERT INTO content VALUES ('伍拾元票', 0, 50)");
        query->exec("INSERT INTO content VALUES ('壹佰元票', 0, 100)");
        model = new QSqlTableModel();
        model->setTable("content");
        model->select();

        ui->tableView->setModel(model);
        ui->tableView->hideColumn(2);
        connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this, SLOT(updateSum()));
    }
}

momPrinter::~momPrinter()
{
    delete ui;
    delete model;
    db.close();
}

void momPrinter::updateSum()
{
    QSharedPointer<QSqlQuery> query = (QSharedPointer<QSqlQuery>) new QSqlQuery();
    query->exec("SELECT num, typePrice FROM content");
    int index_num = query->record().indexOf("num");
    int index_typePrice = query->record().indexOf("typePrice");
    int sum = 0;
    while (query->next()) {
        sum += query->value(index_num).toInt() * query->value(index_typePrice).toInt();
    }
    ui->labelResult->setText(QString::number(sum) + ".00");
}

void momPrinter::on_printButton_clicked()
{
    //updateSum first
    this->updateSum();
    //init a printer
    QSharedPointer<QPrinter> printer = (QSharedPointer<QPrinter>) new QPrinter(QPrinter::HighResolution);
    //set page layout
    printer->setPageLayout(
                QPageLayout(
                    QPageSize(
                        QSizeF(WIDTH, HEIGHT),
                        QPageSize::Millimeter,
                        "stamp-tax",
                        QPageSize::ExactMatch
                        ),
                    QPageLayout::Landscape,
                    QMarginsF(),
                    QPageLayout::Millimeter
                    )
                );
    //display the dialog
    QSharedPointer<QPrintDialog> dialog = (QSharedPointer<QPrintDialog>) new QPrintDialog(printer.data(), this);
    dialog->setWindowTitle("Print Document");
    if (dialog->exec() != QDialog::Accepted){
        return;
    }

    //use painter to draw on the page
    QSharedPointer<QPainter> painter = (QSharedPointer<QPainter>) new QPainter();
    painter->begin(printer.data());
    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    //set font
    QFont font;
    font.setPointSize(12);
    painter->setFont(font);
    //prepare draw
    const int widthPixel = printer->pageRect().width();
    const int heightPixel = widthPixel * HEIGHT / WIDTH;

    //put the static content into the target
    QVector<Content> target(0);
    target.append(Content(ID_X, ID_Y, ui->idEdit->text()));
    target.append(Content(ID_X + CELL_X * 2, ID_Y, ui->nameEdit->text()));
    target.append(Content(DAY_X + CELL_X, DATE_Y, "直属一分局"));
    QDate date = QDate::currentDate ();
    target.append(Content(YEAR_X, DATE_Y, QString::number(date.year())));
    target.append(Content(MONTH_X, DATE_Y, QString::number(date.month())));
    target.append(Content(DAY_X, DATE_Y, QString::number(date.day())));
    //put sql result into the target
    QSharedPointer<QSqlQuery> query = (QSharedPointer<QSqlQuery>) new QSqlQuery();
    query->exec("SELECT typeName, num, typePrice FROM content WHERE num != 0");
    int index_typeName = query->record().indexOf("typeName");
    int index_num = query->record().indexOf("num");
    int index_typePrice = query->record().indexOf("typePrice");
    int currentRow = 0;
    int sum = 0;
    while (query->next()) {
        int price = query->value(index_num).toInt() * query->value(index_typePrice).toInt();
        sum += price;
        target.append(Content(CELL_X * 0 + TABLE_X, ROW_Y * currentRow + TABLE_Y, query->value(index_typeName).toString()));
        target.append(Content(CELL_X * 1 + TABLE_X, ROW_Y * currentRow + TABLE_Y, "借款合同印花税"));
        target.append(Content(CELL_X * 2 + TABLE_X, ROW_Y * currentRow + TABLE_Y, query->value(index_num).toString()));
        target.append(Content(CELL_X * 3 + TABLE_X, ROW_Y * currentRow + TABLE_Y, QString::number(price) + ".00"));
        currentRow++;
    }
    //add the sum
    target.append(Content(SUM_ZH_X, SUM_ZH_Y, toChineseNum(sum)));
    target.append(Content(CELL_X * 3 + TABLE_X, SUM_ZH_Y, QString::number(sum) + ".00"));

    //start paint
    for(Content& item : target){
        painter->drawText(widthPixel * get<0>(item) / WIDTH, heightPixel * get<1>(item) / HEIGHT, get<2>(item));
    }

    painter->end();
}

bool isMust5(QString intString) {
    int length = intString.size();
    if (length > 4) {
        QString subInteger = "";
        if (length > 8) {
            // 取得从低位数，第5到第8位的字串
            subInteger = intString.mid(length - 5, 4);
        } else {
            subInteger = intString.mid(0, 4);
        }
        return subInteger.toInt() > 0;
    } else {
        return false;
    }
}

QString toChineseNum(int num){
    QString sourceString = QString::number(num);
    QStringListIterator sourceStringList(sourceString.split("", QString::SkipEmptyParts));
    QVector<int> integers(0);
    while (sourceStringList.hasNext()){
        integers.append(sourceStringList.next().toInt());
    }
    QString chineseNumbers[] = {"零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖"};
    QString chineseUnits[] = {"元", "拾", "佰", "仟", "万", "拾", "佰", "仟", "亿", "拾", "佰", "仟", "万", "拾", "佰", "仟" };

    QStringList chineseInteger;
    int length = integers.size();
    for (int i = 0; i < length; i++) {
        // 0出现在关键位置：1234(万)5678(亿)9012(万)3456(元)
        // 特殊情况：10(拾元、壹拾元、壹拾万元、拾万元)
        QString key = "";
        if (integers[i] == 0) {
            if ((length - i) == 13)// 万(亿)(必填)
                key = chineseUnits[4];
            else if ((length - i) == 9)// 亿(必填)
                key = chineseUnits[8];
            else if ((length - i) == 5 && isMust5(sourceString))// 万(不必填)
                key = chineseUnits[4];
            else if ((length - i) == 1)// 元(必填)
                key = chineseUnits[0];
            // 0遇非0时补零，不包含最后一位
            if ((length - i) > 1 && integers[i + 1] != 0)
                key += chineseNumbers[0];
        }
        chineseInteger.append(integers[i] == 0 ? key : (chineseNumbers[integers[i]] + chineseUnits[length - i - 1]));
    }
    return chineseInteger.join("") + "整";
}

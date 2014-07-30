#include "momprinter.h"
#include "ui_momprinter.h"
#include <QDebug>

content::content(){};

content::content(int x, int y, QString text) :
    x(x),
    y(y),
    text(text)
{};

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
        query->exec("CREATE TABLE content (typeID INT PRIMARY KEY, num INT)");
        query->exec("CREATE TABLE type(typeID INT PRIMARY KEY, typeName VARCHAR, typePrice REAL)");
        //insert values
        query->exec("INSERT INTO type VALUES (1, '壹元票', 1)");
        query->exec("INSERT INTO type VALUES (2, '贰元票', 2)");
        query->exec("INSERT INTO type VALUES (3, '伍元票', 5)");
        query->exec("INSERT INTO type VALUES (4, '拾元票', 10)");
        query->exec("INSERT INTO type VALUES (5, '伍拾元票', 50)");
        query->exec("INSERT INTO type VALUES (6, '壹佰元票', 100)");
        model = new QSqlRelationalTableModel();
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->setTable("content");
        model->setRelation(0, QSqlRelation("type", "typeID", "typeName"));
        model->select();

        ui->tableView->setModel(model);
        ui->tableView->setItemDelegate(new QSqlRelationalDelegate());
    }
    this->on_addButton_clicked();
}

QString momPrinter::toChineseNum(int num){
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
        else if ((length - i) == 5 && this->isMust5(sourceString))// 万(不必填)
          key = chineseUnits[4];
        else if ((length - i) == 1)// 元(必填)
          key = chineseUnits[0];
        // 0遇非0时补零，不包含最后一位
        if ((length - i) > 1 && integers[i + 1] != 0)
          key += chineseNumbers[0];
      }
      chineseInteger.append(integers[i] == 0 ? key : (chineseNumbers[integers[i]] + chineseUnits[length - i - 1]));
    }
    return chineseInteger.join("");
}

bool momPrinter::isMust5(QString integerStr) {
  int length = integerStr.size();
  if (length > 4) {
    QString subInteger = "";
    if (length > 8) {
      // 取得从低位数，第5到第8位的字串
      subInteger = integerStr.mid(length - 5, 4);
    } else {
      subInteger = integerStr.mid(0, 4);
    }
    return subInteger.toInt() > 0;
  } else {
    return false;
  }
}

momPrinter::~momPrinter()
{
    delete ui;
    delete model;
    db.close();
}


void momPrinter::on_commitButton_clicked()
{
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
    } else {
        model->database().rollback();
        QMessageBox::warning( this, "Commit Error", model->lastError().text());
    }
    //update the sum
    QSharedPointer<QSqlQuery> query = (QSharedPointer<QSqlQuery>) new QSqlQuery();
    query->exec("SELECT num, type.typePrice FROM content JOIN type ON type.typeID = content.typeID");
    int index_num = query->record().indexOf("num");
    int index_typePrice = query->record().indexOf("typePrice");
    int sum = 0;
    while (query->next()) {
        sum += query->value(index_num).toInt() * query->value(index_typePrice).toInt();
    }
    ui->labelResult->setText(QString::number(sum) + ".00");
}

void momPrinter::on_cancelButton_clicked()
{
    model->revertAll();
}

void momPrinter::on_addButton_clicked()
{
    model->insertRow(model->rowCount()); //index of the new row is equal to the rowCount
}

void momPrinter::on_deleteButton_clicked()
{
    model->removeRow(ui->tableView->currentIndex().row());
    this->on_commitButton_clicked();
}

void momPrinter::on_printButton_clicked()
{
    //commit first
    this->on_commitButton_clicked();
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
    font.setPointSize(9);
    painter->setFont(font);
    //prepare draw
    const int widthPixel = printer->pageRect().width();
    const int heightPixel = widthPixel * HEIGHT / WIDTH;

    //put the static content into the target
    QVector<content> target(0);
    target.append(content(ID_X, ID_Y, ui->idEdit->text()));
    target.append(content(ID_X + CELL_X * 2, ID_Y, ui->nameEdit->text()));
    target.append(content(DAY_X + CELL_X, DATE_Y, "直属一分局"));
    target.append(content(CELL_X * 1 + TABLE_X, TABLE_Y, "借款合同印花税"));
    QDate date = QDate::currentDate ();
    target.append(content(YEAR_X, DATE_Y, QString::number(date.year())));
    target.append(content(MONTH_X, DATE_Y, QString::number(date.month())));
    target.append(content(DAY_X, DATE_Y, QString::number(date.day())));
    //put sql result into the target
    QSharedPointer<QSqlQuery> query = (QSharedPointer<QSqlQuery>) new QSqlQuery();
    query->exec("SELECT type.typeName, num, type.typePrice FROM content JOIN type ON type.typeID = content.typeID");
    int index_typeName = query->record().indexOf("typeName");
    int index_num = query->record().indexOf("num");
    int index_typePrice = query->record().indexOf("typePrice");
    int currentRow = 0;
    int sum = 0;
    while (query->next()) {
        int price = query->value(index_num).toInt() * query->value(index_typePrice).toInt();
        sum += price;
        target.append(content(CELL_X * 0 + TABLE_X, ROW_Y * currentRow + TABLE_Y, query->value(index_typeName).toString()));
        target.append(content(CELL_X * 2 + TABLE_X, ROW_Y * currentRow + TABLE_Y, query->value(index_num).toString()));
        target.append(content(CELL_X * 3 + TABLE_X, ROW_Y * currentRow + TABLE_Y, QString::number(price) + ".00"));
        currentRow++;
    }
    //add the sum
    target.append(content(SUM_ZH_X, SUM_ZH_Y, this->toChineseNum(sum)));
    target.append(content(CELL_X * 3 + TABLE_X, SUM_ZH_Y, QString::number(sum) + ".00"));
    //start paint
    QVectorIterator<content> i(target);
    while (i.hasNext()){
        content cache = i.next();
        painter->drawText(widthPixel * cache.x / WIDTH, heightPixel * cache.y / HEIGHT, cache.text);
    }
    painter->end();
}

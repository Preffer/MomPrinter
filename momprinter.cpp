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
        query->exec("CREATE TABLE content (typeID INT PRIMARY KEY, name VARCHAR, num INT)");
        query->exec("CREATE TABLE type(typeID INT PRIMARY KEY, typeName VARCHAR)");
        //insert values
        query->exec("INSERT INTO type VALUES (1, '壹角票')");
        query->exec("INSERT INTO type VALUES (2, '贰角票')");
        query->exec("INSERT INTO type VALUES (3, '伍角票')");
        query->exec("INSERT INTO type VALUES (4, '壹元票')");
        model = new QSqlRelationalTableModel();
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->setTable("content");
        model->setRelation(0, QSqlRelation("type", "typeID", "typeName"));
        model->select();

        ui->tableView->setModel(model);
        ui->tableView->setItemDelegate(new QSqlRelationalDelegate());
    }
}

momPrinter::~momPrinter()
{
    delete ui;
    delete model;
    db.close();
}

void momPrinter::on_printButton_clicked()
{
    //init a printer
    QSharedPointer<QPrinter> printer = (QSharedPointer<QPrinter>) new QPrinter(QPrinter::HighResolution);
   // QPrinter* printer = new QPrinter(QPrinter::HighResolution);
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

    //put the content into the target
    QVector<content> target(0);
    target.append(content(ID_X, ID_Y, "330682146102035"));
    target.append(content(YEAR_X, DATE_Y, "2014"));
    target.append(content(MONTH_X, DATE_Y, "5"));
    target.append(content(DAY_X, DATE_Y, "15"));

    //start paint
    QVectorIterator<content> i(target);
    while (i.hasNext()){
        content cache = i.next();
        painter->drawText(widthPixel * cache.x / WIDTH, heightPixel * cache.y / HEIGHT, cache.text);
    }
    painter->end();
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
    int confirm = QMessageBox::warning(this, "Delete Current Row", "Are you sure!!\nDelete current row?", QMessageBox::Yes,QMessageBox::No);
    if(confirm == QMessageBox::No)
    {
        model->revertAll();
    }
    else model->submitAll();
}

#include "table.h"


Table::Table(QWidget *parent) :
    QMainWindow(parent)
{
    //tablewidgit = new QTableWidget;
    Table::text = new QTextEdit;
    setCentralWidget(text);
}

QTableWidget* Table::tablewidgit = NULL;
QTextEdit* Table::text = NULL;

Table::~Table()
{

}

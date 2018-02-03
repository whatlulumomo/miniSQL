#ifndef TABLE_H
#define TABLE_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>

class Table : public QMainWindow
{
    Q_OBJECT

public:
    Table(QWidget *parent = 0);
    ~Table();
    static QTextEdit * text;
    static QTableWidget* tablewidgit;

};


#endif // TABLE_H

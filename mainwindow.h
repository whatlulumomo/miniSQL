#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static QTextEdit * textedit;
    static QTableWidget* tablewidgit;

private slots:
    int sqlmain();
    void clean();
    void openfile();
    void savefile();
};

#endif // MAINWINDOW_H

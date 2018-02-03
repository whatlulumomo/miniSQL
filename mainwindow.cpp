#include "mainwindow.h"
#include <QTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QIcon>
#include <QTextEdit>
#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include "table.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenuBar *menubar = menuBar();
    QMenu *file = menubar->addMenu("&文件");
    QMenu *help = menubar->addMenu("&帮助");

    QAction *openAction = new QAction(QIcon(":/Action/folder.png"),"&打开", this);
    file->addAction(openAction);
    QAction *saveAction = new QAction(QIcon(":/Action/save_as.png"),"&保存", this);
    file->addAction(saveAction);
    QAction *helpaction = new QAction("&帮助");
    help->addAction(helpaction);

    QAction *AssemblytoMachincecode_runaction = new QAction(QIcon(":/Action/next.png"),"&执行SQL", this);
    file->addAction(AssemblytoMachincecode_runaction);
    QAction *MachincecodetoAssembly_runaction = new QAction(QIcon(":/Action/previous.png"),"&清除窗口", this);
    file->addAction(MachincecodetoAssembly_runaction);

    QToolBar *toolBar = addToolBar("Toolbar");
    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(AssemblytoMachincecode_runaction);
    toolBar->addAction(MachincecodetoAssembly_runaction);

    textedit = new QTextEdit(this);
    setCentralWidget(textedit);
    textedit->setFont(QFont("宋体",14));

    Table *a = new Table;
    a->setWindowTitle("console");
   // a->resize(640,300);
    a->setGeometry(50,50,640,300);
    a->show();

    // 触发动作的槽函数
    connect(openAction, SIGNAL(triggered()), this, SLOT(openfile()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(savefile()));
    //connect(AssemblytoMachincecode_runaction, SIGNAL(triggered()), this, SLOT(AssemblytoMachinecode()));
    connect(AssemblytoMachincecode_runaction, SIGNAL(triggered()), this, SLOT(sqlmain()));
    connect(MachincecodetoAssembly_runaction, SIGNAL(triggered()), this, SLOT(clean()));
}

QTextEdit * MainWindow::textedit = NULL;
QTableWidget * MainWindow::tablewidgit = NULL;

void MainWindow::clean()
{
    textedit->clear();
}

MainWindow::~MainWindow()
{

}

void MainWindow::openfile()
{
    QString path = QFileDialog::getOpenFileName(
                this,   //父窗口
                tr("Open File"),    //对话框名字
                ".",    //默认目录
                tr("Text Files(*.txt);;ini File(*.ini)") //过滤器 只打开txt类型 或 ini 使用;;分隔
                );
    if (!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return; //打开失败

        QTextStream in(&file);
        textedit->setText(in.readAll());

        file.close();
    }
}

void MainWindow::savefile()
{
    QString path = QFileDialog::getSaveFileName(
                this,   //父窗口
                tr("Open File"),    //对话框名字
                ".",    //默认目录
                tr("Text Files(*.txt);;ini File(*.ini)") //过滤器 只打开txt类型 或 ini 使用;;分隔
                );
    if (!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return; //打开失败

        QTextStream out(&file);
        out << textedit->toPlainText();
        out << "\n";
        out << Table::text->toPlainText();

        file.close();
    }
}

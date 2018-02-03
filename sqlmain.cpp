#include <iostream>
#include "Interpreter.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mainwindow.h"
#include <QTableWidget>
#include "table.h"
#include <QString>
#include <QStringList>

#include "API.h"
#pragma warning(disable:4996)

clock_t start;

void init()
{
    FILE *fp;
    fp = fopen("Indexs", "r");
    if (fp == NULL)
    {
        fp = fopen("Indexs", "w+");
        return;
    }
    fclose(fp);
}

void print()
{
    clock_t finish = clock();
    double duration = (double)(finish - start) / CLOCKS_PER_SEC;
    duration *= 1000;
    printf("now time is %2.1f milliseconds\n", duration * 1000);
}

int MainWindow::sqlmain()
{
        API api;
        CatalogManager cm;
        RecordManager rm;

        api.Record_Manager_Pointer = &rm;
        api.Catalog_Manager_Pointer = &cm;
        IndexManager im(&api);

        api.Index_Manager_Pointer = &im;
        rm.api = &api;
        string filename;
        int i;
        Interpreter myinterpreter;
        myinterpreter.myapi = &api;
        //while (cin) {
            //cout << "miniSQL>>";
            //string s = myinterpreter.ReadInput();
        QString tmpin = MainWindow::textedit->toPlainText();
        //string s = tmpin.toStdString();
        tmpin.replace(";"," ; !").replace("\n","").replace(","," , ").replace("("," ( ").replace(")"," ) ").replace("*"," * ");
        QStringList SQLlist = tmpin.split("!");
        int count = SQLlist.size()-1;
        for(int j=0;j< count;j++)
        {
            string s = SQLlist[j].toStdString();
            for( int k = 0;k<s.size();k++)
            {
                if( s[k] != ' ')
                {
                    s.erase(0,k);
                    break;
                }
            }
            i = myinterpreter.RunSQL(s, filename);
            if (i == -1)
            {
                ifstream fin;
                cout << " filename = " << filename << endl;
                string temp =  " filename = ";
                temp = temp + filename;
                Table::text->append(QString::fromStdString(temp));
                fin.open(filename.c_str());
                int num = 0;
                while (!fin.eof())
                {
                    s = myinterpreter.ReadInput(fin);
                    if (s == "")
                        break;
                    i = myinterpreter.RunSQL(s, filename);
                    if (i == -1) {
                        cout << "Fail to execute file" << endl;
                        Table::text->append("Fail to execute file");
                        return 0;
                    }
                    printf("%d sentences executed \n", ++num);
                    string temp = to_string(num);
                    temp +=  " sentences executed ";
                    Table::text->append(QString::fromStdString(temp));
                }
            }
            else if (i == QUITSIGNAL)
            {
                return 0;
            }
        }
}

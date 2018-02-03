//
// Created by Administrator on 2017/6/1 0001.
//

#ifndef MINISQL_A_INTERPRETER_H
#define MINISQL_A_INTERPRETER_H

#include "API.h"
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdio.h>
#define QUITSIGNAL 100000

using namespace std;

class Interpreter
{
public:
	Interpreter();
	~Interpreter();
	API* myapi;
	string filename;
	int RunSQL(string SQL, string& filename);
	string ReadInput();
	string ReadInput(ifstream& fin);
	string process(string& SQL);
	string getword(string& SQL);
	string changestring(string str);
};

#endif //MINISQL_A_INTERPRETER_H

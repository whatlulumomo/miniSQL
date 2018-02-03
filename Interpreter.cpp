//
// Created by Administrator on 2017/6/1 0001.
//

#include "Interpreter.h"
#include "JudgeCondition.h"
#include "Attribute.h"
#include <string>
#include <string.h>
#include <iostream>
#include <QString>
#include <mainwindow.h>
#include "table.h"
class SyntaxException
{

};

Interpreter::Interpreter()
{

}

Interpreter::~Interpreter()
{

}

string Interpreter::ReadInput()
{
	string temp;
	string sql;
    cin >> temp;
	sql = sql + temp;
	sql = sql + " ";
	while (temp.find(";") == temp.npos)
	{
		cin >> temp;
		sql = sql + temp;
		sql = sql + " ";
	}
	return sql;
}

string Interpreter::ReadInput(ifstream &fin)
{
	string temp;
	string sql;
	while (temp.find(";") == temp.npos)
	{
		fin >> temp;
		sql = sql + temp;
		sql = sql + " ";
	}
	return sql;
}

string Interpreter::getword(string &SQL)
{
	string word;
	for (int i = 0; i<SQL.length(); i++)
	{
		if (SQL[i] != ' ')
		{
			word = word + SQL[i];
		}
		else
		{
			while (SQL[i] == ' '&&i + 1<SQL.length())
			{
				i++;
			}
			SQL.erase(0, i);
			break;
		}
	}
	return word;
}

string Interpreter::process(string &SQL)
{
	string sql_exe;
	for (int i = 0; i<SQL.length(); i++)
	{
		if (SQL[i] == ',')
		{
			sql_exe = sql_exe + " , ";
		}
		else if (SQL[i] == '(')
		{
			sql_exe = sql_exe + " ( ";
		}
		else if (SQL[i] == ')')
		{
			sql_exe = sql_exe + " ) ";
		}
		else if (SQL[i] == ';')
		{
			sql_exe = sql_exe + " ; ";
		}
		else if (SQL[i] == '*')
		{
			sql_exe = sql_exe + " * ";
		}
		else if (SQL[i] == '<')
		{
			if (SQL[i + 1] == '=')
			{
				sql_exe = sql_exe + " <= ";
				i++;
			}
			else
			{
				sql_exe = sql_exe + " < ";
			}
		}
		else if (SQL[i] == '>')
		{
			if (SQL[i + 1] == '=')
			{
				sql_exe = sql_exe + " >= ";
				i++;
			}
			else
			{
				sql_exe = sql_exe + " > ";
			}
		}
		else
		{
			sql_exe = sql_exe + SQL[i];
		}
	}
	return sql_exe;
}

string Interpreter::changestring(string str)
{
	for (int i = 0; i<str.length(); i++)
	{
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] = str[i];
		}
		else if (str[i] >= 'A' && str[i] <= 'Z')
		{
			str[i] = str[i] - 'A' + 'a';
		}
	}
	return str;
}

int Interpreter::RunSQL(string SQL, string &filename)
{
	string operation;
	string s1, s2, s3, s4;
	SQL = process(SQL);
	operation = getword(SQL);
	operation = changestring(operation);
	if (operation == "create")
	{
		s1 = getword(SQL);
		s1 = changestring(s1);
		if (s1 == "table")
		{
			string tablename = "";
			string primarykey = "";
			vector<Attribute> attr;
			string attributename;
			int type;
			int ifUnique;
			int primarykeyLocation = 0;
			s1 = getword(SQL);
			tablename = s1;
			s1 = getword(SQL);
			if (s1.compare("(") != 0)
			{
				cout << "Expected '(' after 'table'" << endl;
                Table::text->append("Expected '(' after 'table'");
				return 1; 
			}
			s1 = getword(SQL);
			s1 = changestring(s1);
			while (s1.compare(")") != 0)
			{
				if (s1.compare("primary") == 0)
				{
					s1 = getword(SQL);
					s1 = changestring(s1);
					if (s1.compare("key") != 0)
					{
						cout << "Expected 'key'" << endl;
                        Table::text->append("Expected 'key'");
						return 1;
					}
					s1 = getword(SQL);
					if (s1.compare("(") != 0)
					{
						cout << "Expected '(' after 'primary key'" << endl;
                        Table::text->append("Expected '(' after 'primary key'");
						return 1;
					}
					s1 = getword(SQL);
					primarykey = s1;
					for (int i = 0; i<attr.size(); i++)
					{
						if (attr.at(i).name.compare(primarykey) == 0)
						{
							attr.at(i).ifUnique = true;
							primarykeyLocation = i;
							break;
						}
					}
					s1 = getword(SQL);
					if (s1.compare(")") != 0) {
						cout << "Expected ')'" << endl;
                        Table::text->append("Expected ')'");
						return 1;
					}
					s1 = getword(SQL);
					break;
				}
				attributename = s1;
				type = 0;
				ifUnique = false;
				s1 = getword(SQL);
				s1 = changestring(s1);
				if (s1.compare("int") == 0) 
				{
					type = 0;
				}
				else if (s1.compare("float") == 0) 
				{
					type = -1;
				}
				else if (s1.compare("char") == 0)
				{
					s1 = getword(SQL);
					if (s1.compare("(") != 0)
					{
						cout << "Do you input the number of char?" << endl;
                        Table::text->append("Do you input the number of char?");
						return 1;
					}
					s1 = getword(SQL);
					type = atoi(s1.c_str());
					s1 = getword(SQL);
					if (s1.compare(")") != 0)
					{
						cout << "Expected')'" << endl;
                        Table::text->append("Expected')'");
						return 1;
					}
				}
				else
				{
					return 2;
				}
				s1 = getword(SQL);
				s1 = changestring(s1);
				if (s1.compare("unique") == 0)
				{
					ifUnique = true;
				}
				Attribute attribute(attributename, type, ifUnique);
				attr.push_back(attribute);
				if (s1.compare(")") == 0)
				{
					break;
				}
				s1 = getword(SQL);
			}
			s1 = getword(SQL);
			if (s1.compare(";") != 0)
			{
				cout << "Expected';'" << endl;
                Table::text->append("Expected';'");
				return 1;
			}
			myapi->tableCreate(tablename, &attr, primarykey, primarykeyLocation);
			//cout << "create table successfully!!!" << endl;
		}
		else if (s1.compare("index") == 0)
		{
			string index_name = "";
			string table_name = "";
			string attribute_name = "";
			s1 = getword(SQL);
			index_name = s1;
			s1 = getword(SQL);
			s1 = changestring(s1);
			if (s1.compare("on") != 0)
			{
				cout << "Expected'on'" << endl;
                Table::text->append("Expected'on'");
				return 1;
			}
			s1 = getword(SQL);
			table_name = s1;
			s1 = getword(SQL);
			if (s1.compare("(") != 0)
			{
				cout << "Expected'('" << endl;
                Table::text->append("Expected'('");
				return 1;
			}
			s1 = getword(SQL);
			attribute_name = s1;
			s1 = getword(SQL);
			if (s1.compare(")") != 0)
			{
				cout << "Expected')'" << endl;
                Table::text->append("Expected')'");
				return 1;
			}
			s1 = getword(SQL);
			if (s1.compare(";") != 0)
			{
				cout << "Expected';'" << endl;
                Table::text->append("Expected';'");
				return 1;
			}
			myapi->indexCreate(index_name, table_name, attribute_name);
			return 1; 
		}
	}

	else if (operation == "drop")
	{
		string tablename = "";
		string indexname = "";
		s1 = getword(SQL);
		s1 = changestring(s1);
		if (s1.compare("table") == 0)
		{
			s1 = getword(SQL);
			tablename = s1;
		}
		else if (s1.compare("index") == 0)
		{
			s1 = getword(SQL);
			indexname = s1;
		}
		s1 = getword(SQL);
		if (s1.compare(";") != 0)
		{
			cout << "Expected';'" << endl;
            Table::text->append("Expected';'");
			return 1;
		}
		if (tablename != "")
		{
			myapi->tableDrop(tablename);
			return 1;
		}
		else if (indexname != "")
		{
			myapi->indexDrop(indexname);
			return 1;
		}
	}

	else if (operation == "select")
	{
		vector<string> attrselected;
		string tablename = "";
		s1 = getword(SQL);
		if (s1.compare("*") != 0)
        {
			cout << "Expected '*'" << endl;
            Table::text->append("Expected '*'" );
			return 1;
		}
		s1 = getword(SQL);
		s1 = changestring(s1);
		if (s1.compare("from") != 0)
		{
			cout << "Expected 'from'" << endl;
            Table::text->append("Expected 'from'");
			return 1;
		}
		s1 = getword(SQL);
		tablename = s1;
		s1 = getword(SQL);
		s1 = changestring(s1);
		if (s1.compare("where") == 0)
		{                              
			s1 = getword(SQL);
			vector<Condition> conds;
			while (s1.compare(";") != 0)
			{                   
				string attributename = s1;
				string value = "";
				int operate;
				s2 = getword(SQL);  
				if (s2.compare("=") == 0)
				{
					operate = Condition::OPERATOR_EQUAL;
				}
				else if (s2.compare("<>") == 0)
				{
					operate = Condition::OPERATOR_NOT_EQUAL;
				}
				else if (s2.compare("<") == 0)
				{
					operate = Condition::OPERATOR_LESS;
				}
				else if (s2.compare(">") == 0)
				{
					operate = Condition::OPERATOR_MORE;
				}
				else if (s2.compare("<=") == 0)
				{
					operate = Condition::OPERATOR_LESS_EQUAL;
				}
				else if (s2.compare(">=") == 0)
				{
					operate = Condition::OPERATOR_MORE_EQUAL;
				}
				else
				{
					cout << "Not support this kind of op" << endl;
                    Table::text->append("Not support this kind of op");
					return 1;
				}
				s3 = getword(SQL);
				value = s3;
				Condition condition(attributename, value, operate);
				conds.push_back(condition);
				s1 = getword(SQL);
				if (s1.compare(";") == 0)
				{
					break;
				}
				s1 = getword(SQL);
			}
			myapi->recordShow(tablename, NULL, &conds);
			return 0;
		}
		else if (s1.compare(";") == 0)
		{
			myapi->recordShow(tablename, NULL);
			return 0;
		}
	}

	else if (operation == "insert")
	{
		string tablename = "";
		vector<string> values;
		s1 = getword(SQL);
		s1 = changestring(s1);
		if (s1.compare("into") != 0)
		{
			cout << "Expected 'into'" << endl;
            Table::text->append("Expected 'into'");
			return 1;
		}
		s1 = getword(SQL);
		tablename = s1;
		s1 = getword(SQL);
		s1 = changestring(s1);
		if (s1.compare("values") != 0)
		{
			cout << "Expected 'values'" << endl;
            Table::text->append("Expected 'values'");
			return 1;
		}
		s1 = getword(SQL);
		if (s1.compare("(") != 0)
		{
			cout << "Expected '('" << endl;
            Table::text->append("Expected '('");
			return 1;
		}
		//s1 = getword(SQL);
		while (s1.compare(")") != 0)
		{
			s1 = getword(SQL);
			values.push_back(s1);
			s1 = getword(SQL);
			if (s1.compare(")") == 0)
			{
				break;
			}
			else if (s1.compare(",") != 0)
			{
				cout << "Expected ','" << endl;
                Table::text->append("Expected ','");
				return 1;
			}
		}
		myapi->recordInsert(tablename, &values);
		return 0;
	}

	else if (operation == "delete")
	{
		string tablename;
		vector<Condition> cond;
		s1 = getword(SQL);
		s1 = changestring(s1);
		if (s1.compare("from") != 0)
		{
			cout << "Expected 'from'" << endl;
            Table::text->append("Expected 'from'");
			return 1;
		}
		s1 = getword(SQL);
		tablename = s1;
		s1 = getword(SQL);
		s1 = changestring(s1);
		if (s1.compare("where") == 0)
		{
			string attributename = "";
			string value = "";
			int operate;
			vector<Condition> conds;
			s1 = getword(SQL);
			s2 = getword(SQL);
			s3 = getword(SQL);
			attributename = s1;
			value = s3;
			if (s2.compare("=") == 0)
			{
				operate = Condition::OPERATOR_EQUAL;
			}
			else if (s2.compare("<>") == 0)
			{
				operate = Condition::OPERATOR_NOT_EQUAL;
			}
			else if (s2.compare("<") == 0)
			{
				operate = Condition::OPERATOR_LESS;
			}
			else if (s2.compare(">") == 0)
			{
				operate = Condition::OPERATOR_MORE;
			}
			else if (s2.compare("<=") == 0)
			{
				operate = Condition::OPERATOR_LESS_EQUAL;
			}
			else if (s2.compare(">=") == 0)
			{
				operate = Condition::OPERATOR_MORE_EQUAL;
			}
			else
			{
				cout << "Not support this kind of op!" << endl;
                Table::text->append("Not support this kind of op!");
				return 1;
			}
			s1 = getword(SQL);
			if (s1.compare(";") != 0)
			{
				cout << "Expected ';'" << endl;
                Table::text->append("Expected ';'");
				return 1;
			}
			Condition condition(attributename, value, operate);
			conds.push_back(condition);
			myapi->recordDelete(tablename, &conds);
		}
		else if (s1.compare(";") == 0)
		{
			myapi->recordDelete(tablename);
			return 0;
		}
		else
		{
			cout << "Expected where or ';'" << endl;
            Table::text->append("Expected where or ';'");
			return 1;
		}
		return 0;
	}

	else if (operation == "quit")
	{
        Table::text->append("Quit");
		return QUITSIGNAL;
	}

	else if (operation == "execfile")
	{
		s1 = getword(SQL);
		filename = s1;   
		s1 = getword(SQL);
		if (s1.compare(";") != 0)
		{
			cout << "Expected ';'" << endl;
            Table::text->append("Expected ';'");
			return 1;
		}
		return -1; 
	}

	else
	{
		cout << "Can't support this kind of SQL sentence ! " << endl;
        Table::text->append("Can't support this kind of SQL sentence ! ");
		return 0;
	}
}

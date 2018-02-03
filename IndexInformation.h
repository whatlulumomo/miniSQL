#ifndef _INDEXINFORMATION_H
#define _INDEXINFORMATION_H

#include <string>
using namespace std;

class IndexInformation
{
public:
	IndexInformation(string i,string t,string a,int indextype)
    {indexName = i;tableName = t;Attribute = a;type = indextype;}
    string indexName;
    string tableName;
    string Attribute;
    int type;
};

#endif
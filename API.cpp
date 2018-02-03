//
// Created by Administrator on 2017/6/1 0001.
//

#include "API.h"
#include "RecordManager.h"
#include "CatalogManager.h"
#include "IndexManager.h"

#define UNKNOWN_FILE 8
#define TABLE_FILE 9
#define INDEX_FILE 10

CatalogManager *cm;
IndexManager* im;

void API::tableDrop(string tableName)
{
	if (!tableExist(tableName))
	{
		return;
	}

	vector<string> indexNameVector;

	indexNameListGet(tableName, &indexNameVector);
	for (int i = 0; i < indexNameVector.size(); i++)
	{
		printf("%s", indexNameVector[i].c_str());
        Table::text->append(QString::fromStdString(indexNameVector[i]));
        indexDrop(indexNameVector[i]);
	}

	if (Record_Manager_Pointer->tableDrop(tableName))
	{
		Catalog_Manager_Pointer->dropTable(tableName);
		printf("Drop table %s successfully\n", tableName.c_str());
        string temp = "Drop table ";
        temp += tableName;
        temp += " successfully";
        Table::text->append(QString::fromStdString(temp));
	}
}

void API::indexDrop(string indexName)
{
	if (Catalog_Manager_Pointer->findIndex(indexName) != INDEX_FILE)
	{
		printf("There is no index %s \n", indexName.c_str());
        string temp = "There is no index ";
        temp += indexName;
        Table::text->append(QString::fromStdString(temp));
		return;
	}
	if (Record_Manager_Pointer->indexDrop(indexName))
	{

		//get type of index
		int indexType = Catalog_Manager_Pointer->getIndexType(indexName);
		if (indexType == -2)
		{
			printf("ERROR\n");
            Table::text->append("ERROR");
			return;
		}
		Catalog_Manager_Pointer->dropIndex(indexName);
		Index_Manager_Pointer->dropIndex(Record_Manager_Pointer->indexFileNameGet(indexName), indexType);
		printf("Drop index %s successfully\n", indexName.c_str());
        string temp ="Drop index ";
        temp += indexName;
        temp += " successfully";
        Table::text->append(QString::fromStdString(temp));
	}
}

void API::indexCreate(string indexName, string tableName, string attributeName)
{
	if (Catalog_Manager_Pointer->findIndex(indexName) == INDEX_FILE)
	{
		cout << "There is index " << indexName << " already" << endl;
        string temp = "There is index ";
        temp +=  indexName;
        temp += " already";
        Table::text->append(QString::fromStdString(temp));
		return;
	}

	if (!tableExist(tableName)) return;

	vector<Attribute> attributeVector;
	Catalog_Manager_Pointer->attributeGet(tableName, &attributeVector);
	int i;
	int type = 0;
	for (i = 0; i < attributeVector.size(); i++)
	{
		if (attributeName == attributeVector[i].name)
		{
			if (!attributeVector[i].ifUnique)
			{
				cout << "the attribute is not unique" << endl;
                Table::text->append("the attribute is not unique");

				return;
			}
			type = attributeVector[i].type;
			break;
		}
	}

	if (i == attributeVector.size())
	{
		cout << "there is not this attribute in the table" << endl;
        Table::text->append("there is not this attribute in the table");
		return;
	}
	if (Record_Manager_Pointer->indexCreate(indexName))
	{
		Catalog_Manager_Pointer->addIndex(indexName, tableName, attributeName, type);
		// add a new index information
		int indexType = Catalog_Manager_Pointer->getIndexType(indexName);
		// get the type of index
		if (indexType == -2)
		{
			cout << "ERROR";
            Table::text->append("ERROR");
			return;
		}
		// index type is not normal type
		Index_Manager_Pointer->createIndex(Record_Manager_Pointer->indexFileNameGet(indexName), indexType);
		// create a index tree
		Record_Manager_Pointer->indexRecordAllAlreadyInsert(tableName, indexName);
		// insert all already record into index
		printf("Create index %s successfully\n", indexName.c_str());
        string temp = "Create index ";
        temp +=  indexName;
        temp += " successfully";
        Table::text->append(QString::fromStdString(temp));
	}
	else
	{
		cout << "Create index " << indexName << " fail" << endl;
	}
}

void API::tableCreate(string tableName, vector<Attribute>* attributeVector, string primaryKeyName, int primaryKeyLocation)
{
	if (Catalog_Manager_Pointer->findTable(tableName) == TABLE_FILE)
	{
		cout << "There is a table " << tableName << " already" << endl;
        string temp ="There is a table ";
        temp += tableName;
        temp += " already";
        Table::text->append(QString::fromStdString( temp));
		return;
	}

	// create a table file
	if (Record_Manager_Pointer->tableCreate(tableName))
	{
		// create a table information
		Catalog_Manager_Pointer->addTable(tableName, attributeVector, primaryKeyName, primaryKeyLocation);

		printf("Create table %s successfully\n", tableName.c_str());
        string temp = "Create table ";
        temp += tableName;
        temp += " successfully";
        Table::text->append(QString::fromStdString(temp));
	}

	if (primaryKeyName != "")
	{
		// get a primary key
		string indexName = primaryIndexNameGet(tableName);
		indexCreate(indexName, tableName, primaryKeyName);
	}
}

void API::recordShow(string tableName, vector<string>* attributeNameVector)
{
	vector<Condition> conditionVector;
	//recordShow(tableName, attributeNameVector, &conditionVector);
	recordShow(tableName, attributeNameVector, NULL);
}

void API::recordShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector)
{
	if (Catalog_Manager_Pointer->findTable(tableName) == TABLE_FILE)
	{
		int num = 0;
		vector<Attribute> attributeVector;
		attributeGet(tableName, &attributeVector);

		vector<string> allAttributeName;
		if (attributeNameVector == NULL) 
		{
			for (Attribute attribute : attributeVector)
			{
				allAttributeName.insert(allAttributeName.end(), attribute.name);
			}

			attributeNameVector = &allAttributeName;
		}
		tableAttributePrint(attributeNameVector);
		// print attribute name
		for (string name : *attributeNameVector)
		{
			int i = 0;
			for (i = 0; i < attributeVector.size(); i++)
			{
				if (attributeVector[i].name == name)
				{
					break;
				}
			}

			if (i == attributeVector.size())
			{
				cout << "the attribute which you want to print is not exist in the table" << endl;
                Table::text->append("the attribute which you want to print is not exist in the table");
				return;
			}
		}
		int blockOffset = -1; // initial block offset
		if (blockOffset == -1)
		{
			num = Record_Manager_Pointer->recordAllShow(tableName, attributeNameVector, conditionVector);
		}
		else
		{
			num = Record_Manager_Pointer->recordBlockShow(tableName, attributeNameVector, conditionVector, blockOffset);
			// find the block by index,search in the block
		}

		printf("%d records selected\n", num);
        string temp = to_string(num);
        temp += " records selected";
        Table::text->append(QString::fromStdString(temp));
	}
	else
	{
		cout << "There is no table " << tableName << endl;
        string temp = "There is no table ";
        temp += tableName;
        Table::text->append(QString::fromStdString(temp));
	}
}

void API::recordInsert(string tableName, vector<string>* recordContent)
{
	if (!tableExist(tableName))
	{
		return;
	}

	string indexName;
	vector<Attribute> attributeVector;
	vector<Condition> conditionVector;
	attributeGet(tableName, &attributeVector);
	for (int i = 0; i < attributeVector.size(); i++)
	{
		indexName = attributeVector[i].index;
		if (indexName != "")
		{
			int blockoffest = Index_Manager_Pointer->searchIndex(Record_Manager_Pointer->indexFileNameGet(indexName), (*recordContent)[i], attributeVector[i].type);

			if (blockoffest != -1)
			{
				cout << "insert fail because index value exist" << endl;
                Table::text->append("insert fail because index value exist");
				return;
			}
		}
		else if (attributeVector[i].ifUnique)
		{
			Condition condition(attributeVector[i].name, (*recordContent)[i], Condition::OPERATOR_EQUAL);
			conditionVector.insert(conditionVector.end(), condition);
		}
	}

	if (conditionVector.size() > 0)
	{
		for (int i = 0; i < conditionVector.size(); i++)
		{
			vector<Condition> conditionTmp;
			conditionTmp.insert(conditionTmp.begin(), conditionVector[i]);

			int recordConflictNum = Record_Manager_Pointer->recordAllFind(tableName, &conditionTmp);
			if (recordConflictNum > 0)
			{
				cout << "insert fail because unique value exist" << endl;
                Table::text->append("insert fail because unique value exist");
				return;
			}

		}
	}
	char recordString[2000];
	memset(recordString, 0, 2000);
	// get the record string
	Catalog_Manager_Pointer->recordStringGet(tableName, recordContent, recordString);
	// insert the record into file; and get the position of block being insert
	int recordSize = Catalog_Manager_Pointer->calcuteLenth(tableName);
	int blockOffset = Record_Manager_Pointer->recordInsert(tableName, recordString, recordSize);

	if (blockOffset >= 0)
	{
		recordIndexInsert(recordString, recordSize, &attributeVector, blockOffset);
		Catalog_Manager_Pointer->insertRecord(tableName, 1);
		printf("insert record into table %s successful\n", tableName.c_str());
        string temp = "insert record into table ";
        temp += tableName;
        temp += " successful";
        Table::text->append(QString::fromStdString(temp));
	}
	else
	{
		cout << "insert record into table " << tableName << " fail" << endl;
        string temp = "insert record into table ";
        temp += tableName;
        temp += " fail";
        Table::text->append(QString::fromStdString(temp));
	}
}

void API::recordDelete(string tableName)
{
	vector<Condition> conditionVector;
	recordDelete(tableName, &conditionVector);
}

void API::recordDelete(string tableName, vector<Condition>* conditionVector)
{
	if (!tableExist(tableName)) return;

	int num = 0;
	vector<Attribute> attributeVector;
	attributeGet(tableName, &attributeVector);

	int blockOffset = -1;
	if (conditionVector != NULL)
	{
		for (Condition condition : *conditionVector)
		{
			if (condition.operate == Condition::OPERATOR_EQUAL)
			{
				for (Attribute attribute : attributeVector)
				{
					if (attribute.index != "" && attribute.name == condition.attributeName)
					{
						blockOffset = Index_Manager_Pointer->searchIndex(Record_Manager_Pointer->indexFileNameGet(attribute.index), condition.value, attribute.type);
					}
				}
			}
		}
	}


	if (blockOffset == -1)
	{
		//if we con't find the block by index,we need to find all block
		num = Record_Manager_Pointer->recordAllDelete(tableName, conditionVector);
	}
	else
	{
		//find the block by index,search in the block
		num = Record_Manager_Pointer->recordBlockDelete(tableName, conditionVector, blockOffset);
	}

	//delete the number of record in in the table
	Catalog_Manager_Pointer->deleteValue(tableName, num);
	printf("delete %d record in table %s\n", num, tableName.c_str());
    string temp = "delete ";
    temp += to_string(num);
    temp += " record in table ";
    temp += tableName;
    Table::text->append(QString::fromStdString(temp));
}

int API::recordNumGet(string tableName)
{
	if (!tableExist(tableName)) return 0;

	return Catalog_Manager_Pointer->getRecordNum(tableName);
}

int API::recordSizeGet(string tableName)
{
	if (!tableExist(tableName)) return 0;

	return Catalog_Manager_Pointer->calcuteLenth(tableName);
}

int API::typeSizeGet(int type)
{
	return Catalog_Manager_Pointer->calcuteLenth2(type);
}

int API::indexNameListGet(string tableName, vector<string>* indexNameVector)
{
	if (!tableExist(tableName)) 
	{
		return 0;
	}
	return Catalog_Manager_Pointer->indexNameListGet(tableName, indexNameVector);
}

void API::allIndexAddressInfoGet(vector<IndexInformation> *indexNameVector)
{
	Catalog_Manager_Pointer->getAllIndex(indexNameVector);
	for (int i = 0; i < (*indexNameVector).size(); i++)
	{
		(*indexNameVector)[i].indexName = Record_Manager_Pointer->indexFileNameGet((*indexNameVector)[i].indexName);
	}
}

int API::attributeGet(string tableName, vector<Attribute>* attributeVector)
{
	if (!tableExist(tableName)) 
	{
		return 0;
	}
	return Catalog_Manager_Pointer->attributeGet(tableName, attributeVector);
}

void API::recordIndexInsert(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, int blockOffset)
{
	char* contentBegin = recordBegin;
	for (int i = 0; i < (*attributeVector).size(); i++)
	{
		int type = (*attributeVector)[i].type;
		int typeSize = typeSizeGet(type);
		if ((*attributeVector)[i].index != "")
		{
			indexInsert((*attributeVector)[i].index, contentBegin, type, blockOffset);
		}

		contentBegin += typeSize;
	}
}

void API::indexInsert(string indexName, char* contentBegin, int type, int blockOffset)
{
	string content = "";
	stringstream tmp;
	//if the attribute has index
	if (type == Attribute::TYPE_INT)
	{
		int value = *((int*)contentBegin);
		tmp << value;
	}
	else if (type == Attribute::TYPE_FLOAT)
	{
		float value = *((float*)contentBegin);
		tmp << value;
	}
	else
	{
		char value[255];
		memset(value, 0, 255);
		memcpy(value, contentBegin, sizeof(type));
		string stringTmp = value;
		tmp << stringTmp;
	}
	tmp >> content;
	Index_Manager_Pointer->insertIndex(Record_Manager_Pointer->indexFileNameGet(indexName), content, blockOffset, type);
}

void API::recordIndexDelete(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, int blockOffset)
{
	char* contentBegin = recordBegin;
	for (int i = 0; i < (*attributeVector).size(); i++)
	{
		int type = (*attributeVector)[i].type;
		int typeSize = typeSizeGet(type);

		string content = "";
		stringstream tmp;

		if ((*attributeVector)[i].index != "")
		{
			if (type == Attribute::TYPE_INT)
			{
				int value = *((int*)contentBegin);
				tmp << value;
			}
			else if (type == Attribute::TYPE_FLOAT)
			{
				float value = *((float*)contentBegin);
				tmp << value;
			}
			else
			{
				char value[255];
				memset(value, 0, 255);
				memcpy(value, contentBegin, sizeof(type));
				string stringTmp = value;
				tmp << stringTmp;
			}
			tmp >> content;
			Index_Manager_Pointer->deleteIndexByKey(Record_Manager_Pointer->indexFileNameGet((*attributeVector)[i].index), content, type);
		}
		contentBegin += typeSize;
	}

}

int API::tableExist(string tableName)
{
	if (Catalog_Manager_Pointer->findTable(tableName) != TABLE_FILE)
	{
		cout << "There is no table " << tableName << endl;
        string temp = "There is no table ";
        temp += tableName;
        Table::text->append(QString::fromStdString(temp));
		return 0;
	}
	else
	{
		return 1;
	}
}

string API::primaryIndexNameGet(string tableName)
{
	return  "PRIMARY_" + tableName;
}

void API::tableAttributePrint(vector<string>* attributeNameVector)
{
	int i = 0;
    string temp = "";
	for (i = 0; i < (*attributeNameVector).size(); i++)
	{
		printf("%s ", (*attributeNameVector)[i].c_str());
        temp += (*attributeNameVector)[i];
        temp += " ";
	}
    temp += "\n";
    Table::text->append(QString::fromStdString(temp));
    //Table::text->append(" ");
	if (i != 0)
	{
		printf("\n");
	}
}


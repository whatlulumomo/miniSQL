//
// Created by Administrator on 2017/6/1 0001.
//

#include "IndexInformation.h"
#include <vector>
#include "IndexManager.h"
#include <iostream>
#include "API.h"
#include "table.h"
using namespace std;


// Constructor
IndexManager::IndexManager(API *m_api)
{
	api = m_api;
	vector<IndexInformation> allIndexInfo;
	api->allIndexAddressInfoGet(&allIndexInfo);// read the index file and set information of all index into this vector
	for (vector<IndexInformation>::iterator i = allIndexInfo.begin(); i != allIndexInfo.end(); i++)
	{
		createIndex(i->indexName, i->type);
	}
}

/*
Destructor Function: write the dirty indexs back to the disk.
*/
IndexManager::~IndexManager()
{
	// tranverse the map and write the content into disk then delete it
	for (intMap::iterator itInt = indexIntMap.begin(); itInt != indexIntMap.end(); itInt++)
	{
		// writtenbackToDiskAll is one function of BPLUSTREE
		if (itInt->second)
		{
			itInt->second->writtenbackToDiskAll();
			delete itInt->second;
		}
	}
	for (stringMap::iterator itString = indexStringMap.begin(); itString != indexStringMap.end(); itString++)
	{
		if (itString->second)
		{
			itString->second->writtenbackToDiskAll();
			delete itString->second;
		}
	}
	for (floatMap::iterator itFloat = indexFloatMap.begin(); itFloat != indexFloatMap.end(); itFloat++)
	{
		if (itFloat->second)
		{
			itFloat->second->writtenbackToDiskAll();
			delete itFloat->second;
		}
	}
}


/*
Create index on the specific type.
If there exists the index before, read data from file path and then rebuild the b+ tree.
*/
void IndexManager::createIndex(string filePath, int type)
{
	int keySize = getKeySize(type);// INT,FLOAT,CHAR
	int degree = getDegree(type);//the block's space is restricted so the degree is decided by the type of data
								 // build the tree and insert it into map
	if (type == TYPE_INT)
	{
		BPlusTree<int> *tree = new BPlusTree<int>(filePath, keySize, degree);
		indexIntMap.insert(intMap::value_type(filePath, tree));
	}
	else if (type == TYPE_FLOAT)
	{
		BPlusTree<float> *tree = new BPlusTree<float>(filePath, keySize, degree);
		indexFloatMap.insert(floatMap::value_type(filePath, tree));
	}
	else // string
	{
		BPlusTree<string> *tree = new BPlusTree<string>(filePath, keySize, degree);
		indexStringMap.insert(stringMap::value_type(filePath, tree));
	}

}


/*
delete one index and erase it from the index vector
*/
void IndexManager::dropIndex(string filePath, int type)
{
	if (type == TYPE_INT)
	{
		intMap::iterator itInt = indexIntMap.find(filePath);
		if (itInt == indexIntMap.end())
		{
			cout << "Error:in drop index, no index " << filePath << " exits" << endl;
            string temp = "Error:in drop index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			delete itInt->second;  // delete the B+ Tree, erase this node
			indexIntMap.erase(itInt);
		}
	}
	else if (type == TYPE_FLOAT)
	{
		floatMap::iterator itFloat = indexFloatMap.find(filePath);
		if (itFloat == indexFloatMap.end())
		{
			cout << "Error:in drop index, no index " << filePath << " exits" << endl;
            string temp = "Error:in drop index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			delete itFloat->second;
			indexFloatMap.erase(itFloat);
		}
	}
	else // string
	{
		stringMap::iterator itString = indexStringMap.find(filePath);
		if (itString == indexStringMap.end())
		{
			cout << "Error:in drop index, no index " << filePath << " exits" << endl;
            string temp = "Error:in drop index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			delete itString->second;
			indexStringMap.erase(itString);
		}
	}

}

/*
search the B+ Tree by the key and return the value(Block offset value)
*/
offsetNumber IndexManager::searchIndex(string filePath, string key, int type)
{
	setKey(type, key);

	if (type == TYPE_INT)
	{
		intMap::iterator itInt = indexIntMap.find(filePath);
		if (itInt == indexIntMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return -1;
		}
		else
		{
			return itInt->second->search(kt.intTmp);
		}
	}
	else if (type == TYPE_FLOAT)
	{
		floatMap::iterator itFloat = indexFloatMap.find(filePath);
		if (itFloat == indexFloatMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return -1;
		}
		else
		{
			return itFloat->second->search(kt.floatTmp);

		}
	}
	else // string
	{
		stringMap::iterator itString = indexStringMap.find(filePath);
		if (itString == indexStringMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return -1;
		}
		else
		{
			return itString->second->search(key);
		}
	}
}
/*
Insert the key and value into the B+ Tree
*/
void IndexManager::insertIndex(string filePath, string key, offsetNumber blockOffset, int type)
{
	setKey(type, key);

	if (type == TYPE_INT)
	{
		intMap::iterator itInt = indexIntMap.find(filePath);
		if (itInt == indexIntMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			itInt->second->insertKey(kt.intTmp, blockOffset);
		}
	}
	else if (type == TYPE_FLOAT)
	{
		floatMap::iterator itFloat = indexFloatMap.find(filePath);
		if (itFloat == indexFloatMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			itFloat->second->insertKey(kt.floatTmp, blockOffset);

		}
	}
	else // string
	{
		stringMap::iterator itString = indexStringMap.find(filePath);
		if (itString == indexStringMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			itString->second->insertKey(key, blockOffset);
		}
	}
}

/*
delete the key and its value
*/
void IndexManager::deleteIndexByKey(string filePath, string key, int type)
{
	setKey(type, key);

	if (type == TYPE_INT)
	{
		intMap::iterator itInt = indexIntMap.find(filePath);
		if (itInt == indexIntMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			itInt->second->deleteKey(kt.intTmp);
		}
	}
	else if (type == TYPE_FLOAT)
	{
		floatMap::iterator itFloat = indexFloatMap.find(filePath);
		if (itFloat == indexFloatMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			itFloat->second->deleteKey(kt.floatTmp);

		}
	}
	else // string
	{
		stringMap::iterator itString = indexStringMap.find(filePath);
		if (itString == indexStringMap.end())
		{
			cout << "Error:in search index, no index " << filePath << " exits" << endl;
            string temp = "Error:in search index, no index ";
            temp += filePath;
            temp += " exits";
            Table::text->append(QString::fromStdString(temp));
			return;
		}
		else
		{
			itString->second->deleteKey(key);
		}
	}
}

/*
the block's space is restricted
so the degree is decided by the type of data
*/
int IndexManager::getDegree(int type)
{
	int degree = bm.getBlockSize() / (getKeySize(type) + sizeof(offsetNumber));
	if (degree % 2 == 0) degree -= 1;
	return degree;
}

// return key size
int IndexManager::getKeySize(int type)
{
	if (type == TYPE_FLOAT)
		return sizeof(float);
	else if (type == TYPE_INT)
		return sizeof(int);
	else if (type > 0)
		return type + 1;
	else
	{
		cout << "ERROR: in getKeySize: invalid type" << endl;
        Table::text->append("ERROR: in getKeySize: invalid type");
		return -100;
	}
}


/*
the type of key:Int, char, float, but in the program key is set as string
*/
void IndexManager::setKey(int type, string key)
{
	stringstream ss;
	ss << key;
	if (type == this->TYPE_INT)
		ss >> this->kt.intTmp;
	else if (type == this->TYPE_FLOAT)
		ss >> this->kt.floatTmp;
	else if (type > 0)
		ss >> this->kt.stringTmp;
    else{
		cout << "Error: in getKey: invalid type" << endl;
        Table::text->append("Error: in getKey: invalid type");
    }


}







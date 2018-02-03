#include <iostream>
#include "RecordManager.h"
#include <cstring>
#include "API.h"
#pragma warning(disable:4996)；

RecordManager::RecordManager()
{

}

RecordManager::~RecordManager()
{

}

int RecordManager::tableCreate(string tableName)
{
    string tableFileName = tableFileNameGet(tableName);
    
    FILE *fp;
    fp = fopen(tableFileName.c_str(), "w+");
    if (fp == NULL)
    {
        return 0;
    }
    fclose(fp);
    return 1;
}

int RecordManager::tableDrop(string tableName)
{
    string tableFileName = tableFileNameGet(tableName);
    bm.delete_fileNode(tableFileName.c_str());
    if (remove(tableFileName.c_str()))
    {
        return 0;
    }
    return 1;
}

int RecordManager::indexCreate(string indexName)
{
    string indexFileName = indexFileNameGet(indexName);
    
    FILE *fp;
    fp = fopen(indexFileName.c_str(), "w+");
    if (fp == NULL)
    {
        return 0;
    }
    fclose(fp);
    return 1;
}

int RecordManager::indexDrop(string indexName)
{
    string indexFileName = indexFileNameGet(indexName);
    bm.delete_fileNode(indexFileName.c_str());
    if (remove(indexFileName.c_str()))
    {
        return 0;
    }
    return 1;
}

int RecordManager::recordInsert(string tableName,char* record, int recordSize)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (bm.get_usingSize(*btmp) <= bm.getBlockSize() - recordSize)
        {
            
            char* addressBegin;
            addressBegin = bm.get_content(*btmp) + bm.get_usingSize(*btmp);
            memcpy(addressBegin, record, recordSize);
            bm.set_usingSize(*btmp, bm.get_usingSize(*btmp) + recordSize);
            bm.set_dirty(*btmp);
            return btmp->offsetNum;
        }
        else
        {
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

int RecordManager::recordAllShow(string tableName, vector<string>* attributeNameVector,  vector<Condition>* conditionVector)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (btmp->ifbottom)
        {
            int recordBlockNum = recordBlockShow(tableName,attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            return count;
        }
        else
        {
            int recordBlockNum = recordBlockShow(tableName, attributeNameVector, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);
    if (block == NULL)
    {
        return -1;
    }
    else
    {
        return  recordBlockShow(tableName, attributeNameVector, conditionVector, block);
    }
}

int RecordManager::recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, blockNode* block)
{
    
    //if block is null, return -1
    if (block == NULL)
    {
        return -1;
    }
    
    int count = 0;
    
    char* recordBegin = bm.get_content(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->recordSizeGet(tableName);

    api->attributeGet(tableName, &attributeVector);
    char* blockBegin = bm.get_content(*block);
    int usingSize = bm.get_usingSize(*block);
    
    while (recordBegin - blockBegin  < usingSize)
    {
        //if the recordBegin point to a record
        
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
        {
            count ++;
            recordPrint(recordBegin, recordSize, &attributeVector, attributeNameVector);
            printf("\n");
            Table::text->append("");
        }
        
        recordBegin += recordSize;
    }
    
    return count;
}

int RecordManager::recordAllFind(string tableName, vector<Condition>* conditionVector)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (btmp->ifbottom)
        {
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            return count;
        }
        else
        {
            int recordBlockNum = recordBlockFind(tableName, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

int RecordManager::recordBlockFind(string tableName, vector<Condition>* conditionVector, blockNode* block)
{
    //if block is null, return -1
    if (block == NULL)
    {
        return -1;
    }
    int count = 0;
    
    char* recordBegin = bm.get_content(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->recordSizeGet(tableName);
    
    api->attributeGet(tableName, &attributeVector);
    
    while (recordBegin - bm.get_content(*block)  < bm.get_usingSize(*block))
    {
        //if the recordBegin point to a record
        
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
        {
            count++;
        }
        
        recordBegin += recordSize;
        
    }
    
    return count;
}

int RecordManager::recordAllDelete(string tableName, vector<Condition>* conditionVector)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);

    int count = 0;
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (btmp->ifbottom)
        {
            int recordBlockNum = recordBlockDelete(tableName, conditionVector, btmp);
            count += recordBlockNum;
            return count;
        }
        else
        {
            int recordBlockNum = recordBlockDelete(tableName, conditionVector, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, int blockOffset)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode* block = bm.getBlockByOffset(ftmp, blockOffset);
    if (block == NULL)
    {
        return -1;
    }
    else
    {
        return  recordBlockDelete(tableName, conditionVector, block);
    }
}

int RecordManager::recordBlockDelete(string tableName,  vector<Condition>* conditionVector, blockNode* block)
{
    //if block is null, return -1
    if (block == NULL)
    {
        return -1;
    }
    int count = 0;
    
    char* recordBegin = bm.get_content(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->recordSizeGet(tableName);
    
    api->attributeGet(tableName, &attributeVector);
    
    while (recordBegin - bm.get_content(*block) < bm.get_usingSize(*block))
    {
        //if the recordBegin point to a record
        
        if(recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
        {
            count ++;
            
            api->recordIndexDelete(recordBegin, recordSize, &attributeVector, block->offsetNum);
            int i = 0;
            for (i = 0; i + recordSize + recordBegin - bm.get_content(*block) < bm.get_usingSize(*block); i++)
            {
                recordBegin[i] = recordBegin[i + recordSize];
            }
            memset(recordBegin + i, 0, recordSize);
            bm.set_usingSize(*block, bm.get_usingSize(*block) - recordSize);
            bm.set_dirty(*block);
        }
        else
        {
            recordBegin += recordSize;
        }
    }
    
    return count;
}

int RecordManager::indexRecordAllAlreadyInsert(string tableName,string indexName)
{
    fileNode *ftmp = bm.getFile(tableFileNameGet(tableName).c_str());
    blockNode *btmp = bm.getBlockHead(ftmp);
    int count = 0;
    while (true)
    {
        if (btmp == NULL)
        {
            return -1;
        }
        if (btmp->ifbottom)
        {
            int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, btmp);
            count += recordBlockNum;
            return count;
        }
        else
        {
            int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, btmp);
            count += recordBlockNum;
            btmp = bm.getNextBlock(ftmp, btmp);
        }
    }
    
    return -1;
}

int RecordManager::indexRecordBlockAlreadyInsert(string tableName,string indexName,  blockNode* block)
{
    //if block is null, return -1
    if (block == NULL)
    {
        return -1;
    }
    int count = 0;
    
    char* recordBegin = bm.get_content(*block);
    vector<Attribute> attributeVector;
    int recordSize = api->recordSizeGet(tableName);
    
    api->attributeGet(tableName, &attributeVector);
    
    int type;
    int typeSize;
    char * contentBegin;
    
    while (recordBegin - bm.get_content(*block)  < bm.get_usingSize(*block))
    {
        contentBegin = recordBegin;
        //if the recordBegin point to a record
        for (int i = 0; i < attributeVector.size(); i++)
        {
            type = attributeVector[i].type;
            typeSize = api->typeSizeGet(type);
            
            //find the index  of the record, and insert it to index tree
            if (attributeVector[i].index == indexName)
            {
                api->indexInsert(indexName, contentBegin, type, block->offsetNum);
                count++;
            }
            
            contentBegin += typeSize;
        }
        recordBegin += recordSize;
    }
    
    return count;
}

bool RecordManager::recordConditionFit(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,vector<Condition>* conditionVector)
{
    if (conditionVector == NULL) {
        return true;
    }
    int type;
    string attributeName;
    int typeSize;
    char content[255];
    
    char *contentBegin = recordBegin;
    for(int i = 0; i < attributeVector->size(); i++)
    {
        type = (*attributeVector)[i].type;
        attributeName = (*attributeVector)[i].name;
        typeSize = api->typeSizeGet(type);
        
        //init content (when content is string , we can get a string easily)
        memset(content, 0, 255);
        memcpy(content, contentBegin, typeSize);
        for(int j = 0; j < (*conditionVector).size(); j++)
        {
            if ((*conditionVector)[j].attributeName == attributeName)
            {
                //if this attribute need to deal about the condition
                if(!contentConditionFit(content, type, &(*conditionVector)[j]))
                {
                    //if this record is not fit the conditon
                    return false;
                }
            }
        }

        contentBegin += typeSize;
    }
    return true;
}

void RecordManager::recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector)
{
    int type;
    string attributeName;
    int typeSize;
    char content[255];
    
    char *contentBegin = recordBegin;
    for(int i = 0; i < attributeVector->size(); i++)
    {
        type = (*attributeVector)[i].type;
        typeSize = api->typeSizeGet(type);
        
        //init content (when content is string , we can get a string easily)
        memset(content, 0, 255);
        
        memcpy(content, contentBegin, typeSize);

        for(int j = 0; j < (*attributeNameVector).size(); j++)
        {
            if ((*attributeNameVector)[j] == (*attributeVector)[i].name)
            {
                contentPrint(content, type);
                break;
            }
        }
        
        contentBegin += typeSize;
    }
}

void RecordManager::contentPrint(char * content, int type)
{
    string temp="";
    if (type == Attribute::TYPE_INT)
    {
        //if the content is a int
        int tmp = *((int *) content);   //get content value by point
        printf("%d ", tmp);
        temp = to_string(tmp);
        temp += " ";
        Table::text->moveCursor(QTextCursor::End);
        Table::text->insertPlainText(QString::fromStdString(temp));
    }
    else if (type == Attribute::TYPE_FLOAT)
    {
        //if the content is a float
        float tmp = *((float *) content);   //get content value by point
        printf("%f ", tmp);
        temp = to_string(tmp);
        temp += " ";
        Table::text->moveCursor(QTextCursor::End);
        Table::text->insertPlainText(QString::fromStdString(temp));
    }
    else
    {
        //if the content is a string
        string tmp = content;
        printf("%s ", tmp.c_str());
        Table::text->moveCursor(QTextCursor::End);
        Table::text->insertPlainText(QString::fromStdString(tmp));
    }

}

bool RecordManager::contentConditionFit(char* content,int type,Condition* condition)
{
    if (type == Attribute::TYPE_INT)
    {
        //if the content is a int
        int tmp = *((int *) content);   //get content value by point
        return condition->TestRight(tmp);
    }
    else if (type == Attribute::TYPE_FLOAT)
    {
        //if the content is a float
        float tmp = *((float *) content);   //get content value by point
        return condition->TestRight(tmp);
    }
    else
    {
        //if the content is a string
        return condition->TestRight(content);
    }
    return true;
}

string RecordManager::indexFileNameGet(string indexName)
{
    string tmp = "";
    return "INDEX_FILE_"+indexName;
}

string RecordManager::tableFileNameGet(string tableName)
{
    string tmp = "";
    return tmp + "TABLE_FILE_" + tableName;
}

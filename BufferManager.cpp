#include "Node.h"
#include "BufferManager.h"
#include <queue>
#include <iostream>
#include <fstream>
#include <cstring>
#include "table.h"
#pragma warning(disable:4996)

/*
write one block into disk. if it's not dirty
*/
void BufferManager::OneBlockWriteBackToDisk(const char* fileName, blockNode* block)
{
	if (block->dirty == false)
	{
		return;
	}
	FILE *fp = NULL;
	if ((fp = fopen(fileName, "rb+")) != NULL)
	{
		if (fseek(fp, block->offsetNum*BLOCK_SIZE, 0) == 0)
		{
            if (fwrite(block->address, block->using_size + sizeof(int), 1, fp) == 0)
			{
				printf("module writtenBackToDisk :: Can't write block into file %s\n", fileName);
                string temp = "module writtenBackToDisk :: Can't write block into file ";
                temp += fileName;
                Table::text->append(QString::fromStdString(temp));
				exit(1);
			}
		}
		else
		{
			printf("module writtenBackToDisk :: Can't find position in file %s\n", fileName);
            string temp = "module writtenBackToDisk :: Can't find position in file " ;
            temp += fileName;
            Table::text->append(QString::fromStdString(temp));
			exit(1);
		}
		fclose(fp);
	}
	else
	{
		printf("module writtenBackToDisk :: Can't open the file %s\n", fileName);
        string temp = "module writtenBackToDisk :: Can't open the file " ;
        temp += fileName;
        Table::text->append(QString::fromStdString(temp));
		exit(1);
	}
}

/*
when quiting, write all the blocks into disk
*/
void BufferManager::AllWriteBackToDisk()
{
	fileNode *ftmp;
	blockNode *btmp;
	blockNode *lastblock = NULL;

	// traverse the file then traverse the blocks of this file
	if (fileHead != NULL)
	{
		for (ftmp = fileHead; ftmp != NULL; ftmp = ftmp->nextFile)
		{
			if (ftmp->blockHead != NULL)
			{
				// write one and then initial the pre-one. 
				for (btmp = ftmp->blockHead; btmp != NULL; btmp = btmp->nextBlock)
				{
					if (btmp->preBlock != NULL)
						init_block(*(btmp->preBlock));
					if (btmp->nextBlock == NULL)
						lastblock = btmp;
					OneBlockWriteBackToDisk(btmp->fileName, btmp);
				}
				init_block(*(lastblock));
			}
		}
	}
}


/*
When other modoles calls to get block from file, it returns an empty block
we set temporay block array in the program. not write or read from disk(file) directly
only when replace the full block or quit, we read from or write into disk
*/
blockNode* BufferManager::getBlock(fileNode * file, blockNode *position, bool if_pin)
{
	const char * fileName = file->fileName;
	blockNode * btmp = NULL;

	// Empty block 
	if (total_block == 0)
	{
		btmp = &block_pool[0];
		total_block++;
	}
	// Not empty but not full
	else if (total_block < MAX_BLOCK_NUM)
	{
		for (int i = 0; i < MAX_BLOCK_NUM; i++)
		{
			if (block_pool[i].offsetNum == -1) // not used
			{
				btmp = &block_pool[i];
				total_block++;
				break;
			}
			else
				continue;
		}
	}
	else { // the block pool is full, so we use LRU strategy to replace one
		int i = replaced_block; // -1(intial)
		while (1)
		{
			i++;
			if (i >= total_block) i = 0;  // i must be less than total_block
			if (block_pool[i].pin == false) // not locked
			{
				if (block_pool[i].reference == true)
					block_pool[i].reference = false;
				else // choose this to replace
				{
					btmp = &block_pool[i];
					// remove it from list
					if (btmp->nextBlock != NULL)
						btmp->nextBlock->preBlock = btmp->preBlock;
					if (btmp->preBlock != NULL)
						btmp->preBlock->nextBlock = btmp->nextBlock;

					replaced_block = i; // record this positon in order to meet the requirement of LRU

					OneBlockWriteBackToDisk(btmp->fileName, btmp); // write this replaced block into file

					init_block(*btmp); // clear  block_pool[i];
					break;
				}
			}
			else // locked
				continue;
		}
	}
	//add the block into the block list
	if (position != NULL && position->nextBlock == NULL)
	{
		btmp->preBlock = position;
		position->nextBlock = btmp;
		btmp->offsetNum = position->offsetNum + 1;
	}
	else if (position != NULL && position->nextBlock != NULL)
	{
		btmp->preBlock = position;
		btmp->nextBlock = position->nextBlock;
		position->nextBlock->preBlock = btmp;
		position->nextBlock = btmp;
		btmp->offsetNum = position->offsetNum + 1;
	}
	else // the block will be the head of the list
	{
		btmp->offsetNum = 0;
		if (file->blockHead) // if the file has a wrong block head
		{
			file->blockHead->preBlock = btmp;
			btmp->nextBlock = file->blockHead;
		}
		file->blockHead = btmp;
	}
	set_pin(*btmp, if_pin);

	if (strlen(fileName) + 1 > MAX_FILE_NAME)
	{
		printf("The lengh of file name can't be longer than %d\n", MAX_FILE_NAME);
        string temp = "The lengh of file name can't be longer than " + to_string(MAX_FILE_NAME);
        Table::text->append(QString::fromStdString(temp));
		exit(3);
	}
	strncpy(btmp->fileName, fileName, MAX_FILE_NAME);

	// read the content from the file and write into block
	FILE * fileHandle;
	if ((fileHandle = fopen(fileName, "ab+")) != NULL)
	{
		if (fseek(fileHandle, btmp->offsetNum*BLOCK_SIZE, 0) == 0)
		{
			if (fread(btmp->address, 1, BLOCK_SIZE, fileHandle) == 0)
				btmp->ifbottom = true;
			btmp->using_size = getUsingSize(btmp);
		}
		else
		{
			printf("Problem seeking the file %s in reading(module getblock)", fileName);
            string temp = "Problem seeking the file ";
            temp += fileName;
            temp += " in reading(module getblock)";
            Table::text->append(QString::fromStdString(temp));
			exit(1);
		}
		fclose(fileHandle);
	}
	else
	{
		printf("Problem opening the file %s in reading (module getblock)", fileName);
        string temp = "Problem opening the file ";
        temp += fileName;
        temp += " in reading (module getblock)";
        Table::text->append(QString::fromStdString(temp));
		exit(1);
	}
	return btmp;
}

void BufferManager::init_file(fileNode &file)
{
	file.nextFile = NULL;
	file.preFile = NULL;
	file.blockHead = NULL;
	file.pin = false;
	memset(file.fileName, 0, MAX_FILE_NAME);
}


void BufferManager::init_block(blockNode &block)
{
    block.using_size = sizeof(int); // the size of block, the data stores in the head
	block.dirty = false;		// default false. onced modified, set true
	block.nextBlock = NULL;
	block.preBlock = NULL;
	block.offsetNum = -1;       // default -1. offsetnum can be understood as the distance between this block and file head
	block.pin = false;          // 
	block.reference = false;    // used in LRU Replacement Stratege
	block.ifbottom = false;     // whether is bottom of block list

	memset(block.fileName, 0, MAX_FILE_NAME);
	memset(block.address, 0, BLOCK_SIZE);
    int init_usage = 0;
    memcpy(block.address, (char*)&init_usage, sizeof(int));
	// the block address now is the head of block which store the size used of this block, so it's 0 now
}

/*
In the constructor, what needs to do :
1. allocate memory space for every point in block and file struct
2. return error when fail to allocate
3. initial every block and file
*/
BufferManager::BufferManager() :total_block(0), total_file(0), fileHead(NULL)
{
	for (int i = 0; i < MAX_FILE_NUM; i++)
	{
		file_pool[i].fileName = new char[FILENAME_MAX];
		if (file_pool[i].fileName == NULL)
		{
			cout << "ERROR :: can't allocate space when initializing" << endl;
            Table::text->append("ERROR :: can't allocate space when initializing");
			exit(1);
		}
		init_file(file_pool[i]);
	}
	for (int i = 0; i < MAX_BLOCK_NUM; i++)
	{
		block_pool[i].fileName = new char[MAX_FILE_NAME];
		if (block_pool[i].fileName == NULL)
		{
			cout << "ERROR :: can't allocate space when initializing" << endl;
            Table::text->append("ERROR :: can't allocate space when initializing");
			exit(1);
		}

		block_pool[i].address = new char[BLOCK_SIZE];
		if (block_pool[i].address == NULL)
		{
			cout << "ERROR :: can't allocate space when initializing" << endl;
            Table::text->append("ERROR :: can't allocate space when initializing");
			exit(1);
		}
		init_block(block_pool[i]);
	}
}

/*
in destructor, what we need to do:
1. write the blocks (temporary in program) into disk
2. delete all the varibles which we "new" in constructor
*/
BufferManager::~BufferManager()
{
	AllWriteBackToDisk();
	for (int i = 0; i < MAX_FILE_NUM; i++)
	{
		delete file_pool[i].fileName;
	}
	for (int i = 0; i < MAX_BLOCK_NUM; i++)
	{
		delete block_pool[i].fileName;
		delete block_pool[i].address;
	}
}

void BufferManager::set_pin(blockNode &block, bool pin)
{
	block.pin = pin;
	if (!pin)
		block.reference = true;
}

void BufferManager::set_pin(fileNode &file, bool pin)
{
	file.pin = pin;
}

/*
the head of block records the used size of block
*/
int BufferManager::getUsingSize(blockNode* block)
{
    return *(int*)block->address;
}

/*
true(dirty) means the block has been modified
*/
void BufferManager::clean_dirty(blockNode &block)
{
	block.dirty = false;
}

void BufferManager::set_dirty(blockNode &block)
{
	block.dirty = true;
}
/*
set the usage as the using size of this block
*/
void BufferManager::set_usingSize(blockNode & block, int usage)
{
	block.using_size = usage;
    memcpy(block.address, (char*)&usage, sizeof(int));
}

blockNode* BufferManager::getNextBlock(fileNode* file, blockNode* block)
{
	if (block->nextBlock == NULL)
	{
		if (block->ifbottom == true)
			block->ifbottom = false;
		return getBlock(file, block);
	}
	if (block->offsetNum == block->nextBlock->offsetNum - 1)
	{
		return block->nextBlock;
	}
	else //the block list is not in the right order
	{
		return getBlock(file, block);
	}
}

blockNode* BufferManager::getBlockHead(fileNode *file)
{
	if (file->blockHead != NULL)
	{
		if (file->blockHead->offsetNum == 0) //The right offset of the first block
		{
			return file->blockHead;
		}
		else // in case that the head is not head
		{
			return getBlock(file, NULL);
		}
	}
	else
		return getBlock(file, NULL);
}

blockNode* BufferManager::getBlockByOffset(fileNode *file, int offset)
{
	blockNode* btmp = getBlockHead(file);
	int i = offset;
	while (i > 0)
	{
		btmp = getNextBlock(file, btmp);
		i--;
	}
	return btmp;
}

// wait to modify

void BufferManager::delete_fileNode(const char * fileName)
{
	fileNode* ftmp = getFile(fileName);
	blockNode* btmp = getBlockHead(ftmp);
	queue<blockNode*> blockQ;
	while (true) {
		if (btmp == NULL) return;
		blockQ.push(btmp);
		if (btmp->ifbottom) break;
		btmp = getNextBlock(ftmp, btmp);
	}
	total_block -= blockQ.size();
	while (!blockQ.empty())
	{
		init_block(*blockQ.back());
		blockQ.pop();
	}
	if (ftmp->preFile) ftmp->preFile->nextFile = ftmp->nextFile;
	if (ftmp->nextFile) ftmp->nextFile->preFile = ftmp->preFile;
	if (fileHead == ftmp) fileHead = ftmp->nextFile;
	init_file(*ftmp);
	total_file--;
}

fileNode* BufferManager::getFile(const char * fileName, bool if_pin)
{
	blockNode * btmp = NULL;
	fileNode * ftmp = NULL;

	if (fileHead != NULL)
	{
		for (ftmp = fileHead; ftmp != NULL; ftmp = ftmp->nextFile)
		{
			if (strcmp(fileName, ftmp->fileName) == 0) // find the file
			{
				ftmp->pin = if_pin;
				return ftmp;
			}
		}
	}
	// The file is not in the list
	if (total_file == 0)
	{
		ftmp = &file_pool[total_file];
		total_file++;
		fileHead = ftmp;
	}
	else if (total_file < MAX_FILE_NUM) // There are empty fileNode in the pool
	{
		ftmp = &file_pool[total_file];
		// add this fileNode into the tail of the list
		file_pool[total_file - 1].nextFile = ftmp;
		ftmp->preFile = &file_pool[total_file - 1];
		total_file++;
	}
	else // if total_file >= MAX_FILE_NUM, find one fileNode to replace, write back the block node belonging to the fileNode
	{
		ftmp = fileHead;
		while (ftmp->pin)
		{
			if (ftmp->nextFile)
				ftmp = ftmp->nextFile;
			else //no enough file node in the pool
			{
				printf("There are no enough file node in the pool!");
				exit(2);
			}
		}
		for (btmp = ftmp->blockHead; btmp != NULL; btmp = btmp->nextBlock)
		{
			if (btmp->preBlock)
			{
				init_block(*(btmp->preBlock));
				total_block--;
			}
			OneBlockWriteBackToDisk(btmp->fileName, btmp); // write back the file
		}
		init_file(*ftmp);
	}
	if (strlen(fileName) + 1 > MAX_FILE_NAME)
	{
        printf("The length of filename can't be longer than %d\n", MAX_FILE_NAME);
        string temp = "The length of filename can't be longer than "+ to_string(MAX_FILE_NAME);
        Table::text->append(QString::fromStdString(temp));
		exit(3);
	}
	strncpy(ftmp->fileName, fileName, MAX_FILE_NAME);
	set_pin(*ftmp, if_pin);
	return ftmp;
}

int BufferManager::get_usingSize(blockNode & block)
{
	return block.using_size;
}

int BufferManager::getBlockSize()
{
    return BLOCK_SIZE - sizeof(int);
}

char* BufferManager::get_content(blockNode& block)
{
    return block.address + sizeof(int);
}

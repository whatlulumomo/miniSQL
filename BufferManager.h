#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H
#include "Node.h"
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE  4096

class BufferManager
{
private:
	// variable
	fileNode *fileHead;
	fileNode file_pool[MAX_FILE_NUM];
	blockNode  block_pool[MAX_BLOCK_NUM];
	int total_block;
	int total_file;

	// Initial
	void init_block(blockNode & block);
	void init_file(fileNode & file);

	// call get a block
	blockNode* getBlock(fileNode * file, blockNode* position, bool if_pin = false);

	// write function
	void AllWriteBackToDisk();
	void OneBlockWriteBackToDisk(const char* fileName, blockNode* block);

	void clean_dirty(blockNode &block);
    int getUsingSize(blockNode* block);

public:
	BufferManager();
	~BufferManager();

	void set_dirty(blockNode & block);
	void set_pin(blockNode & block, bool pin); // pin can be understood lock
	void set_pin(fileNode & file, bool pin);
    void set_usingSize(blockNode & block, int usage);

	blockNode* getNextBlock(fileNode * file, blockNode* block);
	blockNode* getBlockHead(fileNode* file);
	blockNode* getBlockByOffset(fileNode* file, int offestNumber);

	void delete_fileNode(const char * fileName);
	fileNode* getFile(const char* fileName, bool if_pin = false);

    int get_usingSize(blockNode & block);

	/*
	BLOCK_SIZE is total size of this block
	the head reocrds the size of this block which is not included in content
	*/
	static int getBlockSize();

	/*
	the head of block is information of size
    so block.address + sizeof(int) is the address of content of this block
	*/
	char* get_content(blockNode& block);

};

static int replaced_block = -1;


#endif // !_BUFFERMANAGER_H

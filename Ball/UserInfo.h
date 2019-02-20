#ifndef __USER_INFO_H__
#define __USER_INFO_H__

#define CLIENT_MAX 50
#define NAME_SIZE 30
#define PWD_SIZE 40
#define BUF_SIZE 1024

typedef struct 
{
	char buffer[BUF_SIZE];
	char name[NAME_SIZE];
	char password[PWD_SIZE];
	short nameSize;
	int sock;

	float record;
	char date[30];

	int roomIdx;
	
	int mapSeq[8];
	int mapSeqMade;
}UserInfo;

void eraseClntFromArray(UserInfo *clntArr,int sock, int count);
void saveUserID(int sock, UserInfo *uiArr, char *name, int clntCount);
int findUserIdx(int sock, UserInfo *uiArr, int clntCount);
void printUserID(int sock, UserInfo *uiArr, int clntCount);
int isUserID(UserInfo *uiArr, int clntCount, char *name);

#endif

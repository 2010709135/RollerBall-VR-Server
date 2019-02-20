#include <stdio.h>
#include <string.h>
#include "UserInfo.h"


void eraseClntFromArray(UserInfo *uiArr,int sock, int count){
	int i=0;

	for(i=0 ; i<count ; i++){
		if(uiArr[i].sock == sock){
			break;
		}
	}

	printf("Client %d(socket fd) is disconnected\n", sock);

	for(;i<count-1 ; i++){
		uiArr[i] = uiArr[i+1];
	}
	
	// close client socket
	close(sock);
}

void saveUserID(int sock, UserInfo *uiArr, char *name, int clntCount){
	int j=0;

	for(j=0 ; j<clntCount ; j++){
		// find user info element
		if(uiArr[j].sock == sock){
			strncpy(uiArr[j].name, name, strlen(name)+1);
			break;
		}
	}

	printf("Connected User ID : %s\n", uiArr[j].name);
}

int findUserIdx(int sock, UserInfo *uiArr, int clntCount){
	int i=0;

	for(i=0 ; i<clntCount ; i++){
		if(uiArr[i].sock == sock)
			break;
	}

	return i;
}

void printUserID(int sock, UserInfo *uiArr, int clntCount){
	int idx;

	idx = findUserIdx(sock, uiArr, clntCount);

	printf("User ID : %s\n", uiArr[idx].name);
}

int isUserID(UserInfo *uiArr, int clntCount, char *name){
	int i=0;
	int exist = 0;

	for(i=0 ; i<clntCount ; i++){
		if(!strncmp(uiArr[i].name, name, strlen(name))){
			exist = 1;
			break;
		}			
	}

	return exist;
}

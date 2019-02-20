#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "RoomInfo.h"

#define WAIT 0
#define START 1

void initRoomInfo(RoomInfo *pRoom){
	int i=0, m=0;

//	srand(time(NULL));
	
	for(i=0 ; i<ROOM_MAX ; i++){
		pRoom[i].numOfClient = 0;
		pRoom[i].isStarted = WAIT;
		pRoom[i].mapSeqMade = 0;
	}
}

int findSpareRoom(RoomInfo *pRoom){
	int i=0;

	for(i=0 ; i<ROOM_MAX ; i++){
		if(pRoom[i].numOfClient < CLIENT_MAX_ROOM && pRoom[i].isStarted == WAIT)
			break;
	}
	
	return i;
}

int joinRoom(RoomInfo *pRoom, int roomIdx, UserInfo *user){
	if(pRoom[roomIdx].numOfClient >= CLIENT_MAX_ROOM && pRoom[roomIdx].isStarted == WAIT)
		return -1;

	pRoom[roomIdx].users[pRoom[roomIdx].numOfClient] = user;
	pRoom[roomIdx].numOfClient++;

	if(pRoom[roomIdx].numOfClient == CLIENT_MAX_ROOM)
		pRoom[roomIdx].isStarted = START;

	(*user).roomIdx = roomIdx;

	return 0;
}

void sendDataToRoomMember(int sock, RoomInfo *pRoom, int roomIdx, char *buffer, int sendSize){
	int i=0;

	for(i=0 ; i<pRoom[roomIdx].numOfClient ; i++){
		if(sock == (pRoom[roomIdx].users[i])->sock)
			continue;
		send(pRoom[roomIdx].users[i]->sock, buffer, sendSize, 0);
		
	}
}
 
int isUserInRoom(RoomInfo *pRoom, UserInfo *uiArr, int sock, int clntCount){
	
	int roomIdx, userIdx;

	userIdx = findUserIdx(sock, uiArr, clntCount);

	roomIdx = uiArr[userIdx].roomIdx;
	
	if(roomIdx == -1)
		return 0;
	else
		return 1;

}

void sendExitMessage(UserInfo *uiArr, int userIdx, RoomInfo *pRoom, int roomIdx, MainMenu menu){
	char exitBuf[BUF_SIZE];
	short totalSize = 0;
	int i=0;


	for(i=0 ; i<pRoom[roomIdx].numOfClient ; i++){
		if(!strcmp(pRoom[roomIdx].users[i]->name, uiArr[userIdx].name))
			continue;
		send(pRoom[roomIdx].users[i]->sock, exitBuf, totalSize, 0);
	}	
}

void subExitMemFromRoom(UserInfo *user, RoomInfo *pRoom, int roomIdx){
	int i=0;

	for(i=0 ; i<pRoom[roomIdx].numOfClient ; i++){
		if(user == pRoom[roomIdx].users[i]){
			pRoom[roomIdx].users[i] = 0;
			break;
		}
	}
	
	for( ; i<pRoom[roomIdx].numOfClient-1 ; i++){
		pRoom[roomIdx].users[i] = pRoom[roomIdx].users[i+1];
	}
	pRoom[roomIdx].users[i] = 0;
}

void removeFromRoom(RoomInfo *pRoom, UserInfo *uiArr, int userIdx){
	int roomIdx = uiArr[userIdx].roomIdx;
	
	subExitMemFromRoom(&uiArr[userIdx], pRoom, roomIdx);

	uiArr[userIdx].roomIdx = -1;
	uiArr[userIdx].mapSeqMade = 0;
	pRoom[roomIdx].numOfClient--;
	if(pRoom[roomIdx].numOfClient == 0){
		pRoom[roomIdx].mapSeqMade = 0;
		pRoom[roomIdx].isStarted = WAIT;
	}
}

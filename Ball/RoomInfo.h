#ifndef __ROOM_INFO_H__
#define __ROOM_INFO_H__

#include "UserInfo.h"
#include "MainMenu.h"

#define CLIENT_MAX_ROOM 4
#define ROOM_MAX 10

typedef struct{
	int numOfClient;
	UserInfo *users[CLIENT_MAX_ROOM];
	int isStarted;
	int mapSeq[8];
	int mapSeqMade;
}RoomInfo;

void initRoomInfo(RoomInfo *pRoom);
int findSpareRoom(RoomInfo *pRoom);
int joinRoom(RoomInfo *pRoom, int roomIdx, UserInfo *user);
void sendDataToRoomMember(int sock, RoomInfo *pRoom, int roomIdx, char *buffer, int sendSize);
int isUserInRoom(RoomInfo *pRoom, UserInfo *uiArr, int sock, int clntCount);
void sendExitMessage(UserInfo *uiArr, int userIdx, RoomInfo *pRoom, int roomIdx, MainMenu menu);
void subExitMemFromRoom(UserInfo *user, RoomInfo *pRoom, int roomIdx);
void removeFromRoom(RoomInfo *pRoom, UserInfo *uiArr, int userIdx);

#endif


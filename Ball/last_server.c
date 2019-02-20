#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>


#include "RoomInfo.h"
#include "UserInfo.h"
#include "MainMenu.h"

void error_terminating(char *message);

int main(int argc, char *argv[]){
	int sock_serv, sock_clnt;
	int clntArr[CLIENT_MAX] = {0,};
	int clntCount = 0;
	struct sockaddr_in addr_serv, addr_clnt;
	socklen_t size_adr;

	int errorCheck = 0;
	
	char buffer[BUF_SIZE];
	char temp[BUF_SIZE];
	int mapSeq[8];

	struct epoll_event *events_all;
	struct epoll_event event;
	int fd_epoll, count = 0;

	int i,j, k, readSize, m, q;

	short totalSize;

	int mapSeqMade = 0;

	int val = 1;

	UserInfo uiArr[CLIENT_MAX];
	UserInfo tempUser;

	RoomInfo room[ROOM_MAX];
	int roomCount = 0;

	MainMenu menu = {"Start", MENU_ID, MENU_SINGLE, MENU_MULTI, MENU_RANK, MENU_EXIT, strlen("Start")};

	int check = 0;

	// init Information
	memset(uiArr, 0, sizeof(UserInfo)*CLIENT_MAX);
	initRoomInfo(room);

	// create socket
	sock_serv = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_serv == -1)
		error_terminating("socket() error");

	errorCheck = setsockopt(sock_serv, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	if(errorCheck <0 )
		error_terminating("setsockopt() error");

	// set address
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_serv.sin_port = htons(atoi(argv[1]));

	if(bind(sock_serv, (struct sockaddr*)&addr_serv, sizeof(addr_serv)) == -1)
		error_terminating("bind() error");
	if(listen(sock_serv, 5) == -1)
		error_terminating("listen() error");

	// create epoll instance and events dynaminc allocated array
	fd_epoll = epoll_create(50);
	events_all = malloc(sizeof(struct epoll_event) * CLIENT_MAX);

	// add server socket in epoll instance
	event.events = EPOLLIN; // sense input message
	event.data.fd = sock_serv;
	epoll_ctl(fd_epoll, EPOLL_CTL_ADD, sock_serv, &event); // add server socket

	printf("Server is Ready\n");

	srand(time(NULL));

	while(1){
		count = epoll_wait(fd_epoll, events_all, CLIENT_MAX, -1);

		for(i=0 ; i<count ; i++){

			// when server gets event from server socket
			// add client into, epoll instance
			if(events_all[i].data.fd == sock_serv){
				// accept client
				size_adr = sizeof(addr_clnt);
				sock_clnt = accept(events_all[i].data.fd
						, (struct sockaddr*)NULL
						, NULL);
				// add client in epoll
				event.events = EPOLLIN;
				event.data.fd = sock_clnt;
				epoll_ctl(fd_epoll, EPOLL_CTL_ADD, sock_clnt, &event);
				//clntArr[clntCount++] = sock_clnt;
				
				uiArr[clntCount].sock = sock_clnt;
				uiArr[clntCount].roomIdx = -1;
				uiArr[clntCount].mapSeqMade = 0;
				uiArr[clntCount].namePassed = 0;
				clntCount++;
			}
			// when client send info with menu selection char
			else{

				readSize = recv(events_all[i].data.fd, buffer, BUF_SIZE, 0);
				buffer[readSize] = '\0';

				// if readSize if zero, than connect is broken. So remove client
				// from epoll and uiArr
				if(readSize == 0){
					int userIdx = findUserIdx(events_all[i].data.fd, uiArr, clntCount);
					
					if(isUserInRoom(room, uiArr, events_all[i].data.fd, clntCount)){							
							sendDataToRoomMember(events_all[i].data.fd, room, uiArr[userIdx].roomIdx,
									buffer, readSize);				
							removeFromRoom(room, uiArr, userIdx);			
					}

					epoll_ctl(fd_epoll, EPOLL_CTL_DEL, events_all[i].data.fd, NULL);
					eraseClntFromArray(uiArr, events_all[i].data.fd, clntCount);

					clntCount--;
					continue;
				}

				// readSize must be largger than strlen("Start") + sizeof(char)
				if(readSize < menu.sizeStart + 1)
					continue;
				
				// if buffer starts with "Start", read menu string
				if(!strncmp(menu.start, buffer, menu.sizeStart)){
					// Menu selection(char) is at buffer[menu.sizeStart] 
					if(MENU_ID == buffer[menu.sizeStart]){
						// convert 2 bytes that stores ID length as short in client to int
						short nameSize = buffer[menu.sizeStart+1] | buffer[menu.sizeStart+2] << 8;
						char name[nameSize+1];

						memset(temp, 0, sizeof(char)*BUF_SIZE);
						strncpy(temp, menu.start, menu.sizeStart);
						// total Size
						totalSize = menu.sizeStart+ sizeof(totalSize) + 2;
						//printf("totalSize = %d\n", totalSize);
						memcpy(temp+menu.sizeStart, &totalSize, sizeof(totalSize));

						// copy name in temporary name array
						strncpy(name, &buffer[menu.sizeStart+3], nameSize);
						name[nameSize] = '\0';

						// if user ID is already in uiArr send No and 
						// do not save user
						if(isUserID(uiArr, clntCount, name) == 1){
							strncpy(temp+menu.sizeStart+sizeof(totalSize), "NO", strlen("NO"));
							send(events_all[i].data.fd, temp, strlen("NO"), totalSize);
						}else{
							strncpy(temp+menu.sizeStart+sizeof(totalSize), "OK", strlen("OK"));
							// save client ID into uiArr
							saveUserID(events_all[i].data.fd, uiArr, name, clntCount);
							send(events_all[i].data.fd, temp, strlen("OK"), totalSize);
							//clntCount++;
						}
					}
					else if(MENU_SINGLE == buffer[menu.sizeStart]){
						printf("Single\n");
					}
					else if(MENU_MULTI == buffer[menu.sizeStart]){						

						// find received data's owner's socket
						int userIdx = findUserIdx(events_all[i].data.fd, uiArr, clntCount);
						int roomIdx = 0;
						int sendSize = 0;

						// find room that has spare room space
						// if user's roomIdx is "-1", that user is not in specific room
						if(uiArr[userIdx].roomIdx == -1){
							roomIdx = findSpareRoom(room);

							if(room[roomIdx].numOfClient == 0 && room[roomIdx].mapSeqMade == 0){
								for(q=0 ; q<8 ; q++)
									room[roomIdx].mapSeq[q] = rand()%9;
								room[roomIdx].mapSeqMade = 1;
							}

							printf("spare room idx : %d\n", roomIdx);
							// save userInfo in specific room
							buffer[menu.sizeStart + 1+2] = '\0';
							if(!strncmp("OK", &buffer[menu.sizeStart + 1], 2)){						
								joinRoom(room, roomIdx, &(uiArr[userIdx]));								

								continue;
							}

							// in temp buffer, "Start" + totalSize + num of Client + map sequence
							memset(temp, 0, sizeof(char)*BUF_SIZE);
							// copy "Start" in temp
							strncpy(temp, menu.start, menu.sizeStart);
							// total Size
							totalSize = menu.sizeStart+ sizeof(totalSize) + sizeof(room[roomIdx].numOfClient)+sizeof(mapSeq);
							//printf("totalSize = %d\n", totalSize);
							memcpy(temp+menu.sizeStart, &totalSize, sizeof(totalSize));
							// copy numOfClient in temp
							memcpy(temp+menu.sizeStart+sizeof(totalSize), &(room[roomIdx].numOfClient),
									 sizeof(room[roomIdx].numOfClient));							
							
							// copy map sequence in temp
							memcpy(temp+menu.sizeStart+sizeof(totalSize)+sizeof(room[roomIdx].numOfClient),
								room[roomIdx].mapSeq, sizeof(int)*8);
							for(q=0 ; q<8 ;q++){
								printf("%d ", room[roomIdx].mapSeq[q]);
							}printf("\n");

							sendSize = send(events_all[i].data.fd, temp,
								menu.sizeStart+ sizeof(totalSize) +sizeof(room[roomIdx].numOfClient)+sizeof(mapSeq), 0);
							printf("%d sendSize to %s\n", sendSize, uiArr[userIdx].name);
							
						}else{
							roomIdx = uiArr[userIdx].roomIdx;
						}
						
						if(room[roomIdx].numOfClient > 0){
							// send data to others who are in same room
							sendDataToRoomMember(uiArr[userIdx].sock, room, roomIdx, buffer, readSize);
						}
						
					}
					else if(MENU_RANK == buffer[menu.sizeStart]){
				
					}
					else if(MENU_EXIT == buffer[menu.sizeStart]){
						printf("Someone Exit this game\n");
						int userIdx = findUserIdx(events_all[i].data.fd, uiArr, clntCount);

						if(isUserInRoom(room, uiArr, events_all[i].data.fd, clntCount)){							
							sendDataToRoomMember(events_all[i].data.fd, room, uiArr[userIdx].roomIdx,
									buffer, readSize);				
							removeFromRoom(room, uiArr, userIdx);			
						}
						
						printf("room 0 's number : %d\n", room[0].numOfClient);
					}					
				}

			}
		}
	}

	close(sock_serv);
	close(fd_epoll);
	return 0;
}

void error_terminating(char *message){
	printf("%s\n", message);
	exit(1);
}


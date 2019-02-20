#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#define MENU_ID 0
#define MENU_SINGLE 1
#define MENU_MULTI 2
#define MENU_RANK 3
#define MENU_EXIT 4

typedef struct{
	const char *start;
	//const char *exit;

	const char ID;
	const char single;
	const char multi;
	const char rank;
	const char exit;
		
	const short sizeStart;	
}MainMenu;


#endif

#ifndef __USER_H__
#define __USER_H__

#include "Types.h"

typedef struct kUserStruct
{
    char userID [ 16 ];
    char userPasswd [ 16 ];

} USERINFO;

BOOL readUserInfo();
BOOL checkUserInfo(const char* userID, const char* userPasswd);
BOOL addUser( const char* userID, const char* userPasswd );
void deleteUser( const char* userID);
static BOOL changeUser( const char* userID, const char* userPasswd);
void setPasswd( const char* userID, const char* newPasswd);
BOOL isPrePasswdExist(const char* userID);
int getUserTableIndex(const char* userID);
BOOL firstUser();
void showTable();

#endif /*__USER_H__*/
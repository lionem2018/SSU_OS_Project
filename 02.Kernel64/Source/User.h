#ifndef __USER_H__
#define __USER_H__

#include "Types.h"

typedef struct kUserStruct
{
    char userID [ 16 ];
    char userPasswd [ 16 ];
    
} USERINFO;

static BOOL addUser( const char* userID, const char* userPasswd );
static BOOL deleteUser( const char* userID, const char* userPasswd );
static BOOL changeUser( const char* userID, const char* userPasswd);
static BOOL setPasswd( const char* userPasswd);

#endif /*__USER_H__*/
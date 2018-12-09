#include "User.h"
#include "Utility.h"
#include "FileSystem.h"

static USERINFO userTable[10];
static int tableIndex;

BOOL readUserInfo(){
    char tmp[3]={0};
    char id[16]={0};
    char password[16]={0};
    int stringIndex=0;
    int nowId=1;

    FILE* pstFile = kOpenFile(".userinfo", 'r', "root");
    while(kReadFile(tmp, 1, 1,pstFile)){
        if(tmp[0]=='/'){
            stringIndex=0;
            nowId^=1;
        }
        else if(tmp[0]=='\n'){
        stringIndex=0;
            tableIndex++;
            nowId^=1;
        }
        else{
            if(nowId==1)
                userTable[tableIndex].userID[stringIndex++]=tmp[0]; 
            else
                userTable[tableIndex].userPasswd[stringIndex++]=tmp[0];           
        }
    }

    kCloseFile(pstFile);

    for(int i=0;i<=tableIndex;i++){
        kPrintf("%s ",userTable[i].userID);
        kPrintf("%s\n",userTable[i].userPasswd);
    }
    return FALSE;
}

BOOL checkUserInfo(const char* userID, const char* userPasswd){
    BOOL ok=FALSE;
    for(int i=0;i<=tableIndex;i++){
        if(kStrCmp(userID,userTable[i].userID)){
            if(kStrCmp(userPasswd,userTable[i].userPasswd))
                ok=TRUE;
            else if((userTable[i].userPasswd[0]==0)&&(userPasswd[0]==0))
                ok=TRUE;
        }
    }
    return ok;
}

BOOL addUser( const char* userID, const char* userPasswd ){
    BOOL only=TRUE;
    for(int i=0;i<=tableIndex;i++){ //no duplication
        if(kStrCmp(userID,userTable[i].userID)){
            only=FALSE;
            break;
        }
    }

    if(only==FALSE) return FALSE;
    else{
        char idPass[35]={0};
        idPass[0]='\n';
        kStrCpy(idPass+1,userID);
        int i=kStrLen(userID)+1;
        idPass[i++]='/';
        kStrCpy(idPass+i,userPasswd);
        // kPrintf("\n%s\n",idPass);

        FILE* pstFile = kOpenFile(".userinfo", 'a', "root");
        kSeekFile(pstFile,0,FILESYSTEM_SEEK_END);
        kWriteFile(idPass,kStrLen(idPass),1,pstFile);
        kStrCpy(userTable[tableIndex].userID,userID);
        kStrCpy(userTable[tableIndex++].userPasswd,userPasswd);

        kCloseFile(pstFile);

        return TRUE;
    }
}

static BOOL deleteUser( const char* userID, const char* userPasswd ){
    return FALSE;
}

// static BOOL changeUser( const char* userID, const char* userPasswd){
    
//     return FALSE;
// }

static BOOL setPasswd( const char* userPasswd){
    return FALSE;
}
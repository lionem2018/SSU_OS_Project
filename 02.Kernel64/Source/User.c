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


    initializeTable();

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
        kStrCpy(userTable[++tableIndex].userID,userID);
        kStrCpy(userTable[tableIndex].userPasswd,userPasswd);

        kCloseFile(pstFile);

        return TRUE;
    }
}

void deleteUser( const char* userID){
    kRemoveFile(".userinfo");
    FILE* pstFile = kOpenFile(".userinfo", 'w', "root");
        
    for(int i=0;i<=tableIndex;i++){
        if(!kStrCmp(userID, userTable[i].userID)){
        char idPass[35]={0};
        char*now=idPass;
        if(i!=0||(kStrCmp(userID, userTable[i].userID)&&i==1)){
            idPass[0]='\n';
            now++;
        }
        kStrCpy(now,userTable[i].userID);
        now+=kStrLen(userTable[i].userID);
        *now='/';
        now++;
        kStrCpy(now,userTable[i].userPasswd);

        kWriteFile(idPass,kStrLen(idPass),1,pstFile);
        }
    }
    kCloseFile(pstFile);

    readUserInfo();
}

void initializeTable(){
    for(int i=0;i<=tableIndex;i++){
        for(int j=0;j<16;j++){
            userTable[i].userID[j]=0;
            userTable[i].userPasswd[j]=0;
        }
    }
    tableIndex=0;
}


void showTable(){
    for(int i=0;i<=tableIndex;i++){
        kPrintf("%s ",userTable[i].userID);
        kPrintf("%s\n",userTable[i].userPasswd);
    }
}

void setPasswd( const char* userID, const char* newPasswd){
    int idx = getUserTableIndex(userID);

    kStrCpy(userTable[idx].userPasswd,newPasswd);

    FILE* pstFile = kOpenFile(".userinfo", 'w', "root");
        
    for(int i=0;i<=tableIndex;i++){
        char idPass[35]={0};
        char*now=idPass;
        if(i!=0){
            idPass[0]='\n';
            now++;
        }
        kStrCpy(now,userTable[i].userID);
        now+=kStrLen(userTable[i].userID);
        *now='/';
        now++;
        kStrCpy(now,userTable[i].userPasswd);

        kWriteFile(idPass,kStrLen(idPass),1,pstFile);
    }

    kCloseFile(pstFile);
}

int getUserTableIndex(const char* userID){
    int nowUserIdx;
    for(int i=0;i<=tableIndex;i++){ 
        if(kStrCmp(userID,userTable[i].userID)){
            nowUserIdx=i;
            break;
        }
    }
    return nowUserIdx;
}

BOOL isPrePasswdExist(const char* userID){
    int idx=getUserTableIndex(userID);
    if(userTable[idx].userPasswd[0]) return TRUE;
    else return FALSE;
}

BOOL firstUser(){
    if(userTable[0].userID[0]==0) return TRUE;
    else return FALSE;
}
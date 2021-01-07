#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include"mysql/mysql.h"

#define address "localhost"
#define dbusr ""
#define dbpwd ""
#define dbname "chatlogs"

//用户注册
bool dbregist(char username[15],char password[15]){
    MYSQL *conn_ptr = mysql_init(NULL);
    conn_ptr = mysql_real_connect(conn_ptr,address,dbusr,dbpwd,dbname,0,NULL,0);
    MYSQL_RES *res_ptr = NULL;
    int res;
    char que[100];
    sprintf(que,"insert into chatlogs.user values('%s','%s')",username,password);
    res = mysql_query(conn_ptr,que);
    if(res){
        return 0;    //失败
    }else{
        return 1;    //成功
    }
    mysql_close(conn_ptr);
}
//用户登录验证
int  dblogin(char username[15],char password[15]){
    MYSQL *conn_ptr = mysql_init(NULL);
    conn_ptr = mysql_real_connect(conn_ptr,address,dbusr,dbpwd,dbname,0,NULL,0);

    MYSQL_RES *res_ptr = NULL;
    MYSQL_ROW sqlrow;
    char que[100];
    sprintf(que,"select password from chatlogs.user where username = '%s'",username);    //生成sql语句
    mysql_query(conn_ptr,que);    //执行sql语句
    res_ptr = mysql_store_result(conn_ptr);    //返回执行结果

    if(mysql_num_rows(res_ptr) == 0){    //如果没有该用户就注册
        dbregist(username,password);
        return 4;    //注册成功
    }else{
        sqlrow = mysql_fetch_row(res_ptr);    //结果集传给sqlrow (sqlrow[0] 二维数组形式读取)
        if(strcmp(sqlrow[0], password) == 0){
            return 0;    //登陆成功
        }else{
            return 1;    //密码错误
        }
    }
    mysql_close(conn_ptr);
}
//插入聊天记录
bool insertMsg(char sender[15],char recipient[15],char msg[100]){
    MYSQL *conn_ptr = mysql_init(NULL);
    conn_ptr = mysql_real_connect(conn_ptr,address,dbusr,dbpwd,dbname,0,NULL,0);

    MYSQL_RES *res_ptr = NULL;
    char que[100];
    sprintf(que,"insert into chatlogs.msg values(null,'%s','%s','%s',curtime())",sender,recipient,msg);
    mysql_query(conn_ptr,que);
    res_ptr = mysql_use_result(conn_ptr);    //返回执行结果
    if(res_ptr){
        return 1;
    }else{
        return 0;
    }
    mysql_close(conn_ptr);
}
//打印聊天记录
char *printMsg(char sender[15],char recipient[15]){
    MYSQL *conn_ptr = mysql_init(NULL);
    conn_ptr = mysql_real_connect(conn_ptr,address,dbusr,dbpwd,dbname,0,NULL,0);
    
    MYSQL_RES *res_ptr = NULL;
    MYSQL_ROW sqlrow;

    char que[200];
    char msgpre[40][100];
    static char msg[4000];
    int i = 0;
    memset(msgpre, 0, sizeof(char)*4000);
    memset(msg, 0, sizeof(char)*4000);

    sprintf(que,"select * from chatlogs.msg where ((username1 = '%s' and username2 = '%s')or(username1 = '%s' and username2 = '%s'));",sender,recipient,recipient,sender);
    mysql_query(conn_ptr,que);
    res_ptr = mysql_store_result(conn_ptr);
    int a = mysql_num_rows(res_ptr);
    while(sqlrow = mysql_fetch_row(res_ptr)){
        sprintf(msgpre[i],"%s %s %s %s\n",sqlrow[1],sqlrow[2],sqlrow[3],sqlrow[4]);
        i++;
    }
    for(i=0;i<a;i++){
        strcat(msg,msgpre[i]);
    }
    //printf("%s",msg);
    mysql_close(conn_ptr);
    return msg;
}
//插入群聊聊天记录
bool insertGroupMsg(long groupid,char sender[15],char msg[100]){
    MYSQL *conn_ptr = mysql_init(NULL);
    conn_ptr = mysql_real_connect(conn_ptr,address,dbusr,dbpwd,dbname,0,NULL,0);

    MYSQL_RES *res_ptr = NULL;
    char que[100];
    sprintf(que,"insert into chatlogs.groupmsg values(null,'%ld','%s','%s',curtime())",groupid,sender,msg);
    mysql_query(conn_ptr,que);
    res_ptr = mysql_use_result(conn_ptr);    //返回执行结果
    if(res_ptr){
        return 1;
    }else{
        return 0;
    }
    mysql_close(conn_ptr);
}
//打印群聊聊天记录
char *printGroupMsg(long groupid){
    MYSQL *conn_ptr = mysql_init(NULL);
    conn_ptr = mysql_real_connect(conn_ptr,address,dbusr,dbpwd,dbname,0,NULL,0);
    
    MYSQL_RES *res_ptr = NULL;
    MYSQL_ROW sqlrow;

    char que[200];
    char msgpre[40][100];
    static char msg[4000];
    int i = 0;
    memset(msgpre, 0, sizeof(char)*4000);
    memset(msg, 0, sizeof(char)*4000);

    sprintf(que,"select * from chatlogs.groupmsg where groupid = %ld;",groupid);
    mysql_query(conn_ptr,que);
    res_ptr = mysql_store_result(conn_ptr);
    int a = mysql_num_rows(res_ptr);
    while((sqlrow = mysql_fetch_row(res_ptr))){
        sprintf(msgpre[i],"%s %s %s %s\n",sqlrow[1],sqlrow[2],sqlrow[3],sqlrow[4]);
        i++;
    }
    for(i=0;i<a;i++){
        strcat(msg,msgpre[i]);
    }
    //printf("%s",msg);
    mysql_close(conn_ptr);
    return msg;
}
// 测试函数
// int main(){
//     // printf("%d",dbregist("7777","444"));
//     // printf("%d",dblogin("7","44aa4"));
//     // insertMsg("haizhong","lingjun","nihaoyaaaaaa");
//     // char *ch = printMsg("haizhong","lingjun");
//     // printf("%s",ch);
//     // char *p = printGroupMsg(12377);
//     // printf("%s",p);
//     // printf("%d",insertGroupMsg(12377,"nihaoyaaaaaa","inniin"));
// }
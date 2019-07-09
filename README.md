# 数据库接口

本部分主要为聊天系统中需要进行数据存储的部分提供支持  
主要功能为：帐号管理，私聊聊天记录管理，群聊聊天记录管理  

用户表有 username password 两个字段，用于存储账号密码  
聊天记录表有 sender recipient message time 四个字段，用于存储聊天记录的发送者、接收者、信息内容、发送时间  
群聊天记录表有 groupid sender message time 四个字段，用于存储群聊聊天记录的群号、发送者、信息内容、发送时间  

接口分别是  
1.用户注册  bool dbregist(char username[15],char password[15])  
    （server 中调用该函数以插入相应用户并返回值来验证是否插入成功）
2.用户登陆验证  int  dblogin(char username[15],char password[15])  
    （server 中调用该函数并返回值来验证是否允许登录，返回0登陆成功，1密码错误）
3.插入聊天记录  bool insertMsg(char sender[15],char recipient[15],char msg[100])  
    （server 中调用该函数以插入相应聊天记录并返回值来验证是否插入成功）
4.打印聊天记录  char *printMsg(char sender[15],char recipient[15])  
    （server 中调用该函数以返回与某人之间的聊天记录）
5.插入群聊聊天记录  bool insertGroupMsg(long groupid,char sender[15],char msg[100])  
    （server 中调用该函数以插入相应聊天记录并返回值来验证是否插入成功）
6.打印群聊聊天记录  char *printGroupMsg(long groupid)  
    （server 中调用该函数以返回相应群号内的聊天记录）

## 注意事项
用户名不大于15字节  
密码不大于15字节  
单条聊天记录不大于100字节 

## 建库数据
create datebase chatlogs;

## 建表数据

#### 用户表
create table user(  
    username varchar(20),  
    password varchar(20)
)  

#### 聊天记录表
create table msg(  
    id int primary key auto_increment,  
    sender varchar(20),  
    recipient varchar(20),  
    message varchar(100),  
    time datetime default current_timestamp
)

#### 群聊天记录表
create table groupmsg(
    id int primary key auto_increment,  
    groupid varchar(20),  
    sender varchar(20),  
    message varchar(20),  
    time datetime default current_timestamp
)
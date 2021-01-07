#include "mymsg.h"
#include "dbop.h"

struct clifd{
    char name[12];
    int fd;
    long group;
};
sem_t mysem;

struct clifd myfd[NUM];
int my_fd_i = 0, res = 0, i;
char currentDirPath[200];

int set_nonblocking(int fd){
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/*私聊*/
void prichat(msg mymsg, int comm_sock){
    int msglen = sizeof(mymsg);
    sem_wait(&mysem);
    for (i = 0; i < my_fd_i; i++){
        if (strcmp(myfd[i].name, mymsg.sendto) == 0){
            sem_post(&mysem);
            write(myfd[i].fd, &mymsg, msglen);
            insertMsg(mymsg.me, mymsg.sendto, mymsg.mess);
            sem_wait(&mysem);
            return;
        }
    }
    printf("未找到该用户！\n");
    mymsg.command = -1;
    strcpy(mymsg.mess, "未找到该用户！");
    write(comm_sock, &mymsg, msglen);
    sem_post(&mysem);
    return;
}

/*在群组中发消息*/
void sendtogroup(msg mymsg){
    int i = 0;
    long groupid;
    int msglen;
    msglen = sizeof(msg);
    groupid = mymsg.groupid;
    sem_wait(&mysem);
    for (; i < my_fd_i; i++){
        if (strcmp(myfd[i].name, mymsg.me) == 0)
            continue;
        if (myfd[i].group == groupid){
            printf("群组成员:%s,fd:%d\n", myfd[i].name, myfd[i].fd);
            write(myfd[i].fd, &mymsg, msglen);
            printf("用户名:%s,群组号:%ld,内容:%s\n",
                   mymsg.me, mymsg.groupid, mymsg.mess);
        }
    }
    sem_post(&mysem);
    return;
}

/*加入一个群组*/
void addtogroup(msg mymsg){
    sem_wait(&mysem);
    for (i = 0; i < my_fd_i; i++){
        if (strcmp(myfd[i].name, mymsg.me) == 0){
            myfd[i].group = mymsg.groupid;
            printf("\n用户%s已加入群组%ld\n", myfd[i].name, mymsg.groupid);
        }
    }
    for (i = 0; i < my_fd_i; i++){
        printf("Name:%s,Fd;%d,Group:%ld\n",
               myfd[i].name, myfd[i].fd, myfd[i].group);
    }
    sem_post(&mysem);
}

/*文件上传*/
void upload(msg mymsg){
    int fd;
    long fileSize;
    char filePath[200], buf[dataLen];
    memset(currentDirPath, 0, sizeof(currentDirPath));
    getcwd(currentDirPath, sizeof(currentDirPath));
    strcpy(filePath, currentDirPath);
    strcat(filePath, "/");
    strcat(filePath, mymsg.fileName);
    fd = open(filePath, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
    if (fd != -1){
        memset(buf, 0, dataLen);
        fileSize = mymsg.fileSize;
        //read(sock,&fileSize,sizeof(long));

        while (fileSize > dataLen){
            strncpy(buf, mymsg.buf, dataLen);
            //read(sock,buf,dataLen);
            write(fd, buf, dataLen);
            fileSize = fileSize - dataLen;
        }
        strncpy(buf, mymsg.buf, fileSize);
        //read(sock,buf,fileSize);
        write(fd, buf, fileSize);
        close(fd);
        printf("接收文件成功！\n");
        printf("上传用户:%s,文件名:%s,文件大小:%ldBytes\n", mymsg.me, mymsg.fileName, mymsg.fileSize);
    }else{
        printf("上传文件至失败！(目录:%s)\n", filePath);
    }
}

/*文件下载*/
void download(msg mymsg, int comm_sock){
    msg youmsg;
    int msglen;
    int fd;
    struct stat fileSta;
    long fileSize;
    char filePath[200], buf[dataLen];

    getcwd(currentDirPath, sizeof(currentDirPath));
    memset(filePath, 0, sizeof(filePath));
    strcpy(filePath, currentDirPath);
    strcat(filePath, "/");
    strcat(filePath, mymsg.fileName);

    fd = open(filePath, O_RDONLY, S_IREAD);
    strcpy(youmsg.fileName, mymsg.fileName);
    msglen = sizeof(youmsg);
    if (fd != -1){
        fstat(fd, &fileSta);
        fileSize = (long)fileSta.st_size;
        youmsg.fileSize = fileSize;
        //write(sock,&fileSize,sizeof(long));
        memset(buf, 0, dataLen);
        while (fileSize > dataLen){
            read(fd, buf, dataLen);
            strncpy(youmsg.buf, buf, dataLen);
            //write(sock,buf,dataLen);
            fileSize = fileSize - dataLen;
        }

        read(fd, buf, fileSize);
        strncpy(youmsg.buf, buf, dataLen);
        //write(sock,buf,fileSize);
        close(fd);
        youmsg.command = 5;
        write(comm_sock, &youmsg, msglen);
        printf("用户%s下载文件%s成功!\n", mymsg.me, mymsg.fileName);
    }else{
        printf("查找文件失败！(目录：%s)\n", filePath);
        youmsg.command = -5;
        write(comm_sock, &youmsg, msglen);
    }
}

/*用户注册*/
/*void regist(msg mymsg,int comm_sock)
{
    int msglen=sizeof(mymsg);
    if(dbregist(mymsg.me,mymsg.pwd)==1){
    printf("注册失败！\n");
    strcpy(mymsg.mess,"注册失败！");
    write(comm_sock,&mymsg,msglen);
  }else{
  printf("注册成功！\n");
  strcpy(mymsg.mess,"注册成功！");
  write(comm_sock,&mymsg,msglen);
    //break;
  }
}*/

/*用户登陆*/
/*访问数据库验证用户名和密码,正确res=0，错误res=1,重复res=2,满载res=3,注册res=4*/
void ser_login(msg mymsg, int comm_sock){
    sem_wait(&mysem);
    if (my_fd_i == NUM){ //判断服务器是否满载
        res = 3;
        write(comm_sock, "3", 1);
        pthread_exit("服务器满载！");
        // break;
    }
    for (i = 0; i < my_fd_i; i++){ //判断是否重名
        if (strcmp(mymsg.me, myfd[i].name) == 0){
            res = 2;
            write(comm_sock, "2", 1);
            //   pthread_exit("该用户已登陆！");
            break;
        }
    }

    res = dblogin(mymsg.me, mymsg.pwd); //从数据库 验证用户名和密码
    printf("res=%d\n", res);
    if (res == 0){ //登陆成功
        write(comm_sock, "0", 1);
        strncpy(myfd[my_fd_i].name, mymsg.me, mymsg.melen);
        myfd[my_fd_i].fd = comm_sock;
        my_fd_i++;
        if (my_fd_i == NUM){
            printf("服务器已满载！\n");
        }
        printf("%s%s\n", space, space);
        printf("已连接一个客户端,列表：\n");
        for (i = 0; i < my_fd_i; i++){
            printf("name:%s,fd:%d\n", myfd[i].name, myfd[i].fd);
        }
    }else if (res == 4){
        printf("已注册用户:%s,密码为:%s\n\n", mymsg.me, mymsg.pwd);
        write(comm_sock, "4", 1);
    }else if (res == 1){
        printf("用户名和密码不匹配，登陆失败！\n");
        write(comm_sock, "1", 1);
        //pthread_exit("用户名与密码不匹配！");
        // continue;
    }else
        printf("系统错误！请重试！\n");
    sem_post(&mysem);
    //break;
}

/*用户退出*/
void logout(msg mymsg, int comm_sock){
    int msglen = sizeof(mymsg);
    sem_wait(&mysem);
    write(comm_sock, &mymsg, msglen);
    printf("断开的客户端：name:%s\n", mymsg.me);
    //删除将退出的客户端的内容及fd
    for (i = 0; i < my_fd_i; i++){
        if (strcmp(myfd[i].name, mymsg.me) == 0){
            // printf("name:%s me:%s",myfd[i].name,mymsg.me);
            for (int j = i; j < my_fd_i - 1; j++){
                myfd[j].fd = myfd[j + 1].fd;
                memset(&myfd[j].name, '\0', 12);
                strncpy(myfd[j].name, myfd[j + 1].name,
                        strlen(myfd[j + 1].name));
            }
        }
    }
    my_fd_i--;
    close(comm_sock);
    for (i = 0; i < my_fd_i; i++){
        printf("在线的客户端：name:%s,fd:%d\n", myfd[i].name, myfd[i].fd);
    }
    sem_post(&mysem);
    pthread_exit("ok");
}

/*打印聊天记录*/
void ser_printmsg(msg mymsg, int comm_sock){
    printf("发信人 收信人 信息内容 时间\n");
    char *msg = printMsg(mymsg.me, mymsg.sendto);
    printf("%s",msg);
    mymsg.command=6;
    strncpy(mymsg.mess,msg,dataLen);
    write(comm_sock,&mymsg,dataLen);
}

void *comm_cli_thread(void *p){
    //char buf:fer[NUM];
    msg mymsg;
    int msglen;
    //int pairfd;
    int i, res;
    int comm_sock = *(int *)p;
    msglen = sizeof(msg);
    char NAME[12];
    while (1){
        //res=0;
        memset(&mymsg, '\0', msglen);
        printf("%s%s\n",space,space);
        if ((res = read(comm_sock, &mymsg, msglen)) == 0){
            logout(mymsg, comm_sock);
        }else
            printf("read %d\n", res);
        
        printf("系统日志:\n");
        printf("用户名:%s,命令:%d,聊天对象:%s,内容: %s\n\n",
               mymsg.me, mymsg.command, mymsg.sendto, mymsg.mess);

        switch (mymsg.command){
            case 0:
                logout(mymsg, comm_sock);
                break;
            case 1: //private chat
                prichat(mymsg, comm_sock);
                break;
            case 2:
                sendtogroup(mymsg);
                break;
            case 3: //add to group
                addtogroup(mymsg);
                break;
            case 4:
                upload(mymsg);
                break;
            case 5:
                download(mymsg, comm_sock);
                break;
            /*case 6:
                regist(mymsg,comm_sock);
                break;*/
            case 6:
                ser_printmsg(mymsg, comm_sock);
                break;
            case 9: //login
                ser_login(mymsg, comm_sock);
                break;
        }
    }
}

int main(){
    struct sockaddr_in ser_addr, cli_addr;
    int ser_sock, cli_sock;
    int len;
    pthread_t tid;
    sem_init(&mysem, 0, 1);
    ser_sock = socket(AF_INET, SOCK_STREAM, 0);
    //set_nonblocking(ser_sock);
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ser_addr.sin_port = 9765;
    len = sizeof(ser_addr);
    bind(ser_sock, (struct sockaddr *)&ser_addr, len);
    listen(ser_sock, 5);

    while (1){
        printf("等待客户端连接...\n");
        len = sizeof(cli_addr);
        cli_sock = accept(ser_sock, (struct sockaddr *)&cli_addr, &len);
        //set_nonblocking(cli_sock);
        pthread_create(&tid, 0, comm_cli_thread, (void *)&cli_sock);
    }
    pthread_join(tid, (void *)NULL);
    sem_destroy(&mysem);
    exit(0);
}

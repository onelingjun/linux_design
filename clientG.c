#include "mymsg.h"
#include <gtk/gtk.h>

long GROUPID;
static int comm_sock;
static char myname[20];

struct login_widget{
	GtkWidget *label_tips; //用于信息提示的标签
	GtkWidget *entry_username;
	GtkWidget *entry_pwd;
} wgt;

struct widget_1{
	GtkWidget *sendto_name;
	GtkWidget *sendto_mess;
} wgt_1;

GtkWidget *window;  //登录窗口
GtkWidget *window_1;  //分支窗口
GtkWidget *window_2;
GtkWidget *window_3;
GtkWidget *window_4;
GtkWidget *window_5;
GtkWidget *window_6;
GtkWidget *window_select; //选择窗口
GtkTextBuffer *buffers;
GtkWidget *entryname;
GtkTextBuffer *bufferuser; //文本框缓冲区
GtkTextBuffer *bufferuser_1;
GtkTextBuffer *bufferuser_2;
char USER[10];
char SEND[10];
char space_much[1024];

void printMsg(GtkWidget *button_search, gpointer group_entry);
void sendto_group(GtkWidget *send_all, gpointer send_entry);
void addto_group(GtkWidget *add_group, gpointer group_entry);
void go_download(GtkWidget *button_download, gpointer filename);
void go_upload(GtkWidget *button_upload, gpointer filename);
void download(msg sendmsg);
void upload(msg sendmsg, int comm_sock);
void send_to(GtkWidget *bsend, gpointer data);
void deal_pressed(GtkWidget *button, gpointer data);    //处理按钮事件
int login(int comm_sock, const char name[12], const char pwd[12]);

void homepage_1(int argc, char *argv[]);
void homepage_2(int argc, char *argv[]);
void homepage_3(int argc, char *argv[]);
void homepage_4(int argc, char *argv[]);
void homepage_5(int argc, char *argv[]);
void homepage_6(int argc, char *argv[]);

void go_homepage_1(GtkWidget *button1, gpointer entry){    //回调函数用于执行命令
	homepage_1(2, entry);
}
void go_homepage_2(GtkWidget *button2, gpointer entry){
	homepage_2(2, entry);
}
void go_homepage_3(GtkWidget *button3, gpointer entry){
	homepage_3(2, entry);
}
void go_homepage_4(GtkWidget *button4, gpointer entry){
	homepage_4(2, entry);
}
void go_homepage_5(GtkWidget *button5, gpointer entry){
	homepage_5(2, entry);
}
void go_homepage_6(GtkWidget *button6, gpointer entry){
	homepage_6(2, entry);
}

void *read_thread(void *p){
	int comm_sock = *(int *)p;
	msg mymsg;
	int msglen;
	char buffer[32];
	msglen = sizeof(msg);
	while (1){
		printf("read_thread is coming\n");
		memset(&mymsg, '\0', msglen);
		read(comm_sock, &mymsg, msglen);
		if (mymsg.command == 1){
			printf("command=1");
			strcpy(USER, "User:");
			strcpy(SEND, "---Msg:");
			strcat(USER, mymsg.me);
			strcat(USER, SEND);
			strcat(USER, mymsg.mess);
			strcpy(buffer, USER);
			GtkTextIter start, end;												    //新建保存文字在buffer中位置的结构start和end。
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser), &start, &end);	//得到当前buffer中开始位置，结束位置的ITER。
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser), &start, buffer, strlen(buffer));
			// printf("222\n");
		}else if (mymsg.command == 5){
			download(mymsg);
		}else if (mymsg.command == 2){
			printf("command=2");
			strcpy(USER, "User:");
			strcpy(SEND, "---Msg:");
			strcpy(space_much, "\n");
			strcat(USER, mymsg.me);
			strcat(USER, SEND);
			strcat(USER, mymsg.mess);
			strcat(USER, space_much);
			strcpy(buffer, USER);
			GtkTextIter start, end;													 //新建保存文字在buffer中位置的结构start和end。
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser_1), &start, &end); //得到当前buffer中开始位置，结束位置的ITER。
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser_1), &start, buffer, strlen(buffer));
		}else if(mymsg.command == 6){
			strcpy(space_much,mymsg.mess);
			GtkTextIter start, end;
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser_2), &start, &end);
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser_2), &start, space_much, strlen(space_much));
		}else{
			printf("%d\n", mymsg.command);
		}
	}
}
//登录（界面）
void login_main(int argc, char *argv[]){
	// 初始化
	gtk_init(&argc, &argv);
	// 创建顶层窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window), "登录");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window, 300, 250);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
	GtkWidget *fixed = gtk_fixed_new();
	// 将布局容器放窗口中
	gtk_container_add(GTK_CONTAINER(window), fixed);
	// 创建标签
	GtkWidget *label_1 = gtk_label_new("请输入昵称");
	GtkWidget *label_2 = gtk_label_new("请输入密码");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed), label_1, 120, 20);
	gtk_fixed_put(GTK_FIXED(fixed), label_2, 120, 100);
	// 行编辑的创建
	//设置最大长度
	wgt.entry_username = gtk_entry_new();
	wgt.entry_pwd = gtk_entry_new();

	gtk_entry_set_max_length(GTK_ENTRY(wgt.entry_username), 50);
	gtk_entry_set_max_length(GTK_ENTRY(wgt.entry_pwd), 50);
	gtk_editable_set_editable(GTK_EDITABLE(wgt.entry_username), TRUE);	// 设置行编辑允许编辑
	gtk_fixed_put(GTK_FIXED(fixed), wgt.entry_username, 70, 50);
	gtk_editable_set_editable(GTK_EDITABLE(wgt.entry_pwd), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), wgt.entry_pwd, 70, 130);
	GtkWidget *button = gtk_button_new_with_label("  登录  ");    // 创建按钮
	gtk_fixed_put(GTK_FIXED(fixed), button, 120, 190);
	g_signal_connect(button, "pressed", G_CALLBACK(deal_pressed), &wgt);    //绑定点击事件
	// 显示窗口全部控件
	gtk_widget_show_all(window);
	//启动主循环
	gtk_main();
}
//处理登录按钮点击事件
void deal_pressed(GtkWidget *button, gpointer data){
	const gchar *name;
	const gchar *pwd;
	struct login_widget *wgt;
	wgt = (struct login_widget *)data;

	struct sockaddr_in ser_addr;
	int len, res;
	long GROUPID;
	char GID[13];
	char buffer[NUM], NAME[13], PASSWORD[13];
	pthread_t tid;
	void *r;
	int msglen;
	msg sendmsg;
	comm_sock = socket(AF_INET, SOCK_STREAM, 0);
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ser_addr.sin_port = 9765;
	len = sizeof(ser_addr);
	if (connect(comm_sock, (struct sockaddr *)&ser_addr, len) == -1){
		printf("%s未连接上服务器%s\n\n", SPACE, SPACE);
		perror("Connect error");
		exit(1);
	}else{
		name = gtk_entry_get_text(GTK_ENTRY(wgt->entry_username));
		pwd = gtk_entry_get_text(GTK_ENTRY(wgt->entry_pwd));
		strncpy(myname, name, strlen(name));
		res = login(comm_sock, name, pwd);
		if (res == 1){
			GtkWidget *dialog;
			dialog = gtk_message_dialog_new((gpointer)window,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"该用户已登陆，请勿重复登陆，拒绝登录！");
			gtk_window_set_title(GTK_WINDOW(dialog), "拒绝");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			close(comm_sock);
		}else if (res == 2){
			gtk_label_set_text(GTK_LABEL(wgt->label_tips), "Invalid user name or password.");
		}else if (res == 3){
			gtk_label_set_text(GTK_LABEL(wgt->label_tips), "Invalid user name or password.");
			exit(1);
		}else if (res == 0){
			printf("res=0\n");
			gtk_widget_destroy(window);
		}else{
			gtk_label_set_text(GTK_LABEL(wgt->label_tips), "Welcome to GTK+-3.0");
		}
	}
}
//登录用户（工具）
int login(int comm_sock, const char name[12], const char pwd[12]){
	int msglen, result;
	char res[1];
	char me[12];
	msg sendmsg;
	msglen = sizeof(msg);
	memset(&sendmsg, '\0', msglen);
	sendmsg.command = 9;
	strncpy(sendmsg.me, name, strlen(name));
	strncpy(sendmsg.pwd, pwd, strlen(pwd));
	sendmsg.melen = strlen(sendmsg.me);
	sendmsg.pwdlen = strlen(sendmsg.pwd);
	write(comm_sock, &sendmsg, msglen);
	memset(&res, '\0', sizeof(res));
	read(comm_sock, &res, 1);
	result = atoi(res);
	return result;
}
//主菜单（界面）
void select_1(int argc, char *argv[]){
	// 初始化
	gtk_init(&argc, &argv);
	// 创建顶层窗口
	window_select = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window_select), "登录");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window_select), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window_select, 200, 400);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window_select), FALSE);
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window_select, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
	GtkWidget *fixed_select = gtk_fixed_new();
	// 将布局容器放窗口中
	gtk_container_add(GTK_CONTAINER(window_select), fixed_select);

	GtkWidget *button1 = gtk_button_new_with_label("  私聊  ");
	GtkWidget *button2 = gtk_button_new_with_label("  群聊  ");
	GtkWidget *button3 = gtk_button_new_with_label("  加群  ");
	GtkWidget *button4 = gtk_button_new_with_label("  上传文件  ");
	GtkWidget *button5 = gtk_button_new_with_label("  下载文件  ");
	GtkWidget *button6 = gtk_button_new_with_label(" 打印聊天记录 ");

	gtk_fixed_put(GTK_FIXED(fixed_select), button1, 50, 30);
	gtk_fixed_put(GTK_FIXED(fixed_select), button2, 50, 90);
	gtk_fixed_put(GTK_FIXED(fixed_select), button3, 50, 150);
	gtk_fixed_put(GTK_FIXED(fixed_select), button4, 50, 210);
	gtk_fixed_put(GTK_FIXED(fixed_select), button5, 50, 270);
	gtk_fixed_put(GTK_FIXED(fixed_select), button6, 50, 330);
	//绑定点击事件
	g_signal_connect(button1, "pressed", G_CALLBACK(go_homepage_1), NULL);
	g_signal_connect(button2, "pressed", G_CALLBACK(go_homepage_2), NULL);
	g_signal_connect(button3, "pressed", G_CALLBACK(go_homepage_3), NULL);
	g_signal_connect(button4, "pressed", G_CALLBACK(go_homepage_4), NULL);
	g_signal_connect(button5, "pressed", G_CALLBACK(go_homepage_5), NULL);
	g_signal_connect(button6, "pressed", G_CALLBACK(go_homepage_6), NULL);
	// 显示窗口全部控件
	gtk_widget_show_all(window_select);
	//启动主循环
	gtk_main();
}
//私聊（界面）
void homepage_1(int argc, char *argv[]){
	char *buf;
	buf = (char *)malloc(1024);
	memset(buf, 0, 1024);
	pid_t pid;
	// 初始化
	gtk_init(&argc, &argv);
	// 创建顶层窗口
	window_1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window_1), "欢迎来到私聊");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window_1), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window_1, 330, 550);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window_1), FALSE);
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window_1, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
	GtkWidget *fixed_1 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_1), fixed_1);

	GtkWidget *label_two;
	GtkWidget *label_one;
	// 创建标签
	label_one = gtk_label_new("聊天内容：");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed_1), label_one, 20, 10);

	label_two = gtk_label_new("发送用户：");
	gtk_fixed_put(GTK_FIXED(fixed_1), label_two, 20, 400);

	label_two = gtk_label_new("发送内容：");
	gtk_fixed_put(GTK_FIXED(fixed_1), label_two, 20, 430);

	// 行编辑的创建

	wgt_1.sendto_name = gtk_entry_new();
	wgt_1.sendto_mess = gtk_entry_new();

	gtk_entry_set_max_length(GTK_ENTRY(wgt_1.sendto_name), 500);
	gtk_entry_set_max_length(GTK_ENTRY(wgt_1.sendto_mess), 500);
	// 设置行编辑允许编辑
	gtk_editable_set_editable(GTK_EDITABLE(wgt_1.sendto_name), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed_1), wgt_1.sendto_name, 85, 400);
	gtk_editable_set_editable(GTK_EDITABLE(wgt_1.sendto_mess), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed_1), wgt_1.sendto_mess, 85, 430);

	// 创建按钮
	GtkWidget *bsend = gtk_button_new_with_label("发送");
	gtk_fixed_put(GTK_FIXED(fixed_1), bsend, 270, 430);

	GtkWidget *save = gtk_button_new_with_label("保存记录");
	gtk_fixed_put(GTK_FIXED(fixed_1), save, 240, 10);

	// 绑定回调函数
	g_signal_connect(bsend, "pressed", G_CALLBACK(send_to), &wgt_1);
	// g_signal_connect(save, "pressed", G_CALLBACK(savetxt), entry);

	// 文本框聊天窗口
	GtkWidget *view = gtk_text_view_new();
	gtk_widget_set_size_request(view, 230, 230);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed_1), view, 20, 50);
	// 获取文本缓冲区
	bufferuser = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

	// 显示窗口全部控件
	gtk_widget_show_all(window_1);
	gtk_main();
}
//消息发送
void send_to(GtkWidget *bsend, gpointer data){
	int msglen;
	msg sendmsg;
	const gchar *sendto_1;
	const gchar *mess_1;
	msglen = sizeof(msg);
	struct widget_1 *wgt_1;
	wgt_1 = (struct widget_1 *)data;
	memset(&sendmsg, '\0', msglen);
	sendmsg.command = 1;
	strncpy(sendmsg.me, myname, strlen(myname));
	sendto_1 = gtk_entry_get_text(GTK_ENTRY(wgt_1->sendto_name));
	mess_1 = gtk_entry_get_text(GTK_ENTRY(wgt_1->sendto_mess));
	strncpy(sendmsg.sendto, sendto_1, strlen(sendto_1));
	strncpy(sendmsg.mess, mess_1, strlen(mess_1));
	write(comm_sock, &sendmsg, msglen);
}
//群聊（界面）
void homepage_2(int argc, char *argv[]){
	char *buf;
	buf = (char *)malloc(1024);
	memset(buf, 0, 1024);
	pid_t pid;
	// 初始化
	gtk_init(&argc, &argv);
	// 创建顶层窗口
	window_2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window_2), "禁止水群刷屏");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window_2), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window_2, 330, 550);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window_2), FALSE);
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window_2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
	GtkWidget *fixed_2 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_2), fixed_2);

	GtkWidget *label_two;
	GtkWidget *label_one;
	// 创建标签
	label_one = gtk_label_new("聊天内容：");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed_2), label_one, 20, 10);

	label_two = gtk_label_new("发送内容：");
	gtk_fixed_put(GTK_FIXED(fixed_2), label_two, 20, 430);

	GtkWidget *send_entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(send_entry), 500);
	gtk_editable_set_editable(GTK_EDITABLE(send_entry), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed_2), send_entry, 85, 430);

	GtkWidget *send_all = gtk_button_new_with_label("群发");
	gtk_fixed_put(GTK_FIXED(fixed_2), send_all, 270, 430);

	// 绑定回调函数
	g_signal_connect(send_all, "pressed", G_CALLBACK(sendto_group), send_entry);

	// 文本框聊天窗口
	GtkWidget *view = gtk_text_view_new();
	gtk_widget_set_size_request(view, 230, 230);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed_2), view, 20, 50);
	// 获取文本缓冲区
	bufferuser_1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

	// 显示窗口全部控件
	gtk_widget_show_all(window_2);

	gtk_main();
}
//群消息发送
void sendto_group(GtkWidget *send_all, gpointer send_entry){
	int msglen;
	msg sendmsg;
	const gchar *mess_1;
	msglen = sizeof(msg);
	memset(&sendmsg, '\0', msglen);
	sendmsg.groupid = GROUPID;
	sendmsg.command = 2;
	strncpy(sendmsg.me, myname, strlen(myname));
	mess_1 = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)send_entry));
	strncpy(sendmsg.mess, mess_1, strlen(mess_1));
	write(comm_sock, &sendmsg, msglen);
}
//加群（界面）
void homepage_3(int argc, char *argv[]){
	char *buf;
	buf = (char *)malloc(1024);
	memset(buf, 0, 1024);
	pid_t pid;
	// 初始化
	gtk_init(&argc, &argv);
	// 创建顶层窗口
	window_3 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window_3), "欢迎加群");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window_3), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window_3, 250, 170);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window_3), FALSE);
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window_3, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
	GtkWidget *fixed_3 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_3), fixed_3);

	GtkWidget *label_two;

	label_two = gtk_label_new("请输入群号：");
	gtk_fixed_put(GTK_FIXED(fixed_3), label_two, 93, 10);

	// 行编辑的创建
	GtkWidget *group_entry = gtk_entry_new();
	// 最大长度
	gtk_entry_set_max_length(GTK_ENTRY(group_entry), 40);
	gtk_editable_set_editable(GTK_EDITABLE(group_entry), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed_3), group_entry, 40, 40);

	GtkWidget *add_group = gtk_button_new_with_label("Add");
	gtk_fixed_put(GTK_FIXED(fixed_3), add_group, 100, 100);

	// 绑定回调函数
	g_signal_connect(add_group, "pressed", G_CALLBACK(addto_group), group_entry);

	// 显示窗口全部控件
	gtk_widget_show_all(window_3);

	gtk_main();
}
//加群
void addto_group(GtkWidget *add_group, gpointer group_entry){
	int msglen;
	msg sendmsg;
	const gchar *group_id;
	msglen = sizeof(msg);
	memset(&sendmsg, '\0', msglen);
	sendmsg.command = 3;
	strncpy(sendmsg.me, myname, strlen(myname));
	group_id = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)group_entry));
	sendmsg.groupid = strtol(group_id, NULL, 10);
	GROUPID = sendmsg.groupid;
	write(comm_sock, &sendmsg, msglen);
}
//上传文件（界面）
void homepage_4(int argc, char *argv[]){
	// 初始化
	gtk_init(&argc, &argv);
	// 创建顶层窗口
	window_4 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window_4), "上传文件");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window_4), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window_4, 250, 170);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window_4), FALSE);
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window_4, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
	GtkWidget *fixed_4 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_4), fixed_4);

	GtkWidget *label_two;

	label_two = gtk_label_new("请输入上传文件名：");
	gtk_fixed_put(GTK_FIXED(fixed_4), label_two, 75, 10);

	// 行编辑的创建
	GtkWidget *filename = gtk_entry_new();
	// 最大长度
	gtk_entry_set_max_length(GTK_ENTRY(filename), 40);
	gtk_editable_set_editable(GTK_EDITABLE(filename), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed_4), filename, 40, 40);

	GtkWidget *button_upload = gtk_button_new_with_label("上传");
	gtk_fixed_put(GTK_FIXED(fixed_4), button_upload, 100, 100);

	// 绑定回调函数
	g_signal_connect(button_upload, "pressed", G_CALLBACK(go_upload), filename);

	// 显示窗口全部控件
	gtk_widget_show_all(window_4);

	gtk_main();
}
//上传文件
void go_upload(GtkWidget *button_upload, gpointer filename){
	msg sendmsg;
	int msglen = sizeof(msg);
	memset(&sendmsg, '\0', msglen);
	const gchar *filename_1;
	filename_1 = gtk_entry_get_text(GTK_ENTRY(filename));
	strncpy(sendmsg.fileName, filename_1, strlen(filename_1));
	upload(sendmsg, comm_sock);
}
//上传文件（工具）
void upload(msg sendmsg, int comm_sock){
	int fd;
	int msglen;
	long fileSize;
	int numRead;
	char filePath[200];
	struct stat fileSta;

	sendmsg.command = 4;

	memset(filePath, 0, sizeof(filePath));
	getcwd(filePath, sizeof(filePath));

	strcat(filePath, "/");
	strncat(filePath, sendmsg.fileName, strlen(sendmsg.fileName));

	fd = open(filePath, O_RDONLY, S_IREAD);

	if (fd != -1){
		fstat(fd, &fileSta);
		fileSize = (long)fileSta.st_size;
		sendmsg.fileSize = fileSize;
		//write(sockmsg,&fileSize,sizeof(long));
		memset(sendmsg.buf, 0, dataLen);
		while (fileSize > dataLen){
			read(fd, sendmsg.buf, dataLen);
			//write(sockmsg,buf,dataLen);
			sendmsg.fileSize = fileSize;
			fileSize = fileSize - dataLen;
		}

		read(fd, sendmsg.buf, fileSize);
		//strncpy(sendmsg.buf,buf,fileSize);
		//write(sockmsg,buf,fileSize);
		close(fd);
		msglen = sizeof(sendmsg);
		write(comm_sock, &sendmsg, msglen);

		printf("成功\n");
		//GtkWidget *dialog_1;
		// dialog_1 = gtk_message_dialog_new((gpointer)window,
		// GTK_DIALOG_DESTROY_WITH_PARENT,
		// GTK_MESSAGE_ERROR,
		// GTK_BUTTONS_OK,
		// "上传文件成功！");
		// gtk_window_set_title(GTK_WINDOW(dialog_1), "恭喜");
		// gtk_dialog_run(GTK_DIALOG(dialog_1));
		// gtk_widget_destroy(dialog_1);
	}else{
		printf("失败%s\n", filePath);
		// GtkWidget *dialog_2;
		// dialog_2 = gtk_message_dialog_new((gpointer)window_4,
		// GTK_DIALOG_DESTROY_WITH_PARENT,
		// GTK_MESSAGE_ERROR,
		// GTK_BUTTONS_OK,
		// "上传文件失败！");
		// gtk_window_set_title(GTK_WINDOW(dialog_2), "很遗憾");
		// gtk_dialog_run(GTK_DIALOG(dialog_2));
		// gtk_widget_destroy(dialog_2);
	}
}
//下载文件（界面）
void homepage_5(int argc, char *argv[]){
	// 初始化
	gtk_init(&argc, &argv);
	// 创建顶层窗口
	window_5 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window_5), "Download Document");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window_5), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window_5, 250, 170);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window_5), FALSE);
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window_5, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
	GtkWidget *fixed_5 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_5), fixed_5);

	GtkWidget *label_two;

	label_two = gtk_label_new("请输入下载文件名：");
	gtk_fixed_put(GTK_FIXED(fixed_5), label_two, 75, 10);

	// 行编辑的创建
	GtkWidget *filename = gtk_entry_new();
	// 最大长度
	gtk_entry_set_max_length(GTK_ENTRY(filename), 40);
	gtk_editable_set_editable(GTK_EDITABLE(filename), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed_5), filename, 40, 40);
	// 新建按钮
	GtkWidget *button_download = gtk_button_new_with_label("Download");
	gtk_fixed_put(GTK_FIXED(fixed_5), button_download, 100, 100);

	//绑定回调函数
	g_signal_connect(button_download, "pressed", G_CALLBACK(go_download), filename);

	// 显示窗口全部控件
	gtk_widget_show_all(window_5);

	gtk_main();
}
//下载文件
void go_download(GtkWidget *button_download, gpointer filename){
	msg sendmsg;
	const gchar *filename_1;
	int msglen = sizeof(msg);
	memset(&sendmsg, '\0', msglen);
	filename_1 = gtk_entry_get_text(GTK_ENTRY(filename));
	strncpy(sendmsg.fileName, filename_1, strlen(filename_1));
	strcpy(sendmsg.me, myname);
	sendmsg.command = 5;
	write(comm_sock, &sendmsg, msglen);
}
//下载文件（工具）
void download(msg sendmsg){
	int fd;
	long fileSize;
	char localFilePath[200];

	memset(localFilePath, 0, sizeof(localFilePath));
	getcwd(localFilePath, sizeof(localFilePath));
	strcat(localFilePath, "/");
	strcat(localFilePath, sendmsg.fileName);

	fd = open(localFilePath, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
	if (fd != -1){
		memset(buf, 0, dataLen);
		fileSize = sendmsg.fileSize;
		//read(sockmsg,&fileSize,sizeof(long));
		while (fileSize > dataLen){
			strncpy(buf, sendmsg.buf, dataLen);
			//read(sockmsg,buf,dataLen);
			write(fd, buf, dataLen);
			fileSize = fileSize - dataLen;
		}
		strncpy(buf, sendmsg.buf, fileSize);
		//read(sockmsg,buf,fileSize);
		write(fd, buf, fileSize);
		close(fd);
		printf("下载文件完成！文件名：%s\n", sendmsg.fileName);
	}else{
		printf("下载文件失败！(目录：%s)\n", localFilePath);
	}
}
//返回聊天记录（界面）
void homepage_6(int argc, char *argv[]){
	char *buf;
	buf = (char *)malloc(1024);
	memset(buf, 0, 1024);
	pid_t pid;
	// 初始化
	gtk_init(&argc, &argv);
	// 创建顶层窗口
	window_6 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window_6), "欢迎查询");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window_6), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window_6, 330, 550);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window_6), FALSE);
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window_6, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
	GtkWidget *fixed_6 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_6), fixed_6);

	GtkWidget *label_two;

	label_two = gtk_label_new("请输入查询对象：");
	gtk_fixed_put(GTK_FIXED(fixed_6), label_two, 93, 10);

	// 行编辑的创建
	GtkWidget *group_entry = gtk_entry_new();
	// 最大长度
	gtk_entry_set_max_length(GTK_ENTRY(group_entry), 40);
	gtk_editable_set_editable(GTK_EDITABLE(group_entry), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed_6), group_entry, 40, 40);
	// 创建按钮
	GtkWidget *button_search = gtk_button_new_with_label("Search");
	gtk_fixed_put(GTK_FIXED(fixed_6), button_search, 100, 100);
	// 绑定回调函数
	g_signal_connect(button_search, "pressed", G_CALLBACK(printMsg), group_entry);
	// 文本框聊天窗口
	GtkWidget *view = gtk_text_view_new();
	gtk_widget_set_size_request(view, 300, 300);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed_6), view, 20, 180);
	// 获取文本缓冲区
	bufferuser_2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	// 显示窗口全部控件
	gtk_widget_show_all(window_6);
	gtk_main();
}
//返回聊天记录
void printMsg(GtkWidget *button_search, gpointer group_entry){
	int msglen;
	msg sendmsg;
	const gchar *sendto;
	msglen = sizeof(msg);
	memset(&sendmsg, '\0', msglen);
	sendmsg.command = 6;
	strncpy(sendmsg.me, myname, strlen(myname));
	sendto = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)group_entry));
	strncpy(sendmsg.sendto, sendto, strlen(sendto));
	write(comm_sock, &sendmsg, msglen);
}

int main(int argc, char *argv[]){
	login_main(argc, argv);
	void *r;
	pthread_t tid;
	pthread_create(&tid, 0, read_thread, (void *)(&comm_sock));
	select_1(argc, argv);
	printf("111\n");
	return 0;
	pthread_join(tid, &r);
}
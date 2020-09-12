#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>
#include <arpa/inet.h> 
#include <errno.h>
#include <fcntl.h>

#define BUFFEN 10000
#define port 80
#define IP "172.30.0.94"

using namespace std;

char infor_1[] = "GET / HTTP/1.1\r\n\
Accept: text/html, application / xhtml + xml, image / jxr, */*\r\n\
Accept-Language: zh-Hans-CN,zh-Hans;q=0.5\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko\r\n\
Accept-Encoding: gzip, deflate\r\n\
Host: 192.168.80.230\r\n\
Connection: Keep-Alive\r\n\
Cookie: yunsuo_session_verify=4c374a5373f32bfe15173afb02d7e8ff\r\n\
\r\n";//ҳ��״̬����

char infor_2[]="GET /F.htm HTTP / 1.1\r\n\
Accept: text / html, application / xhtml + xml, image / jxr, */*\r\n\
Referer: http://192.168.80.230/\r\n\
Accept-Language: zh-Hans-CN,zh-Hans;q=0.5\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko\r\n\
Accept-Encoding: gzip, deflate\r\n\
Host: 192.168.80.230\r\n\
Connection: Keep-Alive\r\n\
Cookie: yunsuo_session_verify=4c374a5373f32bfe15173afb02d7e8ff; \r\n\
PHPSESSID=liunjl450tt27ams2r2cl3otm0\r\n\
\r\n";//ҳ���˳�����

void write_time(char t[])
{
	bzero(t, 20);
	struct tm *ptm;
	long ts;
	int y, m, d, h, n, s;
	ts = time(NULL);
	ptm = localtime(&ts);
	y = ptm->tm_year + 1900;
	m = ptm->tm_mon + 1;
	d = ptm->tm_mday;
	h = ptm->tm_hour;
	n = ptm->tm_min;
	s = ptm->tm_sec;
	sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d", y, m, d, h, n, s);
	t[19] = 0;
}
void read_data(ifstream &in, char username[], char password[], char retrytime[], char checktime[], char data[])//��ȡ���ݲ�����
{
	int i;
	char *p;
	in.open("home//G2208-G00004//autologin.cfg", ios::in);
	if (!in.is_open())
	{
		printf("���ļ�ʧ�ܣ�\n");
		return;
	}
	/*username*/
	in.getline(data, 100, '\n');
	p = strstr(data, "username = ");
	if (p)
	{
		while (*p != '=')
			p++;
		p++;
		p++;
		i = 0;
		while (*p != '#' && *p != ' ')
	    {
			username[i] = *p;
			p++;
			i++;
		}
		username[i] = 0;
	}

	/*password*/
	in.getline(data, 100, '\n');
	p = strstr(data, "password = ");
	if (p)
	{
		while (*p != '=')
			p++;
		p++;
		p++;
		i = 0;
		while (*p != '#' && *p != ' ')
		{
			password[i] = *p;
			p++;
			i++;
		}
		password[i] = 0;
	}

	/*retrytime*/
	in.getline(data, 100, '\n');
	p = strstr(data, "retrytime = ");
	if (p)
	{
		while (*p != '=')
p++;
p++;
p++;
i = 0;
while (*p != '#' && *p != ' ')
{
	retrytime[i] = *p;
	p++;
	i++;
}
retrytime[i] = 0;
	}

	/*checktime*/
	in.getline(data, 100, '\n');
	p = strstr(data, "checktime = ");
	if (p)
	{
		while (*p != '=')
			p++;
		p++;
		p++;
		i = 0;
		while (*p != '#' && *p != ' ')
		{
			checktime[i] = *p;
			p++;
			i++;
		}
		checktime[i] = 0;
	}
	in.close();
}
void write_data(char infor[], char username[], char password[])//���û��������������������
{
	sprintf(infor, "POST / HTTP/1.1\r\n\
Accept: text / html, application / xhtml + xml, image / jxr, */*\r\n\
Referer: http://192.168.80.230/\r\n\
Accept-Language: zh-Hans-CN,zh-Hans;q=0.5\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Accept-Encoding: gzip, deflate\r\n\
Host: 192.168.80.230\r\n\
Content-Length: 36\r\n\
Connection: Keep-Alive\r\n\
Cache-Control: no-cache\r\n\
Cookie: yunsuo_session_verify=4c374a5373f32bfe15173afb02d7e8ff; \r\n\
PHPSESSID=v1bloij7lsq0minb2ns8m3rim0\r\n\
\r\n\
DDDDD=%s&upass=%s&0MKKey=\
", username, password);
}
int check_Login(int sockfd, char buf[], char t[])//����¼״̬
{
	fd_set rfd, wfd;
	int maxfd, sel, len;
	bool flag = 0;
	while (1)
	{
		FD_ZERO(&rfd);
		FD_ZERO(&wfd);
		FD_SET(sockfd, &rfd);
		if (flag == 0)
		{
			FD_SET(sockfd, &wfd);
			flag = 1;
		}
		maxfd = sockfd;
		sel = select(maxfd + 1, &rfd, &wfd, NULL, NULL);
		if (sel == -1)
			return -1;
		else if (sel == 0)
			continue;
		else
		{
			if (FD_ISSET(sockfd, &rfd))
			{
				len = recv(sockfd, buf, BUFFEN, 0);
				if (len <= 0)
				{
					printf("receive error!\n");
					close(sockfd);
					exit(-1);
				}
				buf[len] = 0;
				char *login = strstr(buf, "Login</font>");
				char *logout = strstr(buf, "Logout</font>");
				if (login)
				{
					write_time(t);
					printf("%s ����״̬Ϊδ��¼\n", t);
					return 0;
				}
				else if (logout)
				{
					write_time(t);
					printf("%s ����״̬Ϊ�ѵ�¼\n", t);
					return 1;
				}
			}
			if (FD_ISSET(sockfd, &wfd))
				len = send(sockfd, infor_1, strlen(infor_1), 0);
		}
	}
}
int Login_success(char buf[], char t[])//��¼�Ƿ�ɹ�
{
	char *p = strstr(buf, "You have successfully logged into our system");
	if (p)
	{
		write_time(t);
		printf("%s ��¼�ɹ���\n", t);
		return 2;
	}
	p = strstr(buf, "ldap auth error");
	if (p)
	{
		write_time(t);
		printf("%s ��¼ʧ�ܣ�\n", t);
		return 0;
	}
}
void start_Login(int sockfd, char buf[], char infor[], char t[])//�Զ���¼
{
	fd_set rfd, wfd;
	int maxfd, sel, len;
	bool flag = 0;
	while (1)
	{
		FD_ZERO(&rfd);
		FD_ZERO(&wfd);
		FD_SET(sockfd, &rfd);
		if (flag == 0)
		{
			FD_SET(sockfd, &wfd);
			flag = 1;
		}
		maxfd = sockfd;
		sel = select(maxfd + 1, &rfd, &wfd, NULL, NULL);
		if (sel == -1)
			return;
		else if (sel == 0)
			continue;
		else
		{
			if (FD_ISSET(sockfd, &rfd))
			{
				len = recv(sockfd, buf, BUFFEN, 0);
				if (len <= 0)
				{
					printf("receive error!\n");
					close(sockfd);
					exit(-1);
				}
				buf[len] = 0;
				if (Login_success(buf, t))
					return;
			}
			if (FD_ISSET(sockfd, &wfd))
				len = send(sockfd, infor, strlen(infor), 0);
		}
	}
}

int main(int argc, char **argv)
{
	daemon(0, 1);
	signal(SIGCHLD, SIG_IGN);
	struct sockaddr_in serv_addr;
	int sockfd;//�˿�
	char t[20];//ϵͳʱ��
	char buf[BUFFEN];
	char infor[BUFFEN];//������������
	int len;
	
	/********************************��ȡ�����ļ�����*********************************/
	char username[10];//�û���
	char password[20];//����
	char retrytime[5];//ÿ15s���
	char checktime[5];//ÿ900s����Ƿ��ѵ�¼
	char data[100];
	ifstream in;//*.cfg�����ļ�
	read_data(in, username, password, retrytime, checktime, data);//��ȡ���ݲ�����
	write_data(infor, username, password);//���û��������������������
	int retry = atoi(retrytime);//15
	int check = atoi(checktime);//900
	/*********************************************************************************/
	while (1)
	{
		/*����socket*/
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("socket error!\n");
			return -1;
		}
		/*���ӷ����*/
		bzero((char *)&serv_addr, sizeof(struct sockaddr_in));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(IP);
		serv_addr.sin_port = htons(port);
		write_time(t);
		printf("%s ����������֤������...\n", t);
		sleep(1);

		while (1)
		{
			if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
			{
				write_time(t);
				printf("%s δ��������֤���������ȴ�%d����������...\n", t, retry);
				sleep(retry);
			}
			else
			{
				write_time(t);
				printf("%s ������֤�������ɹ���\n", t);
				break;
			}
		}
		if (argc == 2 && strcmp(argv[1], "exit") == 0)
		{
			len = send(sockfd, infor_2, strlen(infor_2), 0);
			write_time(t);
			printf("%s ��������Ͽ�����\n", t);
			close(sockfd);
                        kill(getpid(),SIGKILL);
			break;
		}
		if (check_Login(sockfd, buf, t) == 0)
		{
			write_time(t);
			printf("%s ��������...\n", t);
			start_Login(sockfd, buf, infor, t);
		}
		else
		{
			write_time(t);
			printf("%s ���紦������״̬\n", t);
		}
		close(sockfd);
		write_time(t);
		printf("%s �ȴ�%d�������������״̬\n", t, check);
		sleep(check);
	}
	return 0;
}
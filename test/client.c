#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <error.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_SERVER_ADRESS "127.0.0.1"
#define DEFAULT_SERVER_PORT 8000


char buff[1024] = {0};

static const struct option long_options[]=
{
    {"ip",required_argument,NULL,'i'},
    {"port",required_argument,NULL,'p'},
    {"sender",no_argument,NULL,'s'},
    {"recv",no_argument,NULL,'r'},
    {NULL,0,NULL,0}
};

void set_buff(char c)
{
    int i;
    for(i=0;i < sizeof(buff);i++)
    {
        buff[i]=c;
    }
    buff[sizeof(buff)-2] = '\r';
    buff[sizeof(buff)-1] = '\n';
}

/*
  writen  发送固定字节数
  @fd: 文件描述符
  @buf:发送缓冲区
  @count:要读取的字节数
  成功返回count,失败返回-1
*/

ssize_t writen(int fd,const void *buf,size_t count)
{
    size_t nleft = count;
    ssize_t nwritten;
    char *bufp = (char *) buf;

    while(nleft > 0 )
    {
        if((nwritten = write(fd, bufp, nleft)) < 0)
        {
            if(errno == EINTR)
                continue;
            return -1;
        }else if(nwritten == 0)
            continue;
        bufp += nwritten;
        nleft -= nwritten;
    }

    return count;

}
void senf_func(int fd,char c)
{
    set_buff(c);
    sleep(1);
    writen(fd,buff,sizeof(buff));
}
int  recv_func(int fd)
{
    int length = 0;
    length = recv(fd,buff,sizeof(buff),0);
    if(length <0)
    {
        printf("recv data error\n");
        return -1;
    }
/*
    int write_length = fwrite(buff,sizeof(char),length,fp);
    if(write_length < length)
    {
        printf("file write error\n");
        return -1;
    }
*/
    printf("%s",buff);
    bzero(buff,sizeof(buff));
    return 1;
}
void usage()
{
    fprintf(stderr,"clent [option] ...\n"
            " -i <ip>            The server ip address.\n"
            " -p <port>          The port server listen\n"
            " -s                 Make client as sender\n"
            " -r                 Make client as recv\n"
        );
}

int main(int argc, char **argv)
{
    int send = 0; //default recv
    int recv = 0;
    int port = 0;
    char *server_ip=NULL;

    int opt=0;
    int options_index =0;
/*
  if(argc ==1)
  {
  usage();
  return 2;
  }
*/
    while((opt = getopt_long(argc,argv,"sri:p:",long_options,&options_index))!=EOF)
    {
        switch(opt)
        {
        case 0:break;
        case 's':send = 1; break;
        case 'r':recv = 1;break;
        case 'p':port = atoi(optarg);break;
        case 'i':server_ip = optarg;break;
        }
    }

    if(port == 0)
    {
        port = DEFAULT_SERVER_PORT;
    }
    if(server_ip == NULL)
    {
        server_ip = DEFAULT_SERVER_ADRESS;
    }

    if(send && recv)
    {
        // all set make it recv
        send = 0;
    }
    if(!send && !recv)
    {
        // no set make it as recv
        recv = 1;
    }
/*
    printf("s:%d\n",send);
    printf("r:%d\n",recv);
    printf("p:%d\n",port);
    printf("ip:%s\n",server_ip);
*/
    struct sockaddr_in client_addr;
    bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);

    int client_socket = socket(AF_INET,SOCK_STREAM,0);
    if( client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_aton(server_ip,&server_addr.sin_addr) == 0) //服务器的IP地址来自程序的参数
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    server_addr.sin_port = htons(port);
    socklen_t server_addr_length = sizeof(server_addr);
    //向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n",server_ip);
        exit(1);
    }
//connect ok!
    char c = 'A';
    while(1)
    {
        if(send)
        {
            senf_func(client_socket,c);
            c++;
            if(c>'Z')
                c='A';
        }
        if(recv)
        {
            if((recv_func(client_socket)) < 0)
                break;
        }
    }
    //关闭socket
    close(client_socket);
    return 0;
}

#include "common.h"
#include "send.h"
#include "data.h"
#include "recv.h"

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    //static int b_sender = 0;

    struct event_base *base =(struct event_base*) arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0)
    {
        perror("accept");
    }
    else if (fd > FD_SETSIZE)
    {
        close(fd);
    }
    else
    {
        struct bufferevent *bev;
        evutil_make_socket_nonblocking(fd);
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

        Rec_set.insert(bev);
        send_func_set(bev);
        printf("There has %zu connectors\n",Rec_set.size());
        //使用bufferevent_socket_new创建一个struct bufferevent *bev，关联该sockfd，托管给event_base
        ////BEV_OPT_CLOSE_ON_FREE表示释放bufferevent时关闭底层传输端口。这将关闭底层套接字，释放底层bufferevent等。
        //  bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
/*        if(Sender == NULL)
          {
          printf("sender is null. Add a new sender!\n");
          Sender = bev;
          send_func_set(bev);
          //b_sender = 0;
          }else
          {
          Rec_set.insert(bev);
          recv_func_set(bev);
          }
*/
    }
}



int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event_base *base;
    struct event *listener_event;

    base = event_base_new();
    if (!base)
        return -1; /*XXXerr*/

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(8000);

    listener = socket(AF_INET, SOCK_STREAM, 0);
//允许多次绑定同一个地址。要用在socket和bind之间
    //evutil_make_listen_socket_reuseable(listener);
    // evutil_make_socket_nonblocking(listener);

#ifndef WIN32
    {
        int one = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    }
#endif

    if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        perror("bind");
        return -1;
    }
    if (listen(listener, 16)<0)
    {
        perror("listen");
        return -1;
    }
    evutil_make_socket_nonblocking(listener);
    listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);
    /*XXX check it */
    event_add(listener_event, NULL);

    event_base_dispatch(base);

    return 0;
}

#include "send.h"
#include "common.h"
#include "data.h"

char buff[4096] = {0};

/*
  static void do_read(evutil_socket_t fd, short events, void *arg)
  {
  }
*/
static void send_data_all_other_conn(struct bufferevent *bev,void* data,size_t datlen)
{
        set<struct bufferevent *>::iterator iter;
        for(iter = Rec_set.begin() ; iter != Rec_set.end() ; ++iter)
        {
                //struct evbuffer * recv_output = bufferevent_get_output(*iter);
                if(*iter == bev)
                {
                        continue;
                }
                bufferevent_write(*iter,data,datlen);
        } // end for
}

static void readcb(struct bufferevent *bev, void *ctx)
{
        struct evbuffer *input;
        size_t len;

        if(!Rec_set.empty())
        {
                input = bufferevent_get_input(bev);//其实就是取出bufferevent中的input
                len = evbuffer_get_length(input);
                size_t nleft = len;
                int nread;
                while(nleft >0)
                {
                        if((nread = evbuffer_remove(input, buff, sizeof(buff))) < 0)
                        {
                                return;
                        }
                        nleft -=nread;
                        send_data_all_other_conn(bev, buff, nread);
                }//end while(nleft >0)
        }//end if(!Rec_set.empty())
        }


static void errorcb(struct bufferevent *bev, short error, void *ctx)
{
        if (error & BEV_EVENT_EOF)
        {
                /* connection has been closed, do any clean up here */
                printf("one connection closed\n");
        }
        else if (error & BEV_EVENT_ERROR)
        {
                /* check errno to see what error occurred */
                printf("some other error\n");
        }
        else if (error & BEV_EVENT_TIMEOUT)
        {
                /* must be a timeout event handle, handle it */
                printf("Timed out\n");
        }
        Rec_set.erase(bev);
        bufferevent_free(bev);
        //Sender = NULL;
        printf("There has %zu connectors left\n",Rec_set.size());
}


void send_func_set(struct bufferevent *bev)
{
        bufferevent_setcb(bev,readcb,NULL,errorcb,NULL);
        bufferevent_enable(bev,EV_READ|EV_WRITE);
}

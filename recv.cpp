#include "common.h"
#include "recv.h"
#include "data.h"
static void readcb(struct bufferevent *bev, void *ctx)
{
        printf("called recv readcb!\n");
        struct evbuffer *input, *output;
        // char *request_line;
        //size_t len;
        input = bufferevent_get_input(bev);//其实就是取出bufferevent中的input
        output = bufferevent_get_output(bev);//其实就是取出bufferevent中的output

        size_t input_len = evbuffer_get_length(input);
        printf("input_len: %zu\n", input_len);
        size_t output_len = evbuffer_get_length(output);
        printf("output_len: %zu\n", output_len);

        bufferevent_write_buffer(Sender,input);//发给发送者
        evbuffer_add_buffer(output,input); //echo

}



static void errorcb(struct bufferevent *bev, short error, void *ctx)
{
        if (error & BEV_EVENT_EOF)
        {
                /* connection has been closed, do any clean up here */
                printf("recv connection closed\n");
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
        bufferevent_free(bev);
        bev = NULL;
}
void recv_func_set(struct bufferevent *bev)
{
        bufferevent_setcb(bev,NULL,NULL,errorcb,NULL);
        //bufferevent_setcb(bev,readcb,NULL,errorcb,NULL);
        bufferevent_enable(bev,EV_READ|EV_WRITE);

        cout << "add a new receiver.All have " <<Rec_set.size() <<" receivers." << endl;
}

#include <stdio.h>
#include <unistd.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

static struct blob_buf b;

static void receive_call_result_data(struct ubus_request *req, int type, struct blob_attr *msg)
{
    char *str;
    if(!msg)
        return;

    str = blobmsg_format_json_indent(msg, true, 0);
    printf("---> %s\n", str);

    free(str);
}

static int client_main(struct ubus_context *ctx, int argc, char**argv)
{
    uint32_t id;
    int ret;

    printf("argc =%d, argv[0] =%s\n", argc, argv[0]);
    if(argc != 3)
        return -2;
    //blob_buf一般声明为本地静态变量，id一般使用0（BLOBMSG_TYPE_UNSPEC）来初始化。
    blob_buf_init(&b, 0);
    //这里获取到的是传递的参数  ubus_client stu add '{"no":20}'  这里的就是{"no":20}
    if(!blobmsg_add_json_from_string(&b, argv[2])){
        printf("Failed to parse message data\n");
        return -1;
    }
    //调用ubus_lookup_id函数找到指定对象的ID，然后通过ubus_invoke函数调用来请求服务器，返回的结果使用 receive_call_result_data来处理
    //向ubusd查询是否存在argv[0]这个对象，如果存在，返回其id
    //TODO:这里的逻辑也是和ubus的程序流程一样，先查找对象，再调用对象的方法
    ret = ubus_lookup_id(ctx, argv[0], &id);
    if(ret)
        return ret;
	printf("name:%s --> id:%x\n", argv[0], id);//这里的id是stu这个对象的id，和ubus -v list看到的对应id相同

    //触发server端注册的函数，并接收返回值，处理函数为receive_call_result_data
    //调用argv[1]方法
    return ubus_invoke(ctx, id, argv[1], b.head, receive_call_result_data, NULL, 3*1000);
}

int main(int argc, char **argv)
{
    struct ubus_context *ctx = NULL;
    char ch;

    /* 1. create an epoll instatnce descriptor poll_fd */
    char *ubus_socket = NULL;
    int ret = 0;

    argc -= optind;
    argv += optind;

    ctx = ubus_connect(NULL);
    if(!ctx){
        printf("ubus connect error.\n");
        return -1;
    }

    ret = client_main(ctx, argc, argv);
	printf("ret=%d\n", ret);
    if(ret < 0)
        return ret;

    if(ctx)
        ubus_free(ctx);

    return 0;
}
// ser.c
#include <stdio.h>
#include <unistd.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <ubusmsg.h>


static char *ubus_socket = NULL;
struct ubus_context *ctx = NULL;
static struct blob_buf b;

const char *__ubus_strerror[__UBUS_STATUS_LAST] = {
	[UBUS_STATUS_OK] = "Success",
	[UBUS_STATUS_INVALID_COMMAND] = "Invalid command",
	[UBUS_STATUS_INVALID_ARGUMENT] = "Invalid argument",
	[UBUS_STATUS_METHOD_NOT_FOUND] = "Method not found",
	[UBUS_STATUS_NOT_FOUND] = "Not found",
	[UBUS_STATUS_NO_DATA] = "No response",
	[UBUS_STATUS_PERMISSION_DENIED] = "Permission denied",
	[UBUS_STATUS_TIMEOUT] = "Request timed out",
	[UBUS_STATUS_NOT_SUPPORTED] = "Operation not supported",
	[UBUS_STATUS_UNKNOWN_ERROR] = "Unknown error",
	[UBUS_STATUS_CONNECTION_FAILED] = "Connection failed",
};
int error_send_result_reply(struct ubus_context *ctx, struct ubus_request_data *req, 
	struct blob_buf *buf, char *result, char *failreason)
{
	blob_buf_init(buf, 0);
	blobmsg_add_string(buf, "result", result);
	blobmsg_add_string(buf, "failreason", failreason);
	ubus_send_reply(ctx, req, buf->head);
	return 0;
}

static int stu_add(struct ubus_context *ctx, struct ubus_object *obj,
                   struct ubus_request_data *req, const char *method,
                   struct blob_attr *msg);
static int stu_sub(struct ubus_context *ctx, struct ubus_object *obj,
                   struct ubus_request_data *req, const char *method,
                   struct blob_attr *msg);
enum
{
    STU_NO1,
	STU_NO2,
    __STU_MAX
};

//用于消息解析，指明key为no字符串的时候，它的值是一个INT32类型的值，可以单纯的理解为"no" : 20
static const struct blobmsg_policy stu_policy[__STU_MAX] = {
    [STU_NO1] = {.name = "no1", .type = BLOBMSG_TYPE_INT32},
	[STU_NO2] = {.name = "no2", .type = BLOBMSG_TYPE_INT32},
};

static const struct ubus_method stu_methods[] = {
    /* 不推荐使用该方式
    { .name = "add", .handler = stu_add, .policy = stu_policy, .n_policy = 1 },
    { .name = "sub", .handler = stu_sub, .policy = stu_policy, .n_policy = 1 }
*/
    UBUS_METHOD("add", stu_add, stu_policy),    //stu_add 和stu_sub是回调函数，/** 方法处理回调函数 */
    UBUS_METHOD("sub", stu_sub, stu_policy),
};


//绑定对象类型和方法数组(固定写法)
static struct ubus_object_type stu_object_type =
    UBUS_OBJECT_TYPE("stu", stu_methods);

//ubus将消息处理抽象为对象（object）和方法（method）的概念。一个对象中包含多个方法。对象和方法都有自己的名字，发送请求方在消息中指定要调用的对象和方法名字即可
//TODO:这里可以看出调用的是对象加方法名的方式。
//定义对象结构体的名称和绑定对象的方法数组-会在数组中注册具体的方法名称和具体调用的方法
static struct ubus_object stu_object = {
    .name = "stu",
    .type = &stu_object_type,
    .methods = stu_methods, //事件数组
    .n_methods = ARRAY_SIZE(stu_methods)};  //事件数组个数




//sta方法函数
static int stu_add(struct ubus_context *ctx, struct ubus_object *obj,
                   struct ubus_request_data *req, const char *method,
                   struct blob_attr *msg)
{
    struct blob_attr *tb[__STU_MAX];

    //根据policy从解析出blobmsg  blobmsg在blob_attr的data区
    /*
        blobmsg用于二进制对象网络序列化。嵌套在blob数据结构（blob_attr）的data区。因此形成：blob_buff <- blob_attr -< blobmsg，blob_buff可存储管理多个blob_attr，每个blob_attr又可存储管理一个blogmsg。且可存储在线性数据区，不需要链表指针。
        blobmsg_policy用于解析和缓存blobmsg列表，一般声明为一个静态数组，用于指导消息解析。
        blobmsg默认使用id为table。array类似C语言的数组，table类似C的结构。
    */
    blobmsg_parse(stu_policy, ARRAY_SIZE(stu_policy), tb, blob_data(msg), blob_len(msg));

    /*
        这里是一系列函数，因为上面的.type = BLOBMSG_TYPE_INT32 所以这里用blobmsg_get_u32来解析，一系列函数如下
        static inline uint8_t blobmsg_get_u8(struct blob_attr *attr)
        static inline bool blobmsg_get_bool(struct blob_attr *attr)
        static inline uint16_t blobmsg_get_u16(struct blob_attr *attr)
        static inline uint32_t blobmsg_get_u32(struct blob_attr *attr)
        static inline uint64_t blobmsg_get_u64(struct blob_attr *attr)
        static inline char *blobmsg_get_string(struct blob_attr *attr)
     */
	int stu_no1 = 0;
	int stu_no2 = 0;
    if (tb[STU_NO1])
	{
        stu_no1 = blobmsg_get_u32(tb[STU_NO1]);
	}
	else
	{
		printf("tb[STU_NO1] is NULL");

        return -1;

	}
	
	if (tb[STU_NO2])
        stu_no2 = blobmsg_get_u32(tb[STU_NO2]);
	else
	{
		printf("tb[STU_NO2] is NULL");

        return -1;

	}

    //因为blob_buff
    //blob_buf一般声明为本地静态变量，id一般使用0（BLOBMSG_TYPE_UNSPEC）来初始化。
    blob_buf_init(&b, 0);
    //将stu_no的值添加到buf中，然后调用发送函数
    blobmsg_add_u32(&b, "result", stu_no1+stu_no2);
    //ubus发送函数，执行完成方法调用后发送响应
    ubus_send_reply(ctx, req, b.head);

    return 0;
}

static int stu_sub(struct ubus_context *ctx, struct ubus_object *obj,
                   struct ubus_request_data *req, const char *method,
                   struct blob_attr *msg)
{
    struct blob_attr *tb[__STU_MAX];

    //解析
    int ret = blobmsg_parse(stu_policy, ARRAY_SIZE(stu_policy), tb, blob_data(msg), blob_len(msg));
	
	if (ret != UBUS_STATUS_OK)
	{
		printf("error:%s\n", __ubus_strerror[ret]);
		error_send_result_reply(ctx, req, &b, __ubus_strerror[ret], "failed to blobmsg_parse");
		return ret;
	}

    int stu_no1 = 0;
	int stu_no2 = 0;
    if (tb[STU_NO1])
	{
        stu_no1 = blobmsg_get_u32(tb[STU_NO1]);
	}
	else
	{
		printf("tb[STU_NO1] is NULL");

        return -1;

	}
	
	if (tb[STU_NO2])
        stu_no2 = blobmsg_get_u32(tb[STU_NO2]);
	else
	{
		printf("tb[STU_NO2] is NULL");

        return -1;

	}

    blob_buf_init(&b, 0);
    blobmsg_add_u32(&b, "result", stu_no1 - stu_no2);
    ubus_send_reply(ctx, req, b.head);

    return 0;
}


static void ubus_add_fd(void)
{
    ubus_add_uloop(ctx);

#ifdef FD_CLOEXEC
    fcntl(ctx->sock.fd, F_SETFD,
          fcntl(ctx->sock.fd, F_GETFD) | FD_CLOEXEC);
#endif
}

static void ubus_reconn_timer(struct uloop_timeout *timeout)
{
    static struct uloop_timeout retry =
        {
            .cb = ubus_reconn_timer,
        };
    int t = 2;

    if (ubus_reconnect(ctx, ubus_socket) != 0)
    {
        uloop_timeout_set(&retry, t * 1000);
        return;
    }

    ubus_add_fd();
}

static void ubus_connection_lost(struct ubus_context *ctx)
{
    ubus_reconn_timer(NULL);    //当ubus断开的时候调用
}
int main(int argc, char **argv)
{
    char ch;
    int ret = 0;


    //创建一个epoll的句柄，最多监控32个文件描述符。
    uloop_init();

    //使用ubus_connect连接到服务管理进程ubusd，得到ubus_context(包含了连接fd、注册fd的回调等)
    /* use default UNIX sock path: /var/run/ubus.sock */
    //TODO: 这里使用ubus的原因就是ubusd绑定了/var/run/ubus.sock文件，服务器要和客户端通信，需要通过ubus转发，所以需要绑定同样的文件
    ctx = ubus_connect(ubus_socket);
    if (!ctx)
    {
        printf("ubus connect error.\n");
        return -1;
    }

    //注册断开回调函数
    ctx->connection_lost = ubus_connection_lost;

    //client端向ubusd server请求增加一个新object(虽然这个是服务器程序，但是对于ubusd来说它还是个客户端程序)
    ret = ubus_add_object(ctx, &stu_object);
    if (ret)
    {
        printf("Failed to add object to ubus:%s\n", ubus_strerror(ret));
        return 0;
    }

    //ubus_add_uloop(ctx);
    //添加fd到ubus中
    ubus_add_fd();

    /**
        * 事件循环主处理入口
        *1.当某一个进程第一次调用uloop_run时，注册sigchld和sigint信号
        *2.循环获取当前时间，把超时的timeout处理掉，有一条timeout链表在维护
        *3.循环检测是否收到一个sigchld信号，如果收到，删除对应的子进程，有一条process子进程链表在维护
        *4.循环调用epoll_wait 监相应的触发事件文件描述符fd
    **/

   //TODO: 调用uloop后，是否uloop是一个死循环程序，后面的程序是否会运行,如果这是个死循环函数，那么退出的机制是什么
    uloop_run();

    printf("================ line is %d\n",__LINE__);

    if (ctx)
        ubus_free(ctx);

    /**
        * 销毁事件循环
        * 关闭epoll描述符
        * 销毁子进程链表
        * 销毁timeout链表
    **/
    uloop_done();

    return 0;
}

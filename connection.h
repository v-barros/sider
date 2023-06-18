/*
 * connection.h
 *
 *  Created on: 2023-05-23
 *      Author: @v-barros
 */
#ifndef CONNECTION_H_
#define CONNECTION_H_
struct _eventloop;

typedef struct connection connection;
typedef void (*ConnectionCallbackFunc)(struct connection *conn);
typedef enum {
    CONN_STATE_NONE = 0,
    CONN_STATE_CONNECTING,
    CONN_STATE_ACCEPTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_CLOSED,
    CONN_STATE_ERROR
} ConnectionState;

typedef struct ConnectionType {
    void (*events_handler)(struct _eventloop *el, int fd, void *clientData, int mask);
    int (*write)(struct connection *conn, const void *data, size_t data_len);
    int (*read)(struct connection *conn, void *buf, size_t buf_len);
    void (*close)(struct connection *conn);
    int (*accept)(struct connection *conn, ConnectionCallbackFunc accept_handler);
    int (*set_write_handler)(struct connection *conn, ConnectionCallbackFunc handler, int barrier);
    int (*set_read_handler)(struct connection *conn, ConnectionCallbackFunc handler);
    const char *(*get_last_error)(struct connection *conn);
    int (*blocking_connect)(struct connection *conn, const char *addr, int port, long long timeout);
    ssize_t (*sync_write)(struct connection *conn, char *ptr, ssize_t size, long long timeout);
    ssize_t (*sync_read)(struct connection *conn, char *ptr, ssize_t size, long long timeout);
    ssize_t (*sync_readline)(struct connection *conn, char *ptr, ssize_t size, long long timeout);
    int (*get_type)(struct connection *conn);
} ConnectionType;

struct connection
{
    ConnectionType *type;
    ConnectionState state;
    short int flags;
    short int refs;
    int last_errno;
    void *private_data;
    ConnectionCallbackFunc conn_handler;
    ConnectionCallbackFunc write_handler;
    ConnectionCallbackFunc read_handler;
    int fd;
};

// set up new NON BLOCKING listening socket using port and return socketfd
int init_conn(int port);

int connGetSocketError(connection *conn);
static void connSocketEventHandler(struct _eventloop *el, int fd, void *clientData, int mask);
int connGetSocketError(connection *conn);

connection *connCreateSocket();
connection *connCreateAcceptedSocket(int fd);

int connGetState(connection *conn);
static void connSocketClose(connection *conn);
static int connSocketRead(connection *conn, void *buf, size_t buf_len);
static int connSocketWrite(connection *conn, const void *data, size_t data_len);
static int connSocketConnect(connection *conn, const char *addr, int port, const char *src_addr,ConnectionCallbackFunc connect_handler);

static inline int connAccept(connection *conn, ConnectionCallbackFunc accept_handler) {
    return conn->type->accept(conn, accept_handler);
}

static inline void connClose(connection *conn) {
    conn->type->close(conn);
}
/* Get the associated private data pointer */
void *connGetPrivateData(connection *conn);

#endif // CONNECTION_H_
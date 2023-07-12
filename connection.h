/*
 * connection.h
 *
 *  Created on: 2023-05-23
 *      Author: @v-barros
 */
#ifndef CONNECTION_H_
#define CONNECTION_H_

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
    int (*set_write_handler)(struct connection *conn, ConnectionCallbackFunc handler);
    int (*set_read_handler)(struct connection *conn, ConnectionCallbackFunc handler);
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
    ConnectionCallbackFunc write_handler;   //sendReplyToClient
    ConnectionCallbackFunc read_handler;    //readQueryFromClient
    int fd;
};

// set up new NON BLOCKING listening socket using port and return socketfd
int init_conn(int port);

int connGetSocketError(connection *conn);
static void connSocketEventHandler(struct _eventloop *el, int fd, void *clientData, int mask);
int connGetSocketError(connection *conn);

connection *connCreateSocket();
connection *connCreateAcceptedSocket(int fd);
/* Get the associated private data pointer */
void *connGetPrivateData(connection *conn);
void connSetPrivateData(connection *conn,void* privateData);

int connGetState(connection *conn);
static void connSocketClose(connection *conn);
static int connSocketRead(connection *conn, void *buf, size_t buf_len);
static int connSocketWrite(connection *conn, const void *data, size_t data_len);
static int connSocketConnect(connection *conn, const char *addr, int port, const char *src_addr,ConnectionCallbackFunc connect_handler);
static int connSocketSetReadHandler(connection *conn, ConnectionCallbackFunc func);
static int connSocketSetWriteHandler(connection *conn, ConnectionCallbackFunc func);

static inline int connAccept(connection *conn, ConnectionCallbackFunc accept_handler) {
    return conn->type->accept(conn, accept_handler);
}

static inline void connClose(connection *conn) {
    conn->type->close(conn);
}

static inline int connRead(connection *conn, void *buf, size_t buf_len) {
    int ret = conn->type->read(conn, buf, buf_len);
    return ret;
}

/* Write to connection, behaves the same as write(2).
 *
 * Like write(2), a short write is possible. A -1 return indicates an error.
 *
 * The caller should NOT rely on errno. Testing for an EAGAIN-like condition, use
 * connGetState() to see if the connection state is still CONN_STATE_CONNECTED.
 */
static inline int connWrite(connection *conn, const void *data, size_t data_len) {
    return conn->type->write(conn, data, data_len);
}

/* Register a read handler using conn->type->set_read_handler, to be called when the connection is readable.
 * If NULL, the existing handler is removed.
 */
static inline int connSetReadHandler(connection *conn, ConnectionCallbackFunc func) {
    return conn->type->set_read_handler(conn, func);
}

/* Register a write handler using conn->type->set_write_handler, to be called when the connection is writable.
 * If NULL, the existing handler is removed.
 */
static inline int connSetWriteHandler(connection *conn, ConnectionCallbackFunc func) {
    return conn->type->set_write_handler(conn, func);
}



#endif // CONNECTION_H_
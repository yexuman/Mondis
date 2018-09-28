//
// Created by 11956 on 2018/9/5.
//

#ifndef MONDIS_MONDISCLIENT_H
#define MONDIS_MONDISCLIENT_H


#include <stdint-gcc.h>
#include <queue>

#ifdef WIN32

#include <winsock2.h>
#include <inaddr.h>

#elif defined(linux)
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "HashMap.h"
#include "Command.h"

class MondisClient {
public:
    uint64_t id;            /* Client incremental unique ID. */
#ifdef WIN32
    SOCKET sock;
#elif defined(linux)
    int fd;/* Client socket. */
#endif
    int curDbIndex = 0;
    HashMap *keySpace = nullptr;            /* Pointer to currently SELECTed DB. */
    string name;             /* As set by CLIENT SETNAME. */
    queue<string> queryBuffer;         /* Buffer we use to accumulate client queries. */
    queue<string> pendingQuerybuf;   /* If this client is flagged as master, this buffer
                               represents the yet not applied portion of the
                               replication stream that we are receiving from
                               the master. */
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size. */
    Command *curCommand;          /* Arguments of current command. */
    Command *lastcmd;  /* Last command executed. */
    int reqtype;            /* Request protocol type: PROTO_REQ_* */
    int multibulklen;       /* Number of multi bulk arguments left to read. */
    long bulklen;           /* Length of bulk argument in multi bulk request. */
    vector<ExecutionResult> *reply;            /* List of reply objects to send to the client. */
    unsigned long long reply_bytes; /* Tot bytes of objects in reply list. */
    size_t sentlen;         /* Amount of bytes already sent in the current
                               buffer or object being sent. */
    time_t ctime;           /* Client creation time. */
    time_t lastinteraction; /* Time of the last interaction, used for timeout */
    time_t obuf_soft_limit_reached_time;
    int flags;              /* Client flags: CLIENT_* macros. */
    int authenticated;      /* When requirepass is non-NULL. */
    int replstate;          /* Replication state if this is a slave. */
    int repl_put_online_on_ack; /* Install slave write handler on ACK. */
    int repldbfd;           /* Replication DB file descriptor. */
    off_t repldboff;        /* Replication DB file offset. */
    off_t repldbsize;       /* Replication DB file size. */
    string replpreamble;       /* Replication DB preamble. */
    long long read_reploff; /* Read replication offset if this is a master. */
    long long reploff;      /* Applied replication offset if this is a master. */
    long long repl_ack_off; /* Replication ack offset, if this is a slave. */
    long long repl_ack_time;/* Replication ack time, if this is a slave. */
    long long psync_initial_offset; /* FULLRESYNC reply offset other slaves
                                       copying this slave output buffer
                                       should use. */

    string ip;
    string port;
public:
    bool hasLogin = false;
private:
    static int nextId;
public:
#ifdef WIN32

    MondisClient(SOCKET sock);

#elif defined(linux)
    MondisClient(int fd);

#endif
    string readCommand();

    void sendResult(const string &res);
};


#endif //MONDIS_MONDISCLIENT_H

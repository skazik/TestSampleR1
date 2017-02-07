// this module is mapping IPC API to Linux for test purposes
// the platform specific API to be used or implemented for the real device

#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "common.h"
#include "ipc.h"

static const key_t gMsgQueueKey = 1234;
static int gMsgQueue = 0;

// Declare the message structure.
typedef struct msgbuf {
     long   mType;
     char   mPayload[MSGSZ];
} message_buf;

///
/// \brief ipc_create_queue
/// \return queue Id if succeeded, -1 otherwise
///
int ipc_create_queue(void)
{
    int msgflg = IPC_CREAT | MSGQID;

    if ((gMsgQueue = msgget(gMsgQueueKey, msgflg)) < 0) {
        perror("msgget");
        return -1;
    }
    else
    {
        printf("msgget: msgget succeeded: gMsgQueue = %d\n", gMsgQueue);
    }

    return gMsgQueue;
}

///
/// \brief ipc_remove_queue
///
void ipc_remove_queue()
{
    msgctl(gMsgQueue, IPC_RMID, NULL);
    gMsgQueue = 0;
}

///
/// \brief ipc_msg_send
/// \param gMsgQueue
/// \param aType
/// \param aPayload
/// \param aSize
/// \return n_bytes sent if succeeded, -1 otherwise
///
int ipc_msg_send(long aType, void * aPayload, int aSize)
{
    return ipc_msg_send_to(gMsgQueue, aType, aPayload, aSize);
}

///
/// \brief ipc_msg_send
/// \param aMsgQueue
/// \param aType
/// \param aPayload
/// \param aSize
/// \return n_bytes sent if succeeded, -1 otherwise
///
int ipc_msg_send_to(int aMsgQueue, long aType, void * aPayload, int aSize)
{
    message_buf sbuf;
    size_t buf_length;

    sbuf.mType = aType;

    buf_length = MIN(aSize, MSGSZ);
    memcpy(sbuf.mPayload, aPayload, buf_length);

    if (msgsnd(aMsgQueue, &sbuf, buf_length, IPC_NOWAIT) < 0) {
       printf ("send to %d, [%d, %d] failed\n", aMsgQueue, (int) sbuf.mType, (int) buf_length);
       perror("msgsnd");
       return -1;
    }

    return buf_length;
}

///
/// \brief ipc_get_queue
/// \return
///
int ipc_get_queue()
{
    int msg_queue;
    // Get the message queue id by key
    if ((msg_queue = msgget(gMsgQueueKey, MSGQID)) < 0) {
        perror("msgget");
        return -1;
    }
    return msg_queue;
}

///
/// \brief ipc_msg_get
/// \param aType
/// \param aPayload
/// \param aSize
/// \return n_bytes > 0 on success or error < 0 otherwise
///
int ipc_msg_get(long aType, void* aPayload, int aSize, int aFlags)
{
    int msg_queue, n_bytes;
    message_buf  rbuf;

    if (aPayload == NULL || aSize <= 0)
    {
        return -EINVAL;
    }

    msg_queue = ipc_get_queue();

    if (0 >= (n_bytes = msgrcv(msg_queue, &rbuf, MSGSZ, aType, aFlags))) {
        if (errno == ENOMSG)
        {
            return -ENOMSG;
        }
        perror("msgrcv");
        return -2;
    }

    n_bytes = MIN(aSize, n_bytes);
    memcpy(aPayload, rbuf.mPayload, n_bytes);
    return n_bytes;
}

// this module defines abstract layer of the single common queue IPC
// which should be resolved using simulated or platform specific API
// see ipc.c for details

#ifndef _IPC_H_
#define _IPC_H_

#include <sys/ipc.h>

#define MSGTYPE_INT     1
#define MSGTYPE_EXT     2
#define MSGSZ           16
#define MSGQID          0666

enum {
    IPC_STOP_REQ    = 666,
    IPC_BUTTON_DOWN = 111,
    IPC_BUTTON_UP   = 222,
    IPC_LOW_POWER   = 333,
    IPC_POWER_OK    = 444,
    IPC_CHARGER_ON  = 555,
    IPC_CHARGER_OFF = 777,
} eIPC_Msg_t;

///
/// \brief ipc_create_queue
/// \return queue Id if succeeded, -1 otherwise
///
int ipc_create_queue(void);

///
/// \brief ipc_get_queue
/// \return
///
int ipc_get_queue();

///
/// \brief ipc_remove_queue
///
void ipc_remove_queue();

///
/// \brief ipc_msg_send
/// \param aType
/// \param aPayload
/// \param aSize
/// \return n_bytes sent if succeeded, -1 otherwise
///
int ipc_msg_send(long aType, void * aPayload, int aSize);

///
/// \brief ipc_msg_send
/// \param aMsgQueue
/// \param aType
/// \param aPayload
/// \param aSize
/// \return n_bytes sent if succeeded, -1 otherwise
///
int ipc_msg_send_to(int aMsgQueue, long aType, void * aPayload, int aSize);

///
/// \brief ipc_msg_get
/// \param aType
/// \param aPayload
/// \param aSize
/// \return n_bytes > 0 on success or error < 0 otherwise
///
int ipc_msg_get(long aType, void* aPayload, int aSize, int aFlags);

#endif // _IPC_H_

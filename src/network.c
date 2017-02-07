// this module implements network task API on Linux for test purposes
// the platform specific API could be used if different on real devices

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "common.h"
#include "ipc.h"
#include "network.h"

// static definitions
static unsigned short gu16HeartbeatCounter = 0;
static int giSleepCounter = 0;
static pid_t gNetworkPid = 0;
static BOOL gbHbRunning = FALSE;

///
/// \brief is_stop_requested
/// \return
///
static BOOL is_stop_requested(void)
{
    int ipc_retval, ipc_msg;

    if (-ENOMSG != (ipc_retval = ipc_msg_get(MSGTYPE_INT, (void*) &ipc_msg, sizeof(ipc_msg), IPC_NOWAIT)))
    {
        DEBUG_LOG(("ipc_retval = %d, %d\n", ipc_retval, ipc_msg));
        if (ipc_retval == sizeof(ipc_msg) && ipc_msg == IPC_STOP_REQ)
        {
            return TRUE;
        }
    }
    return FALSE;
}

///
/// \brief network_process_run
/// \return
///
static int network_process_run(void)
{
    int retval = NO_ERROR;
    int sockfd = INVALID_SOCKET;
    int n_bytes;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buf[PACKET_SIZE];
    BOOL bDone;

    // get server IP address
    server = gethostbyname(HOST_NAME);
    if (server == NULL) {
        ERROR_LOG(("ERROR, no such host as %s\n", HOST_NAME));
        retval = ERR_HOST_NOT_FOUND;
        // optional - goto err; if allowed by programming convention
    }
        // the section for debugging only
    // if preprocessor doesn't exclude empty "else" statements then should be surrounded by #if DEBUGGING
    else
    {
        DEBUG_LOG(("%s = %d.%d.%d.%d\n", server->h_name, (uint8_t) server->h_addr[0],
                  (uint8_t) server->h_addr[1],(uint8_t)  server->h_addr[2], (uint8_t) server->h_addr[3]));
    }

    if (retval == NO_ERROR)
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            ERROR_LOG(("opening socket failed\n"));
            retval = INVALID_SOCKET;
        }
    }

    if (retval == NO_ERROR)
    {
        //set timeout for recv_socket
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = ACK_TIMEOUT_MS * 1000;

        if (NO_ERROR != (retval = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval))))
        {
            ERROR_LOG(("setsockopt error %d\n", errno));
            goto err;
        }
    }

    // reset packet # couter and vars
    bDone = FALSE;
    gu16HeartbeatCounter = 0;

    // main loop of sending incremental counter to server
    while (!bDone) {

        // reset sleep counter
        giSleepCounter = PACKET_INTERVAL_MS;

        // loop of sending/resending packet in case of 500 ms timeout,
        // recvfrom error or ACK value mismatch
        do {
            // format host address
            bzero((char *) &serveraddr, sizeof(serveraddr));
            serveraddr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
            serveraddr.sin_port = htons(HOST_PORT);

            // requirement: the device shall send a packet of 2 bytes to the server
            // with a counter, starting at value 0 and increasing by 1 every packet.
            // implementation: assuming counter is an unsigned short value [0..0xFFFF] in big-endian format

            serverlen = sizeof(serveraddr);
            buf[0] = (gu16HeartbeatCounter >> 8) & 0xFF;
            buf[1] = gu16HeartbeatCounter & 0xFF;

            n_bytes = sendto(sockfd, buf, PACKET_SIZE, 0, (const struct sockaddr *) &serveraddr, serverlen);
            if (n_bytes < 0)
            {
                ERROR_LOG(("failed to sendto\n"));
                retval = ERR_SENT_TO_HOST;
                goto err; // optional if allowed
            }
            else // maybe with #if DEBUGGING
            {
                DEBUG_LOG(("%d bytes sent to %s, counter = %d\n", n_bytes, server->h_name, gu16HeartbeatCounter));
            }

            if (retval == NO_ERROR)
            {
                n_bytes = recvfrom(sockfd, buf, PACKET_SIZE, 0, (__SOCKADDR_ARG) &serveraddr, &serverlen);
                if (n_bytes != PACKET_SIZE)
                {
                    DEBUG_LOG(("timeout or error in recvfrom server ACK\n"));
                }
                else
                {
                    DEBUG_LOG(("Ack from server: %02X:%02X\n", buf[0], buf[1]));
                    if (gu16HeartbeatCounter == ((uint16_t) (buf[0] & 0xFF) << 8 | (buf[1] & 0xFF)))
                    {
                        gu16HeartbeatCounter++;
                    }
                    else
                    {
                        DEBUG_LOG(("ACK mismatch, resend\n"));
                        n_bytes = -1; // to repeat sending loop
                    }
                }
            }

            if (is_stop_requested())
            {
                bDone = TRUE;
                break;
            }
        } while (n_bytes != PACKET_SIZE); // end of resending loop

        // the following section implements 1 sec delay between sendig messages
        // this is not 100% accurate but simple implementation on loop of 100ms to check STOP request and exit
        // it can also be implemented using timers and/or via queue timeout
        while (giSleepCounter > 0)
        {
            if (is_stop_requested())
            {
                bDone = TRUE;
                break; // exit loop, stop sending
            }
            usleep(SLEEP_DELAY_MS * 1000);
            giSleepCounter -= SLEEP_DELAY_MS;
        }

    } // end of process

err:
    if (sockfd > INVALID_SOCKET)
    {
        close(sockfd);
    }

    return retval;
}

///
/// \brief network_start_heartbeat
/// \return
///
int network_start_heartbeat(void)
{
    if (gbHbRunning)
        return NO_ERROR;

    gNetworkPid = fork();

    if(gNetworkPid >= 0) // fork was successful
    {
        if(gNetworkPid == 0) // child process (actual networking)
        {
            int retval = network_process_run();
            printf("network task completed with err = %d\n", retval);
            exit(0);
        }
        else //Parent (calling function) process
        {
            gbHbRunning = TRUE;
            DEBUG_LOG(("created network process pid = %d\n", gNetworkPid));
            return NO_ERROR;
        }
    }
    else // fork failed
    {
        printf("\n Fork failed, quitting!!!!!!\n");
        return -101;
    }

    return NO_ERROR;
}

///
/// \brief network_stop_heartbeat
/// \return
///
int network_stop_heartbeat(void)
{
    int returnStatus = NO_ERROR;
    int stop_request = IPC_STOP_REQ;

    if (gbHbRunning == FALSE)
        return NO_ERROR;

    // stop network process can be done with direct kill() sys call
    // however let's try to do it gracefully first just in case of memory allocations, etc.
    returnStatus = ipc_msg_send(MSGTYPE_INT, (void*) &stop_request, sizeof(stop_request));

    if (returnStatus == sizeof(stop_request))
    {
        DEBUG_LOG(("waiting for child network process pid = %d\n", gNetworkPid));

        waitpid(gNetworkPid, &returnStatus, 0);

        DEBUG_LOG(("network process completed with status = %d\n", returnStatus));
    }
    else
    {
        kill(gNetworkPid, SIGKILL);
    }

    gbHbRunning = FALSE;
    return returnStatus;
}

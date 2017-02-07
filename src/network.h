// defines network process public API
// to start/stop sending packets to server

#ifndef _NETWORK_H_
#define _NETWORK_H_

// networking definition
#define INVALID_SOCKET      (-1)
#define HOST_NAME           "localhost" // any url can be used for tests, e.g. "www.yahoo.com"
#define HOST_PORT           13469
#define PACKET_SIZE         2

#define ACK_TIMEOUT_MS      500
#define PACKET_INTERVAL_MS  1000
#define SLEEP_DELAY_MS      100

///
/// \brief network_start_heartbeat
/// \return NO_ERROR on success
///
int network_start_heartbeat(void);

///
/// \brief network_stop_heartbeat
/// \return NO_ERROR on success
///
int network_stop_heartbeat(void);

#endif // _NETWORK_H_

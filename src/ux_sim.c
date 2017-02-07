static const char info[] =
        "This module simulate UX for sample device\n"
        "Usage:\n"
        "space - toggle spring button: down/up\n"
        "b     - toggle battery state: low/normal\n"
        "c     - toggle charger state: in/out\ns"
        "e     - exit, end of the ux simulator\n";

#include<stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO

#include "ipc.h"
#include "common.h"

int main(void){
    int c;
    static struct termios oldt, newt;
    BOOL bButtonDown = FALSE;
    BOOL bLowPower = FALSE;
    BOOL bChargerIn = FALSE;
    BOOL bDone = FALSE;
    int msg_queue = ipc_get_queue();

    if (msg_queue <= 0)
    {
        perror("message queue failed");
        return -1;
    }

    /*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
    tcgetattr( STDIN_FILENO, &oldt);
    /*now the settings will be copied*/
    newt = oldt;

    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    newt.c_lflag &= ~(ICANON);

    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    printf("%s\n", info);
    /*This is your part:
    I choose 'e' to end input. Notice that EOF is also turned off
    in the non-canonical mode*/
    while(c = getchar())
    {
        int pl_val = 0;

        putchar('\n');
        switch (c)
        {
            case ' ':
                bButtonDown = !bButtonDown;
                pl_val = bButtonDown ? IPC_BUTTON_DOWN : IPC_BUTTON_UP;
                printf("command bButtonDown now is %s\n", bButtonDown ? "down":"up");
                ipc_msg_send_to(msg_queue, MSGTYPE_EXT, &pl_val, sizeof(pl_val));
            break;

            case 'b':
                bLowPower = !bLowPower;
                pl_val = bLowPower ? IPC_LOW_POWER : IPC_POWER_OK;
                printf("command bLowPower now is %s\n", bLowPower ? "low":"normal");
                ipc_msg_send_to(msg_queue, MSGTYPE_EXT, &pl_val, sizeof(pl_val));
            break;

            case 'c':
                bChargerIn = !bChargerIn;
                pl_val = bChargerIn ? IPC_CHARGER_ON : IPC_CHARGER_OFF;
                printf("command bChargerIn now is %s\n", bChargerIn ? "IN":"OUT");
                ipc_msg_send_to(msg_queue, MSGTYPE_EXT, &pl_val, sizeof(pl_val));
            break;

            case 'e':
                pl_val = IPC_STOP_REQ;
                ipc_msg_send_to(msg_queue, MSGTYPE_EXT, &pl_val, sizeof(pl_val));
                bDone = TRUE;
            break;

            default:
                printf("invalid input\n");
            break;
        }

        if (bDone)
            break;
    }
    printf("\nexited...\n");

    /*restore the old settings*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

    return 0;
}



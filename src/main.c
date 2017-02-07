#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "network.h"
#include "ipc.h"
#include "led.h"

int main(void)
{
    int retval, msg_val;
    BOOL bDone = FALSE;
    BOOL bButtonDown = FALSE;
    BOOL bLowPower = FALSE;

    // init section if required
    // e.g. queues_init() etc.
    led_init();

    // create main app queue
    retval = ipc_create_queue();

    if (retval < 0)
    {
        ERROR_LOG(("Can't create IPC queue\n"));
        return -1;
    }

    // section of registration for events if required
    // e.g. button_press_register()
    // charger_notify_register()
    // power_state_register()

    // main app loop
    while (!bDone)
    {
        if (ipc_msg_get(MSGTYPE_EXT, &msg_val, sizeof(msg_val), 0) > 0)
        {
            switch (msg_val)
            {
            case IPC_STOP_REQ    :
                DEBUG_LOG(("IPC_STOP_REQ\n"));
                bDone = TRUE;
                break;

            case IPC_BUTTON_DOWN :
                DEBUG_LOG(("IPC_BUTTON_DOWN\n"));
                bButtonDown = TRUE;
                if (bLowPower == FALSE)
                {
                    network_start_heartbeat();
                    led_start(LED_WHITE, LED_BLINK);
                }
                break;

            case IPC_BUTTON_UP   :
                DEBUG_LOG(("IPC_BUTTON_UP\n"));
                bButtonDown = FALSE;
                network_stop_heartbeat();
                if (!bLowPower)
                {
                    led_stop();
                }
                break;

            case IPC_LOW_POWER   :
                // assuming that if the battery voltage goes <3.5V the driver/task will send us
                // notification once - simulated in ux_sim.c
                DEBUG_LOG(("IPC_LOW_POWER\n"));
                bLowPower = TRUE;
                network_stop_heartbeat();
                led_start(LED_RED, LED_BLINK);
                break;

            case IPC_POWER_OK    :
                // assuming that if the battery voltage goes >=3.5V the driver/task will send us
                // notification once - simulated in ux_sim.c
                DEBUG_LOG(("IPC_POWER_OK\n"));
                bLowPower = FALSE;

                if (bButtonDown)
                {   // if still holding button - resume HB
                    network_start_heartbeat();
                    led_start(LED_WHITE, LED_BLINK);
                }
                else
                {
                    led_stop();
                }
                break;

            case IPC_CHARGER_ON  :
                DEBUG_LOG(("IPC_CHARGER_ON\n"));
                // not really sure what to do in this case
                // if charging "overrides" Low Power state then can act as IPC_POWER_OK
                if (bButtonDown)
                {   // if still holding button - resume HB
                    network_start_heartbeat();
                    led_start(LED_WHITE, LED_BLINK);
                }
                else
                {
                    led_stop();
                }
                break;

            case IPC_CHARGER_OFF :
                DEBUG_LOG(("IPC_CHARGER_OFF\n"));
                // not really sure what to do in this case
                // if Low Power state was overriden and no IPC_POWER_OK event received
                // then should probably return back to Low Power state
                if (bLowPower == TRUE)
                {
                    network_stop_heartbeat();
                    led_start(LED_RED, LED_BLINK);
                }
                break;

            default:
                DEBUG_LOG(("unexpected IPC msg = %d\n", msg_val));
                break;
            }
        }
    }

    // stop network process if running
    retval = network_stop_heartbeat();

    // section of unregistering and release resources
    // e.g. led_shutdow(), etc.
    ipc_remove_queue();

    return retval;
}

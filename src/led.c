// this module simulate asynchronous LED API for test purposes
// assuming the LED module/driver is separate task accessible via message queue for ctrl

#include <stdio.h>
#include "common.h"
#include "led.h"

static int gTagVal = 0;

///
/// \brief led_man_msg_send
/// \param aReq
///
static void led_man_msg_send(LedMsgFmt_t *aReq)
{
    // post command to LED driver

    // to suppress warning
    (void) aReq;
}

///
/// \brief led_init
///
void led_init(void)
{
    // init resources if required
}

///
/// \brief led_start
/// \param aColor
/// \param aType
///
void led_start(int aColor, int aType)
{
    LedMsgFmt_t req;

    // if animation in running then may need to be stopped
    led_stop();

    req.aTag = ++gTagVal;
    req.aType = aType;
    req.mColor = aColor;

    led_man_msg_send(&req);

    // simulate color change
    switch (aColor)
    {
    case LED_WHITE:
        DEBUG_LOG(("%sblinking white started\n", KWHT));
        break;
    case LED_RED:
        DEBUG_LOG(("%sblinking red started\n", KRED));
    default:
        break;
    }
}

///
/// \brief led_stop
///
void led_stop(void)
{
    LedMsgFmt_t req;

    req.aTag = gTagVal;
    req.aType = LED_OFF;

    led_man_msg_send(&req);

    // simulate LED stop
    DEBUG_LOG(("%sLED is OFF\n", KNRM));
}


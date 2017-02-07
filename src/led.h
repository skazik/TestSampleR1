// this module simulate asynchronous LED API for test purposes
// assuming the LED module/driver is separate task accessible via message queue for ctrl

#ifndef _LED_H_
#define _LED_H_

// types definitions
enum {
    LED_WHITE = 0,
    LED_RED,
} eLedColor_t;

enum {
    LED_OFF = 0,
    LED_ON,
    LED_BLINK,
} eLedAnimation_t;

typedef struct _LedMsgFmt
{
    int     aTag; // for sync, depends on implementation
    int     mColor;
    int     aType;
} LedMsgFmt_t;

///
/// \brief led_init
///
void led_init(void);

///
/// \brief led_start
/// \param aColor
/// \param aType
///
void led_start(int aColor, int aType);

///
/// \brief led_stop
///
void led_stop(void);

#endif // _LED_H_

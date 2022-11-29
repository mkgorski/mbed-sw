#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

typedef struct timer {
  uint32_t start;
  uint32_t interval;
} timer;

#define STOPPED_TIMER {0,-1}

void timer_start (timer *ti, uint32_t interval);
void timer_restart (timer *ti);
void timer_advance (timer *ti);
void timer_stop (timer *ti);

int timer_expired (timer *ti);
int timer_stopped (timer *ti);

uint32_t timer_value (timer *ti);
int timer_remaining (timer *ti);

#endif /* _TIMER_H_ */
#include "time.h"

void timer_start (timer *ti, uint32_t interval)
{
  ti->start = current_time();
  ti->interval = interval;
}

void timer_restart (timer *ti)
{
  ti->start = current_time();
}

void timer_advance (timer *ti)
{
  ti->start += ti->interval;
}

void timer_stop (timer *ti)
{
  timer_start (ti, 0xffffffff);
}

int timer_expired (timer *ti)
{
  return !timer_stopped (ti) && timer_value(ti) >= ti->interval;
}

int timer_stopped (timer *ti)
{
  return ti->interval == 0xffffffff;
}

uint32_t timer_value (timer *ti)
{
  return current_time() - ti->start;
}

int timer_remaining (timer *ti)
{
  return ti->interval - timer_value(ti);
}

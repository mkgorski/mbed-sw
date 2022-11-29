#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "utest.h"


static uint32_t __current_time = 0;
uint32_t current_time(void)
{
  return __current_time;
}

UTEST(timer, init)
{
  timer tim = STOPPED_TIMER;

  ASSERT_EQ(timer_stopped(&tim), 1);
}

UTEST(timer, start_stop)
{
  timer tim = STOPPED_TIMER;

  timer_start(&tim, 1000);
  ASSERT_EQ(timer_stopped(&tim), 0);
  ASSERT_EQ(timer_expired(&tim), 0);
  ASSERT_EQ(timer_remaining(&tim), 1000);

  timer_stop(&tim);
  ASSERT_EQ(timer_stopped(&tim), 1);
}

UTEST(timer, expired)
{
  timer tim = STOPPED_TIMER;

  timer_start(&tim, 1000);
  ASSERT_EQ(timer_expired(&tim), 0);

  __current_time += 1000;
  ASSERT_EQ(timer_expired(&tim), 1);
}

UTEST(timer, expired_edge)
{
  timer tim = STOPPED_TIMER;


  __current_time = 0xffffffff - 9;
  timer_start(&tim, 100);
  ASSERT_EQ(timer_expired(&tim), 0);

  __current_time = 10;
  ASSERT_EQ(timer_expired(&tim), 0);
  ASSERT_EQ(timer_remaining(&tim), 80);

  __current_time = 100;
  ASSERT_EQ(timer_expired(&tim), 1);
}

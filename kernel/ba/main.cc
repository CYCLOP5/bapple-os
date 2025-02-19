#include <console.h>
#include <memory.h>
#include <pair>
#include <stdint.h>
#include <stdio.h>
#include <systime.h>
#include <timer.h>
#include <vector>
#include "badapple.h"

using systime::systime_t;
using systime::get_systime;
using timer::handle_t;

namespace {

badapple::video v;

}

void play(void) {
  const int fps = 9;

  timer::add(TIMER_TICK_PER_SECOND, [](uint64_t count, handle_t h) {
    if (count < 3) {
      for (uint64_t i = 0; i <= count; ++i) {
        printf(".");
      }
    } else {
      timer::remove(h);
      timer::add(TIMER_TICK_PER_SECOND / fps, [](uint64_t, handle_t h) {
        if (v.has_next()) {
          v.next();
          printf(" (%d%%) ", v.progress());
        } else {
          mm::log_status(MM_LOG_SILENT);

          timer::remove(h);
          v.free();
          console::clear();
          timer::add(1, [](uint64_t, handle_t) -> void {
            systime_t *st = get_systime(NULL);
            printf("\rCurrent system time = %d/%02d/%02d %02d:%02d:%02d.",
              st->year, st->month, st->day,
              st->hour, st->minute, st->second);
          });
        }
      });
    }
  });
}

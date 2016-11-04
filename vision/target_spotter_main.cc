#include "vision/target_spotter.h"

#include <thread>

int main() {
  ::vision::target_spotter::TargetSpotter target_spotter;
  ::std::thread target_spotter_thread(::std::ref(target_spotter));

  // Wait forever.
  while (true) {
    const int r = select(0, nullptr, nullptr, nullptr, nullptr);
    (void) r;
  }

  target_spotter.Quit();
  target_spotter_thread.join();

  return 0;
}

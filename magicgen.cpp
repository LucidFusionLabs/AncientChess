#include "core/app/app.h"
#include "core/app/ipc.h"
#include "chess.h"

namespace LFL {
namespace Chess {
void GenerateMagicNumbers(int p, const BitBoard *occupancyMask, const vector<BitBoard> &occupancy_variation,
                          const vector<BitBoard> &attack_set, BitBoard *magic_number_out, int *magic_number_bits_out) {
  int bit_count=Bit::Count(occupancyMask[p]), variation_count=int(1LL << bit_count);
  vector<BitBoard> used_by(int(1LL << bit_count), 0);
  BitBoard magic_number;

  bool fail=0;
  for (int attempts=0; /**/; ++attempts, fail=0) {
    magic_number = Rand64() & Rand64() & Rand64();

    for (int i=0;          i<variation_count; i++) used_by[i] = 0;
    for (int i=0; !fail && i<variation_count; i++) {
      int magic_index = MagicMoves::MagicHash(occupancy_variation[i], magic_number, bit_count);
      fail = used_by[magic_index] != 0 && used_by[magic_index] != attack_set[i];
      used_by[magic_index] = attack_set[i];
    }
  } 
  while (fail);

  *magic_number_out = magic_number;
  *magic_number_bits_out = bit_count;
}

}; // namespace Chess
}; // namespace LFL
using namespace LFL;

extern "C" void MyAppCreate(int argc, const char* const* argv) {
  app = new Application(argc, argv);
  app->focused = new Window();
}

extern "C" int MyAppMain() {
  return 0;
}

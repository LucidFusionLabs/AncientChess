#include "core/app/app.h"
#include "core/app/ipc.h"
#include "chess.h"

using namespace LFL;

extern "C" void MyAppCreate(int argc, const char* const* argv) {
  app = new Application(argc, argv);
  app->focused = new Window();
}

extern "C" int MyAppMain() {
  string line;
  LFL::FLAGS_font = LFL::FakeFontEngine::Filename();
  CHECK_EQ(0, LFL::app->Create(__FILE__));
  LFL::Chess::Engine engine([](const string &s) { printf("%s", s.c_str()); });
  while (FGetsLine(&line)) engine.LineCB(line);
  return 0;
}

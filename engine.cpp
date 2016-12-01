#include "core/app/app.h"
#include "core/app/ipc.h"
#include "chess.h"

using namespace LFL;

extern "C" void MyAppCreate(int argc, const char* const* argv) {
  app = new Application(argc, argv);
  app->focused = new Window();
  app->logout = app->logerr = nullptr;
}

extern "C" int MyAppMain() {
  int len;
  char buf[4096];
  NextRecordDispatcher line_buf;
  LFL::FLAGS_font = LFL::FakeFontEngine::Filename();
  CHECK_EQ(0, LFL::app->Create(__FILE__));
  LFL::Chess::Engine engine([](const string &s) { write(1, s.data(), s.size()); });
  line_buf.cb = bind(&LFL::Chess::Engine::LineCB, &engine, _1);
  while ((len = read(0, buf, sizeof(buf))) > 0) line_buf.AddData(StringPiece(buf, len));
  return 0;
}

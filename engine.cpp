#include "core/app/app.h"
#include "core/app/ipc.h"

namespace LFL {
Application *app;
};

#include "chess.h"

using namespace LFL;

extern "C" LFApp *MyAppCreate(int argc, const char* const* argv) {
  app = make_unique<Application>(argc, argv).release();
  app->focused = app->framework->ConstructWindow(app).release();
  app->logout = app->logerr = nullptr;
  return app;
}

extern "C" int MyAppMain(LFApp*) {
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

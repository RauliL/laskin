/*
 * Copyright (c) 2023-2026, Rauli Laine
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <filesystem>
#include <optional>

#include "./window.hpp"

int
main(int argc, char** argv)
{
  auto app = Gtk::Application::create(
    "dev.rauli.laskin.gui",
    Gio::Application::Flags::HANDLES_COMMAND_LINE
  );
  auto context = Glib::RefPtr<laskin::gui::Context>(new laskin::gui::Context());
  std::optional<std::filesystem::path> script_path;

  app->signal_command_line().connect(
    [&app, &script_path](
      const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line
    ) -> int
    {
      int argc = 0;
      auto args = command_line->get_arguments(argc);

      script_path.reset();
      for (int i = 1; i < argc; ++i)
      {
        const auto arg = args[i];

        if (arg && *arg && arg[0] != '-')
        {
          script_path = std::filesystem::path(arg);
          break;
        }
      }
      g_strfreev(args);

      app->activate();

      return EXIT_SUCCESS;
    },
    false
  );

  app->signal_activate().connect(
    [app, context, &script_path]()
    {
      auto window = new laskin::gui::Window(context);

      app->add_window(*window);
      if (script_path)
      {
        window->load_script(*script_path);
      }
      window->present();
    }
  );

  return app->run(argc, argv);
}

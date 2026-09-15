/*
 * Copyright (c) 2018-2026, Rauli Laine
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
#include <cmath>
#include <numbers>

#include <SDL2/SDL.h>

#include "laskin/context.hpp"
#include "laskin/error.hpp"
#include "laskin/plugin.hpp"

using namespace laskin;

namespace
{
  struct turtle_state
  {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool sdl_initialized = false;
    double x = 0.0;
    double y = 0.0;
    double heading = 0.0;
    bool pen_down = true;

    static constexpr int width = 800;
    static constexpr int height = 600;

    void ensure_sdl()
    {
      if (sdl_initialized)
      {
        return;
      }

      if (SDL_Init(SDL_INIT_VIDEO) != 0)
      {
        throw error(error::type::system, SDL_GetError());
      }

      sdl_initialized = true;
    }

    void ensure_window()
    {
      ensure_sdl();

      if (window)
      {
        return;
      }

      window = SDL_CreateWindow(
        "Laskin Turtle",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
      );

      if (!window)
      {
        throw error(error::type::system, SDL_GetError());
      }

      renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
      );

      if (!renderer)
      {
        throw error(error::type::system, SDL_GetError());
      }

      clear_screen();
    }

    void to_screen(double logo_x, double logo_y, int& screen_x, int& screen_y)
    {
      screen_x = width / 2 + static_cast<int>(std::lround(logo_x));
      screen_y = height / 2 - static_cast<int>(std::lround(logo_y));
    }

    void present()
    {
      if (renderer)
      {
        SDL_RenderPresent(renderer);
      }
    }

    void draw_to(double target_x, double target_y)
    {
      ensure_window();

      int start_x;
      int start_y;
      int end_x;
      int end_y;

      to_screen(x, y, start_x, start_y);
      to_screen(target_x, target_y, end_x, end_y);

      if (pen_down)
      {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, start_x, start_y, end_x, end_y);
      }

      x = target_x;
      y = target_y;
    }

    void move(double distance)
    {
      const double radians = heading * std::numbers::pi / 180.0;
      draw_to(
        x + distance * std::sin(radians),
        y + distance * std::cos(radians)
      );
    }

    void turn(double degrees)
    {
      heading = std::fmod(heading + degrees, 360.0);

      if (heading < 0.0)
      {
        heading += 360.0;
      }
    }

    void home()
    {
      draw_to(0.0, 0.0);
      heading = 0.0;
    }

    void clear_screen()
    {
      ensure_window();

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderClear(renderer);
      present();

      x = 0.0;
      y = 0.0;
      heading = 0.0;
      pen_down = true;
    }

    void wait()
    {
      ensure_window();
      present();

      bool running = true;

      while (running)
      {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
          if (
            event.type == SDL_QUIT
            || event.type == SDL_KEYDOWN
            || event.type == SDL_MOUSEBUTTONDOWN
          )
          {
            running = false;
          }
        }

        SDL_Delay(16);
      }
    }
  };

  turtle_state& state()
  {
    static turtle_state instance;
    return instance;
  }
}

/**
 * turtle:show ( -- )
 *
 * Opens the turtle graphics window.
 */
LASKIN_PLUGIN_WORD(w_show)
{
  state().ensure_window();
}

/**
 * turtle:fd ( distance -- )
 *
 * Moves the turtle forward by the given distance.
 */
LASKIN_PLUGIN_WORD(w_fd)
{
  double distance;

  context >> distance;
  state().move(distance);
}

/**
 * turtle:bk ( distance -- )
 *
 * Moves the turtle backward by the given distance.
 */
LASKIN_PLUGIN_WORD(w_bk)
{
  double distance;

  context >> distance;
  state().move(-distance);
}

/**
 * turtle:rt ( degrees -- )
 *
 * Turns the turtle right by the given number of degrees.
 */
LASKIN_PLUGIN_WORD(w_rt)
{
  double degrees;

  context >> degrees;
  state().turn(degrees);
}

/**
 * turtle:lt ( degrees -- )
 *
 * Turns the turtle left by the given number of degrees.
 */
LASKIN_PLUGIN_WORD(w_lt)
{
  double degrees;

  context >> degrees;
  state().turn(-degrees);
}

/**
 * turtle:pu ( -- )
 *
 * Lifts the pen up so movement does not draw.
 */
LASKIN_PLUGIN_WORD(w_pu)
{
  state().pen_down = false;
}

/**
 * turtle:pd ( -- )
 *
 * Puts the pen down so movement draws lines.
 */
LASKIN_PLUGIN_WORD(w_pd)
{
  state().pen_down = true;
}

/**
 * turtle:home ( -- )
 *
 * Moves the turtle to the center and sets heading to north.
 */
LASKIN_PLUGIN_WORD(w_home)
{
  state().home();
}

/**
 * turtle:cs ( -- )
 *
 * Clears the screen and resets the turtle to the center.
 */
LASKIN_PLUGIN_WORD(w_cs)
{
  state().clear_screen();
}

/**
 * turtle:setxy ( x y -- )
 *
 * Moves the turtle to the given coordinates without changing heading.
 */
LASKIN_PLUGIN_WORD(w_setxy)
{
  double x;
  double y;

  context >> y >> x;
  state().draw_to(x, y);
}

/**
 * turtle:seth ( degrees -- )
 *
 * Sets the turtle heading in degrees. Zero points north.
 */
LASKIN_PLUGIN_WORD(w_seth)
{
  double degrees;

  context >> degrees;
  state().heading = std::fmod(degrees, 360.0);

  if (state().heading < 0.0)
  {
    state().heading += 360.0;
  }
}

/**
 * turtle:update ( -- )
 *
 * Updates the graphics window to show the latest drawing.
 */
LASKIN_PLUGIN_WORD(w_update)
{
  state().present();
}

/**
 * turtle:wait ( -- )
 *
 * Keeps the window open until it is closed or clicked.
 */
LASKIN_PLUGIN_WORD(w_wait)
{
  state().wait();
}

LASKIN_PLUGIN
{
  context.dictionary[U"turtle:show"] = w_show;
  context.dictionary[U"turtle:fd"] = w_fd;
  context.dictionary[U"turtle:bk"] = w_bk;
  context.dictionary[U"turtle:rt"] = w_rt;
  context.dictionary[U"turtle:lt"] = w_lt;
  context.dictionary[U"turtle:pu"] = w_pu;
  context.dictionary[U"turtle:pd"] = w_pd;
  context.dictionary[U"turtle:home"] = w_home;
  context.dictionary[U"turtle:cs"] = w_cs;
  context.dictionary[U"turtle:setxy"] = w_setxy;
  context.dictionary[U"turtle:seth"] = w_seth;
  context.dictionary[U"turtle:update"] = w_update;
  context.dictionary[U"turtle:wait"] = w_wait;
}

/*
 * Copyright (c) 2023, Rauli Laine
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
#pragma once

#include <gtkmm.h>

#include "laskin/context.hpp"

namespace laskin::gui
{
  class DictionaryDisplayColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    explicit DictionaryDisplayColumns();

    inline const Gtk::TreeModelColumn<Glib::ustring>& name_column() const
    {
      return m_name_column;
    }

    inline const Gtk::TreeModelColumn<Glib::ustring>& value_column() const
    {
      return m_value_column;
    }

  private:
    Gtk::TreeModelColumn<Glib::ustring> m_name_column;
    Gtk::TreeModelColumn<Glib::ustring> m_value_column;
  };

  class DictionaryDisplay : public Gtk::Bin
  {
  public:
    using word_activated_signal = sigc::signal<
      void,
      Glib::ustring,
      Glib::ustring
    >;

    explicit DictionaryDisplay();

    void update(const context::dictionary_type& dictionary);

    inline word_activated_signal& signal_word_activated()
    {
      return m_signal_word_activated;
    }

    inline const word_activated_signal& signal_word_activated() const
    {
      return m_signal_word_activated;
    }

  protected:
    void on_row_activated(
      const Gtk::TreeModel::Path& path,
      Gtk::TreeViewColumn* column
    );

  private:
    Gtk::ScrolledWindow m_scrolled_window;
    Gtk::TreeView m_tree_view;
    DictionaryDisplayColumns m_columns;
    Glib::RefPtr<Gtk::ListStore> m_tree_model;
    word_activated_signal m_signal_word_activated;
  };
}

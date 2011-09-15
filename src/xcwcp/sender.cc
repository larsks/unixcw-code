// vi: set ts=2 shiftwidth=2 expandtab:
//
// Copyright (C) 2001-2006  Simon Baldwin (simon_baldwin@yahoo.com)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include "../config.h"

#include <cstdlib>
#include <string>
#include <deque>
#include <sstream>

#include <qevent.h>

#include "sender.h"
#include "display.h"
#include "modeset.h"

#include "cwlib.h"

#include "i18n.h"

namespace cw {


//-----------------------------------------------------------------------
//  Class Sender
//-----------------------------------------------------------------------

// poll()
//
// Poll the CW library tone queue, and if it is getting low, arrange for
// more data to be passed in to the sender.
void
Sender::poll (const Mode *current_mode)
{
  if (current_mode->is_dictionary () || current_mode->is_keyboard ())
    {
      if (cw_get_tone_queue_length () <= 1)
        {
          // Arrange more data for the sender.  In dictionary modes, add more
          // random data if the queue is empty.  In keyboard mode, just
          // dequeue anything currently on the character queue.
          if (current_mode->is_dictionary () && send_queue_.empty ())
            {
              const DictionaryMode *dict_mode = current_mode->is_dictionary ();
              enqueue_string (std::string (1, ' ')
                              + dict_mode->get_random_word_group ());
            }

          dequeue_character ();
        }
    }
}


// handle_key_event()
//
// Specific handler for keyboard mode_key events.  Handles presses only;
// releases are ignored.
void
Sender::handle_key_event (QKeyEvent *event, const Mode *current_mode)
{
  if (current_mode->is_keyboard ())
    {
      if (event->type () == QEvent::KeyPress)
        {
          // If the key was backspace, remove the last queued character, or at
          // least try, and we are done.
          if (event->key () == QEvent::Key_Backspace)
            {
              delete_character ();
              event->accept ();
            }
          else
            {
              // Extract the ASCII keycode from the key event, and queue the
              // character for sending, converted to uppercase.
              const char c = event->ascii ();
              enqueue_string (std::string (1, c));

              // Accept the event if the character was sendable.  If not, it
              // won't have queued, and so by ignoring it we can let
              // characters such as Tab pass up to the parent.
              if (cw_check_character (toupper (c)))
                event->accept ();
            }
        }
    }
}


// clear()
//
// Flush the tone queue, empty the character queue, and set to idle.
void
Sender::clear ()
{
  cw_flush_tone_queue ();
  send_queue_.clear ();
  is_queue_idle_ = true;
}


// dequeue_character()
//
// Called when the CW send buffer is empty.  If the queue is not idle, take
// the next character from the queue and send it.  If there are no more queued
// characters, set the queue to idle.
void
Sender::dequeue_character ()
{
  if (!is_queue_idle_)
    {
      if (!send_queue_.empty ())
        {
          // Take the next character off the queue and send it.  We don't
          // expect sending to fail as only sendable characters are queued.
          const char c = toupper (send_queue_.front ());
          send_queue_.pop_front ();
          if (!cw_send_character (c))
            {
              perror ("cw_send_character");
              abort ();
            }

          // Update the status bar with the character being sent.
          std::ostringstream outs;
          outs << _("Sending '")
               << c << _("' at ") << cw_get_send_speed () << _(" WPM");
          display_->show_status (outs.str ());
        }
      else
        {
          is_queue_idle_ = true;
          display_->clear_status ();
        }
    }
}


// enqueue_string()
//
// Queues a string for sending by the CW sender.  Rejects any unsendable
// characters found in the string.  Rejection is silent.
void
Sender::enqueue_string (const std::string &word)
{
  bool is_queue_notify = false;

  // Add each character, and note if we need to change from idle.
  for (unsigned int index = 0; index < word.size (); index++)
    {
      const char c = toupper (word[index]);

      if (cw_check_character (c))
        {
          send_queue_.push_back (c);
          display_->append (c);

          if (is_queue_idle_)
            is_queue_notify = true;
        }
    }

  // If we queued any character, mark the queue as not idle.
  if (is_queue_notify)
    is_queue_idle_ = false;
}


// delete_character()
//
// Remove the most recently added character from the queue, provided that
// the dequeue hasn't yet reached it.  If there's nothing available to
// delete, fail silently.
void
Sender::delete_character ()
{
  if (!send_queue_.empty ())
    {
      send_queue_.pop_back ();
      display_->backspace ();
    }
}

}  // cw namespace

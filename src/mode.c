#include "vterm_internal.h"

#include <stdio.h>

void vterm_state_initmodes(vterm_t *e48)
{
  vterm_mode mode;
  for(mode = VTERM_MODE_NONE; mode < VTERM_MODE_MAX; mode++) {
    int val = 0;

    switch(mode) {
    case VTERM_MODE_DEC_CURSORBLINK:
    case VTERM_MODE_DEC_CURSORVISIBLE:
      val = 1;
      break;

    default:
      break;
    }

    vterm_state_setmode(e48, mode, val);
  }
}

static void mousefunc(int x, int y, int button, int pressed, void *data)
{
  vterm_t *e48 = data;
  vterm_state_t *state = e48->state;

  int old_buttons = state->mouse_buttons;

  if(pressed)
    state->mouse_buttons |= (1 << button);
  else
    state->mouse_buttons &= ~(1 << button);

  if(state->mouse_buttons != old_buttons) {
    if(button < 4) {
      vterm_push_output_sprintf(e48, "\e[M%c%c%c", pressed ? button + 31 : 35, x + 33, y + 33);
    }
  }
}

void vterm_state_setmode(vterm_t *e48, vterm_mode mode, int val)
{
  vterm_state_t *state = e48->state;

  int done = 0;
  if(state->callbacks && state->callbacks->setmode)
    done = (*state->callbacks->setmode)(e48, mode, val);

  switch(mode) {
  case VTERM_MODE_NONE:
  case VTERM_MODE_MAX:
    break;

  case VTERM_MODE_KEYPAD:
    e48->mode.keypad = val;
    break;

  case VTERM_MODE_DEC_CURSOR:
    e48->mode.cursor = val;
    break;

  case VTERM_MODE_DEC_CURSORBLINK:
    e48->mode.cursor_blink = val;
    break;

  case VTERM_MODE_DEC_CURSORVISIBLE:
    e48->mode.cursor_visible = val;
    break;

  case VTERM_MODE_DEC_MOUSE:
    if(state->callbacks && state->callbacks->setmousefunc) {
      if(val) {
        state->mouse_buttons = 0;
      }
      (*state->callbacks->setmousefunc)(e48, val ? mousefunc : NULL, e48);
    }
    break;

  case VTERM_MODE_DEC_ALTSCREEN:
    /* Only store that we're on the alternate screen if the usercode said it
     * switched */
    if(done)
      e48->mode.alt_screen = val;
    if(done && val) {
      if(state->callbacks && state->callbacks->erase) {
        vterm_rectangle_t rect = {
          .start_row = 0,
          .start_col = 0,
          .end_row = e48->rows,
          .end_col = e48->cols,
        };
        (*state->callbacks->erase)(e48, rect, state->pen);
      }
    }
    break;

  case VTERM_MODE_DEC_SAVECURSOR:
    e48->mode.saved_cursor = val;
    if(val) {
      state->saved_pos = state->pos;
    }
    else {
      vterm_position_t oldpos = state->pos;
      state->pos = state->saved_pos;
      if(state->callbacks && state->callbacks->movecursor)
        (*state->callbacks->movecursor)(e48, state->pos, oldpos, e48->mode.cursor_visible);
    }
    break;

  }
}

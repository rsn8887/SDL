/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_SWITCH

#include <switch.h>

#include "SDL_timer.h"
#include "SDL_events.h"
#include "SDL_log.h"
#include "SDL_mouse.h"
#include "SDL_switchvideo.h"
#include "SDL_switchmouse_c.h"
#include "../../events/SDL_mouse_c.h"

static uint64_t prev_buttons = 0;
static uint64_t last_timestamp = 0;
const uint64_t mouse_read_interval = 15; // in ms

void 
SWITCH_InitMouse(void)
{
}

void 
SWITCH_PollMouse(void)
{
	SDL_Window *window = SDL_GetFocusWindow();
	uint64_t buttons;
	uint64_t changed_buttons;
	MousePosition mouse_pos;
	uint64_t timestamp;
	int dx, dy;
	int wheel_dx, wheel_dy;

	// We skip polling mouse if no window is created
	if (window == NULL)
		return;

	buttons = hidMouseButtonsHeld();
	changed_buttons = buttons ^ prev_buttons;

	if (changed_buttons & MOUSE_LEFT) {
		if (prev_buttons & MOUSE_LEFT)
			SDL_SendMouseButton(window, 0, SDL_RELEASED, SDL_BUTTON_LEFT);
		else
			SDL_SendMouseButton(window, 0, SDL_PRESSED, SDL_BUTTON_LEFT);
	}
	if (changed_buttons & MOUSE_RIGHT) {
		if (prev_buttons & MOUSE_RIGHT)
			SDL_SendMouseButton(window, 0, SDL_RELEASED, SDL_BUTTON_RIGHT);
		else
			SDL_SendMouseButton(window, 0, SDL_PRESSED, SDL_BUTTON_RIGHT);
	}
	if (changed_buttons & MOUSE_MIDDLE) {
		if (prev_buttons & MOUSE_MIDDLE)
			SDL_SendMouseButton(window, 0, SDL_RELEASED, SDL_BUTTON_MIDDLE);
		else
			SDL_SendMouseButton(window, 0, SDL_PRESSED, SDL_BUTTON_MIDDLE);
	}

	prev_buttons = buttons;

	timestamp = SDL_GetTicks();

	if (SDL_TICKS_PASSED(timestamp, last_timestamp + mouse_read_interval)) {
		hidMouseRead(&mouse_pos);
		// if hidMouseRead is called once per frame, a factor two on the velocities
		// results in approximately the same mouse motion as reported by mouse_pos.x and mouse_pos.y
		// but without the clamping to 1280 x 720
		dx = mouse_pos.velocityX * 2;
		dy = mouse_pos.velocityY * 2;
		if (dx || dy) {
			SDL_SendMouseMotion(window, 0, 1, dx, dy);
		}

		wheel_dx = mouse_pos.scrollVelocityX;
		wheel_dy = mouse_pos.scrollVelocityY;
		if (wheel_dx || wheel_dy) {
			SDL_SendMouseWheel(window, 0, (float) wheel_dx, (float) wheel_dy, SDL_MOUSEWHEEL_NORMAL);
		}

		last_timestamp = timestamp;
	}
}

void 
SWITCH_QuitMouse(void)
{
}

#endif /* SDL_VIDEO_DRIVER_SWITCH */

/* vi: set ts=4 sw=4 expandtab: */

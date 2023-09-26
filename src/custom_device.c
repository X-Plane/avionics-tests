/*===--------------------------------------------------------------------------------------------===
 * custom_device.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 *
 *
 * Custom avionics device demo for X-Plane 12
 *===--------------------------------------------------------------------------------------------===
 */
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMUtilities.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "SystemGL.h"

void log_msg(const char *fmt, ...);
const char *click_type(int mouse);

#define WIDTH		480
#define	HEIGHT		360

#define BEZEL_SIZE	50
#define DEV_WIDTH	(2*BEZEL_SIZE + WIDTH)
#define DEV_HEIGHT	(2*BEZEL_SIZE + HEIGHT)

// Used to keep track of mouse down, drag, and up positions so we can show it on
// the cockpit display.
static int click_x = 0, click_y = 0;
static int clicked = false;
static XPLMAvionicsID device = NULL;
static XPLMCommandRef toggle_popup = NULL;

static int custom_keyboard(
	XPLMDeviceID id,
	char key,
	XPLMKeyFlags flags,
	char vkey,
	void *refcon,
	int losing
)
{
	(void)flags;
	(void)vkey;
	(void)refcon;
	(void)losing;

	log_msg("device %d: key %c (0x%02x) pressed", id, key, (int)key);
	
	// Return 1 only if you want to intercept the key press, and don't want X-Plane's device
	// to receive it.
	return 0;
}

static int custom_bezel_click(XPLMDeviceID id, int x, int y, int mouse, void *refcon)
{
	log_msg("device %d: bezel click %s at (%d, %d)", id, click_type(mouse), x, y);
	return 1;
}

static int custom_screen_click(XPLMDeviceID id, int x, int y, XPLMMouseStatus mouse, void *refcon)
{
	log_msg("device %d: screen touch %s at (%d, %d)", id, click_type(mouse), x, y);
	clicked = mouse != xplm_MouseUp;
	click_x = x;
	click_y = y;
	return 1;
}

static int custom_bezel(XPLMDeviceID id, int before, void *refcon)
{
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5);
    glVertex2f(0, 0);
    glVertex2f(0, DEV_HEIGHT);
    glVertex2f(DEV_WIDTH, DEV_HEIGHT);
    glVertex2f(DEV_WIDTH, 0);

    glColor3f(0, 0, 0);
    glVertex2f(BEZEL_SIZE, BEZEL_SIZE);
    glVertex2f(BEZEL_SIZE, BEZEL_SIZE+HEIGHT);
    glVertex2f(BEZEL_SIZE+WIDTH, BEZEL_SIZE+HEIGHT);
    glVertex2f(BEZEL_SIZE+WIDTH, BEZEL_SIZE);
    glEnd();
	
	return 1;
}

static int custom_screen(XPLMDeviceID id, int before, void *refcon)
{
	(void)id;
	(void)refcon;
	(void)before;
	
	XPLMSetGraphicsState(0, 0, 0, 0, 0, 0, 0);

	glLineWidth(5.0);
	glColor3f(1.f, 0.f, 1.f);
	
	glBegin(GL_LINES);
	glVertex2f(5, 			5);
	glVertex2f(5, 			HEIGHT - 10);
	
	glVertex2f(5, 			HEIGHT - 10);
	glVertex2f(WIDTH - 10, 	HEIGHT - 10);
	
	glVertex2f(WIDTH - 10, 	HEIGHT - 10);
	glVertex2f(WIDTH - 10, 	5);
	
	glVertex2f(WIDTH - 10, 	5);
	glVertex2f(5, 			5);
	
	glVertex2f(5, 			5);
	glVertex2f(WIDTH - 10,	HEIGHT - 10);
	
	
	glVertex2f(5, 			HEIGHT - 10);
	glVertex2f(WIDTH - 10,	5);
	glEnd();

	
	if(clicked)
	{
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "touch location: %d,%d", click_x, click_y);
		float color[3] = {1.f, 0.f, 1.f};
		XPLMDrawString(color, 50, 200, buffer, NULL, xplmFont_Proportional);
	}
	
	// If you return 0 in a `before` callback, X-Plane will go ahead and render
	// the stock device's screen;
	return 1;
}

static int handle_popup(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon)
{
	(void)cmd;
	
    if(phase != xplm_CommandBegin) return 1;
    XPLMAvionicsID id = refcon;
    XPLMSetAvionicsPopupVisible(id, !XPLMIsAvionicsPopupVisible(id));
    return 1;
}

void custom_device_init()
{
	XPLMCreateAvionics_t av = (XPLMCreateAvionics_t){
		.structSize = sizeof(XPLMCreateAvionics_t),
		.width = WIDTH,
		.height = HEIGHT,
		.bezelWidth = DEV_WIDTH,
		.bezelHeight = DEV_HEIGHT,
		.screenOffsetX = BEZEL_SIZE,
		.screenOffsetY = BEZEL_SIZE,
		.bezelDrawCallback = custom_bezel,
		.drawCallback = custom_screen,
		.touchScreenCallback = custom_screen_click,
		.bezelCallback = custom_bezel_click,
		.keyboardCallback = custom_keyboard,
		.deviceID = "TEST_AVIONICS"
	};
	device = XPLMCreateAvionicsEx(&av);
	
	toggle_popup = XPLMCreateCommand("laminar/avionics_test/toggle_popup", "Toggle Test Avionics Popup");
	XPLMRegisterCommandHandler(toggle_popup, handle_popup, 1, device);
	
}

void custom_device_fini()
{
	XPLMUnregisterCommandHandler(toggle_popup, handle_popup, 1, device);
	XPLMDestroyAvionics(device);
}

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
static int pos_x = 0, pos_y = 0;
static int clicked = false;
static int right_pos_x = 0, right_pos_y = 0;
static int right_clicked = false;
static XPLMAvionicsID device = NULL;
static XPLMCommandRef toggle_popup = NULL;

typedef struct {
    int x, y, w, h;
    bool clicked;
    bool right_clicked;
} ui_btn_t;

static ui_btn_t btns[] = {
    { 20, 20, 100, 40, false },
    { 130, 20, 100, 40, false },
};

#define BTN_COUNT   (2)

static bool in_button(const ui_btn_t *btn, int x, int y) {
    return x >= btn->x && x < (btn->x + btn->w)
        && y >= btn->y && y < (btn->y + btn->h);
}

static void draw_button(const ui_btn_t *btn, bool hover) {
    
    if(btn->right_clicked) {
        glColor3f(0.4, 0.6, 0.6);
    } else {
        glColor3f(0.4, 0.4, 0.4);
    }
    glBegin(GL_QUADS);
    glVertex2f(btn->x, btn->y);
    glVertex2f(btn->x, btn->y + btn->h);
    glVertex2f(btn->x + btn->w, btn->y + btn->h);
    glVertex2f(btn->x + btn->w, btn->y);
    glEnd();
    
    
    if(!btn->clicked && !hover)
        return;
    
    if(btn->clicked) {
        glColor3f(1, 0, 1);
    } else {
        glColor3f(1, 1, 1);
    }
    glLineWidth(2);
    glBegin(GL_LINES);
    
    glVertex2f(btn->x, btn->y);
    glVertex2f(btn->x, btn->y + btn->h);
    
    glVertex2f(btn->x, btn->y + btn->h);
    glVertex2f(btn->x + btn->w, btn->y + btn->h);

    glVertex2f(btn->x + btn->w, btn->y + btn->h);
    glVertex2f(btn->x + btn->w, btn->y);

    glVertex2f(btn->x + btn->w, btn->y);
    glVertex2f(btn->x, btn->y);
    glEnd();
}

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

static int custom_bezel_right_click(XPLMDeviceID id, int x, int y, int mouse, void *refcon)
{
	log_msg("device %d: bezel right click %s at (%d, %d)", id, click_type(mouse), x, y);
	return 1;
}

static int custom_bezel_scroll(XPLMDeviceID id, int x, int y, int wheel, int clicks, void *refcon)
{
	log_msg("device %d: bezel scroll %d (%d) at (%d, %d)", id, wheel, clicks, x, y);
	return 1;
}

static int custom_screen_click(XPLMDeviceID id, int x, int y, XPLMMouseStatus mouse, void *refcon)
{
	log_msg("device %d: screen touch %s at (%d, %d)", id, click_type(mouse), x, y);
	clicked = mouse != xplm_MouseUp;
	pos_x = x;
	pos_y = y;
    
    switch(mouse) {
    case xplm_MouseDown:
        for(int i = 0; i < BTN_COUNT; ++i) {
            if(in_button(&btns[i], x, y)) {
                btns[i].clicked = true;
            }
        }
        break;
    case xplm_MouseUp:
        for(int i = 0; i < BTN_COUNT; ++i) {
            btns[i].clicked = false;
        }
        break;
    default:
        break;
    }
	return 1;
}

static int custom_screen_right_click(XPLMDeviceID id, int x, int y, XPLMMouseStatus mouse, void *refcon)
{
	log_msg("device %d: screen touch %s at (%d, %d)", id, click_type(mouse), x, y);
	right_clicked = mouse != xplm_MouseUp;
	right_pos_x = x;
	right_pos_y = y;
    
    switch(mouse) {
    case xplm_MouseDown:
        for(int i = 0; i < BTN_COUNT; ++i) {
            if(in_button(&btns[i], x, y)) {
                btns[i].right_clicked = true;
            }
        }
        break;
    case xplm_MouseUp:
        for(int i = 0; i < BTN_COUNT; ++i) {
            btns[i].right_clicked = false;
        }
        break;
    default:
        break;
    }
	return 1;
}

static int custom_screen_scroll(XPLMDeviceID id, int x, int y, int wheel, int clicks, void *refcon)
{
	log_msg("device %d: screen scroll %d (%d) at (%d, %d)", id, wheel, clicks, x, y);
	return 1;
}

static int custom_screen_cursor(XPLMDeviceID id, int x, int y, void *refcon)
{
    return xplm_CursorHidden;
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
    
    
    int x = 0, y = 0;
    int hover = XPLMIsCursorOverAvionics(device, &x, &y);
    
    
    for(int i = 0; i < BTN_COUNT; ++i) {
        bool btn_hover = hover && in_button(&btns[i], x, y);
        draw_button(&btns[i], btn_hover);
    }
    
    
    if(clicked)
    {
        glBegin(GL_LINES);
        glColor3f(1.f, 0.f, 1.f);
        glVertex2f(pos_x - 15, pos_y - 10);
        glVertex2f(pos_x - 3, pos_y - 2);

        glVertex2f(pos_x - 15, pos_y + 10);
        glVertex2f(pos_x - 3, pos_y + 2);

        glVertex2f(pos_x + 15, pos_y + 10);
        glVertex2f(pos_x + 3, pos_y + 2);

        glVertex2f(pos_x + 15, pos_y - 10);
        glVertex2f(pos_x + 3, pos_y - 2);
        glEnd();
    }
    else if(hover)
    {
        glBegin(GL_LINES);
        glColor3f(1.f, 1.f, 1.f);
        glVertex2f(x - 15, y - 10);
        glVertex2f(x - 3, y - 2);

        glVertex2f(x - 15, y + 10);
        glVertex2f(x - 3, y + 2);

        glVertex2f(x + 15, y + 10);
        glVertex2f(x + 3, y + 2);

        glVertex2f(x + 15, y - 10);
        glVertex2f(x + 3, y - 2);
        glEnd();
    }
	
	if(clicked)
	{
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "left touch location: %d,%d", pos_x, pos_y);
		float color[3] = {1.f, 0.f, 1.f};
		XPLMDrawString(color, 50, 200, buffer, NULL, xplmFont_Proportional);
	}
    
    if(right_clicked)
    {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "right touch location: %d,%d", right_pos_x, right_pos_y);
        float color[3] = {1.f, 0.f, 1.f};
        XPLMDrawString(color, 50, 250, buffer, NULL, xplmFont_Proportional);
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
        .drawEveryFrame = true,
		.bezelDrawCallback = custom_bezel,
		.drawCallback = custom_screen,
		.screenTouchCallback = custom_screen_click,
        .screenRightTouchCallback = custom_screen_right_click,
        .screenScrollCallback = custom_screen_scroll,
        .screenCursorCallback = custom_screen_cursor,
		.bezelClickCallback = custom_bezel_click,
        .bezelRightClickCallback = custom_bezel_right_click,
        .bezelScrollCallback = custom_bezel_scroll,
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

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
#include <XPLMMenus.h>
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
static XPLMCommandRef show_popup = NULL;

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

    glBegin(GL_QUADS);
    if(btn->right_clicked) {
        glColor3f(0.4, 0.6, 0.6);
    } else {
        glColor3f(0.4, 0.4, 0.4);
    }
    glVertex2f(btn->x, btn->y);
    glVertex2f(btn->x, btn->y + btn->h);
    glVertex2f(btn->x + btn->w, btn->y + btn->h);
    glVertex2f(btn->x + btn->w, btn->y);
    glEnd();
    
    
    if(!btn->clicked && !hover)
        return;
    

    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    if(btn->clicked) {
        glColor4f(1, 0, 1, 1);
    } else {
        glColor4f(1, 1, 1, 1);
    }
    
    glVertex2f(btn->x, btn->y);
    glVertex2f(btn->x, btn->y + btn->h);
    glVertex2f(btn->x + btn->w, btn->y + btn->h);
    glVertex2f(btn->x + btn->w, btn->y);
    glEnd();
}

static void draw_cursor(int x, int y, float r, float g, float b) {
    glBegin(GL_LINE_LOOP);
    glColor4f(r, g, b, 1);
    
    glVertex2f(x - 3, y - 3);
    glVertex2f(x - 3, y + 3);
    glVertex2f(x + 3, y + 3);
    glVertex2f(x + 3, y - 3);
    glEnd();
    
    glBegin(GL_LINES);
    glColor4f(r, g, b, 1);
    
    glVertex2f(x, y - 12);
    glVertex2f(x, y - 3);
    
    glVertex2f(x, y + 12);
    glVertex2f(x, y + 3);
    
    glVertex2f(x - 15, y);
    glVertex2f(x - 3, y);
    
    glVertex2f(x + 15, y);
    glVertex2f(x + 3, y);
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
	return 0;
}

static int custom_bezel_right_click(XPLMDeviceID id, int x, int y, int mouse, void *refcon)
{
    if(mouse != xplm_MouseUp)
    {
        float brt = (float)y / (float)DEV_HEIGHT;
        XPLMSetAvionicsBrightness(device, brt);
        log_msg("brightness: %.2f", XPLMGetAvionicsBrightness(device));
    }
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

static void custom_bezel(float r, float b, float g, void *refcon)
{
	XPLMSetGraphicsState(0, 0, 0, 0, 1, 1, 0);
    glBegin(GL_QUADS);
    glColor4f(0.5 * r, 0.5 * b, 0.5 * g, 1.f);
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
}

static void custom_screen(void *refcon)
{
	(void)refcon;
	
	XPLMSetGraphicsState(0, 0, 0, 0, 1, 1, 0);
    glClearColor(0, 0, 0, 1);
    glPolygonMode(GL_FRONT, GL_FILL);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    int x = 0, y = 0;
    int hover = XPLMIsCursorOverAvionics(device, &x, &y);
    
    
    for(int i = 0; i < BTN_COUNT; ++i) {
        bool btn_hover = hover && in_button(&btns[i], x, y);
        draw_button(&btns[i], btn_hover);
    }
    
    
    if(clicked)
    {
        draw_cursor(pos_x, pos_y, 1, 0, 1);
    }
    else if(hover)
    {
        draw_cursor(x, y, 1, 1, 1);
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
}

static int handle_popup(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon)
{
	(void)cmd;
	
    if(phase != xplm_CommandBegin) return 1;
    XPLMAvionicsID id = refcon;
    XPLMSetAvionicsPopupVisible(id, 1);
    return 1;
}

void custom_device_init(XPLMMenuID menu)
{
	XPLMCreateAvionics_t av = (XPLMCreateAvionics_t){
		.structSize = sizeof(XPLMCreateAvionics_t),
		.screenWidth = WIDTH,
		.screenHeight = HEIGHT,
		.bezelWidth = DEV_WIDTH,
		.bezelHeight = DEV_HEIGHT,
		.screenOffsetX = BEZEL_SIZE,
		.screenOffsetY = BEZEL_SIZE,
        .drawOnDemand = false,
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
		.deviceID = "TEST_AVIONICS",
        .deviceName = "Test Avionics 9000"
	};
	device = XPLMCreateAvionicsEx(&av);
	
	show_popup = XPLMCreateCommand("laminar/avionics_test/show_popup", "Show Test Avionics Popup");
	XPLMRegisterCommandHandler(show_popup, handle_popup, 1, device);
	
	if(menu)
		XPLMAppendMenuItemWithCommand(menu, "Open Test Device Popup", show_popup);
}

void custom_device_fini()
{
	XPLMUnregisterCommandHandler(show_popup, handle_popup, 1, device);
	XPLMDestroyAvionics(device);
}

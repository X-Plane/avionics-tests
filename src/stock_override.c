/*===--------------------------------------------------------------------------------------------===
 * stock_override.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 *
 *
 * Stock cockpit device override demo for X-Plane 12
 *===--------------------------------------------------------------------------------------------===
 */
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMMenus.h>
#include <XPLMUtilities.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SystemGL.h"

void log_msg(const char *fmt, ...);
const char *click_type(int mouse);

// Used to keep track of mouse down, drag, and up positions so we can show it on
// the cockpit display.
static int click_x = 0, click_y = 0;
static int clicked = false;

static XPLMAvionicsID gns530_1 = NULL;
static XPLMAvionicsID gns430_2 = NULL;
static XPLMAvionicsID cdu_1 = NULL;
static XPLMCommandRef show_popout = NULL;

static XPLMMenuID devices_menu = NULL;
static int devices_menu_item = -1;

static XPLMDeviceID device_ids[] = {
    xplm_device_GNS430_1,
    xplm_device_GNS430_2,
    xplm_device_GNS530_1,
    xplm_device_GNS530_2,
    xplm_device_CDU739_1,
    xplm_device_CDU739_2,
    xplm_device_G1000_PFD_1,
    xplm_device_G1000_MFD,
    xplm_device_G1000_PFD_2,
    xplm_device_CDU815_1,
    xplm_device_CDU815_2,
    xplm_device_Primus_PFD_1,
    xplm_device_Primus_PFD_2,
    xplm_device_Primus_MFD_1,
    xplm_device_Primus_MFD_2,
    xplm_device_Primus_MFD_3,
    xplm_device_Primus_RMU_1,
    xplm_device_Primus_RMU_2,	
};

static const char *device_str[] = {
    "xplm_device_GNS430_1",
    "xplm_device_GNS430_2",
    "xplm_device_GNS530_1",
    "xplm_device_GNS530_2",
    "xplm_device_CDU739_1",
    "xplm_device_CDU739_2",
    "xplm_device_G1000_PFD_1",
    "xplm_device_G1000_MFD",
    "xplm_device_G1000_PFD_2",
    "xplm_device_CDU815_1",
    "xplm_device_CDU815_2",
    "xplm_device_Primus_PFD_1",
    "xplm_device_Primus_PFD_2",
    "xplm_device_Primus_MFD_1",
    "xplm_device_Primus_MFD_2",
    "xplm_device_Primus_MFD_3",
    "xplm_device_Primus_RMU_1",
    "xplm_device_Primus_RMU_2",	
};

static const char *device_names[] = {
    "GNS430 1",
    "GNS430 2",
    "GNS530 1",
    "GNS530 2",
    "CDU739 1",
    "CDU739 2",
    "G1000 PFD 1",
    "G1000 MFD",
    "G1000 PFD 2",
    "CDU815 1",
    "CDU815 2",
    "Primus_PFD 1",
    "Primus_PFD 2",
    "Primus_MFD 1",
    "Primus_MFD 2",
    "Primus_MFD 3",
    "Primus_RMU 1",
    "Primus_RMU 2",	
};

static const int device_count = sizeof(device_ids) / sizeof(device_ids[0]);

static int stock_keyboard(
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
    XPLMDeviceID id = (XPLMDeviceID)(intptr_t)refcon;

	log_msg("%s: key %c (0x%02x) pressed", device_str[id], key, (int)key);
	
	// Return 1 only if you want to intercept the key press, and don't want X-Plane's device
	// to receive it.
	return 0;
}

static int stock_bezel_click(int x, int y, int mouse, void *refcon)
{
    XPLMDeviceID id = (XPLMDeviceID)(intptr_t)refcon;
	log_msg("%s: bezel click %s at (%d, %d)", device_str[id], click_type(mouse), x, y);
	// Return 1 only if you want to intercept the key click, and don't want X-Plane's device
	// to receive it.    
	return 0;
}

static int stock_bezel_right_click(int x, int y, int mouse, void *refcon)
{
    XPLMDeviceID id = (XPLMDeviceID)(intptr_t)refcon;
    if(mouse != xplm_MouseUp)
    {
        float brt = (float)y / 200.f;
        XPLMSetAvionicsBrightnessRheo(gns530_1, brt);
        log_msg("brightness: %.2f", XPLMGetAvionicsBrightnessRheo(gns530_1));
    }
    return 1;
}

static int stock_bezel_scroll(int x, int y, int wheel, int clicks, void *refcon)
{
    XPLMDeviceID id = (XPLMDeviceID)(intptr_t)refcon;
    log_msg("%s: bezel scroll %d (%d) at (%d, %d)", device_str[id], wheel, clicks, x, y);
    
    return 0;
}

static int stock_screen_click(int x, int y, XPLMMouseStatus mouse, void *refcon)
{
    XPLMDeviceID id = (XPLMDeviceID)(intptr_t)refcon;
	log_msg("%s: screen touch %s at (%d, %d)", device_str[id], click_type(mouse), x, y);
	if(id == xplm_device_GNS530_1)
	{
		click_x = x;
		click_y = y;
		clicked = mouse != xplm_MouseUp;
	}
	
	// Return 1 if you want to intercept the screen touch. To keep receiving drags, you must
	// keep returning 1.
	return mouse != xplm_MouseUp;
}

static XPLMCursorStatus stock_screen_cursor(int x, int y, void *refcon) {
    XPLMDeviceID id = (XPLMDeviceID)(intptr_t)refcon;
    log_msg("%s: screen cursor at (%d, %d)", device_str[id], x, y);
    bool in_rect = x > 0 && x < 100 && y > 100 && y < 200;
    
    return in_rect ? xplm_CursorHidden : xplm_CursorArrow;
}

static int stock_draw(XPLMDeviceID id, int before, void *refcon)
{
	(void)refcon;
	
	int x = before ? 0 : 100;
	int y = before ? 100 : 0;
	
#if XPLM411
    auto gtex = XPLMGetTexture(xplm_Tex_Radar_Pilot);           // This is the pilot side radar, if the airplane has it installed. If the acf doesn't have it, this returns 0.
    if (gtex > 0 && id == xplm_device_GNS530_1 && !before)      // Let's draw the radar onto the 530!
    {
        XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
        XPLMBindTexture2d(gtex, 0);                             // Bind the non-null texture that we got for the radar
        glBegin(GL_QUADS);
        glColor3f(1.f, 1.f, 1.f);
        glTexCoord2f(0, 0); glVertex2f(0,  0);
        glTexCoord2f(0, 1); glVertex2f(0,  520.f/M_SQRT2);      // The radar texture is an A-landscape, so it's sqrt(2) times as wide as it is high.
        glTexCoord2f(1, 1); glVertex2f(520,520.f/M_SQRT2);      // or, to put it here on a surface of known width, it is 1/sqrt(2) times as high as it is wide.
        glTexCoord2f(1, 0); glVertex2f(520,0);
        glEnd();
        return 1;
    }
#endif

    XPLMSetGraphicsState(0, 0, 0, 0, 0, 0, 0);

	glBegin(GL_QUADS);
	if(before)
		glColor3f(1.f, 0.f, 1.f);
	else
		glColor3f(0.f, 1.f, 1.f);
    glVertex2f(x, 		y);
    glVertex2f(x, 		y+100);
    glVertex2f(x+100, 	y+100);
    glVertex2f(x+100, 	y);
	glEnd();
	
	if(!before && id == xplm_device_GNS530_1 && clicked)
	{
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "touch location: %d,%d", click_x, click_y);
		float color[3] = {1.f, 0.f, 1.f};
		XPLMDrawString(color, 0, 300, buffer, NULL, xplmFont_Proportional);
	}
	
	// If you return 1 in a `before` callback, X-Plane will go ahead and render
	// the stock device's screen;
	return !before || id != xplm_device_GNS430_2;
}

static XPLMAvionicsID register_device(XPLMDeviceID id, XPLMAvionicsCallback_f draw)
{
	XPLMCustomizeAvionics_t av = (XPLMCustomizeAvionics_t) {
		.structSize = sizeof(XPLMCustomizeAvionics_t),
		.deviceId = id,
		.drawCallbackBefore = draw,
		.drawCallbackAfter = draw,
		.bezelClickCallback = stock_bezel_click,
        .bezelRightClickCallback = stock_bezel_right_click,
        .bezelScrollCallback = stock_bezel_scroll,
		.screenTouchCallback = stock_screen_click,
        .screenCursorCallback = stock_screen_cursor,
		.keyboardCallback = stock_keyboard,
		.refcon = (void *)(intptr_t)id
	};

	char debug_str[128];
	if(draw)
	{
        log_msg("Drawing overrides for stock device %s", device_str[id]);
	} else {
		log_msg("Non-drawing overrides for stock device %s", device_str[id]);
	}
	return XPLMRegisterAvionicsCallbacksEx(&av);
}



// Make a list of all devices, let users pick one and check whether it's bound
static void menu_handler(void *menu_ptr, void *item_ptr)
{
    XPLMDeviceID id = (XPLMDeviceID)(intptr_t)item_ptr;
    XPLMAvionicsID handle = XPLMGetAvionicsHandle(id);
    if(!handle)
    {
        log_msg("could not get handle for %s (0x%02x)", device_str[id], id);
        return;
    }
    bool bound = XPLMIsAvionicsBound(handle);
    
    log_msg("%s (0x%02x) %s", device_str[id], id, bound ? "bound" : "not bound");
}

static void create_menus(XPLMMenuID parent)
{
    devices_menu_item = XPLMAppendMenuItem(parent, "Check Device Binds", NULL, 0);
    devices_menu = XPLMCreateMenu("Check Device Binds", parent, devices_menu_item, menu_handler, NULL);
    
    for(int i = 0; i < device_count; ++i)
    {
        XPLMAppendMenuItem(devices_menu, device_names[i], (void *)(intptr_t)device_ids[i], 0);
    }
}

static int handle_530_popup(XPLMCommandRef cmd, XPLMCommandPhase phase, void *refcon)
{
	(void)cmd;
	
    if(phase != xplm_CommandBegin)
        return 1;
    XPLMAvionicsID id = refcon;
    XPLMPopOutAvionics(id);
    return 1;
}

void stock_overrides_init(XPLMMenuID menu)
{
	gns530_1 = register_device(xplm_device_GNS530_1, stock_draw);
	gns430_2 = register_device(xplm_device_GNS430_2, stock_draw);
	cdu_1 = register_device(xplm_device_CDU739_1, NULL);
    
    create_menus(menu);
    
	show_popout = XPLMCreateCommand("laminar/avionics_test/show_530_popout", "Show GNS 530 Popout");
	XPLMRegisterCommandHandler(show_popout, handle_530_popup, 1, gns530_1);
	
	if(menu)
		XPLMAppendMenuItemWithCommand(menu, "Open GNS 530 Popout", show_popout);
}

void stock_overrides_fini()
{
	XPLMUnregisterCommandHandler(show_popout, handle_530_popup, 1, gns530_1);
    XPLMClearAllMenuItems(devices_menu);
    XPLMDestroyMenu(devices_menu);
    devices_menu = NULL;
    devices_menu_item = -1;
    
	XPLMUnregisterAvionicsCallbacks(gns530_1);
	XPLMUnregisterAvionicsCallbacks(gns430_2);
	XPLMUnregisterAvionicsCallbacks(cdu_1);
}

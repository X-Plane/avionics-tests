/*===--------------------------------------------------------------------------------------------===
 * plugin.c
 *
 * Created by Amy Parent <amy@amyparent.com>
 *===--------------------------------------------------------------------------------------------===
*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <XPLMDataAccess.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMMenus.h>
#include <XPLMPlugin.h>
#include <XPLMUtilities.h>
#include <XPLMProcessing.h>

#include "SystemGL.h"


#define PLUGIN_SIG  "com.x-plane.avionics"
#define PLUGIN_NAME "Avionics Checks"
#define PLUGIN_DESC "Creates a basic avionics device"

static XPLMMenuID menu = NULL;
static int menu_item = -1;

void stock_overrides_init(XPLMMenuID menu);
void stock_overrides_fini();

void custom_device_init(XPLMMenuID menu);
void custom_device_fini();


const char *click_type(int mouse) {
	switch(mouse) {
		case xplm_MouseDown: return "down";
		case xplm_MouseDrag: return "drag";
		case xplm_MouseUp: return "up";
	}
	return "";
}

void log_msg(const char *fmt, ...)
{
	char data[2048];
	va_list args;
	va_start(args, fmt);
	vsnprintf(data, sizeof(data), fmt, args);
	va_end(args);
	XPLMDebugString(data);
	XPLMDebugString("\n");
	fprintf(stderr, "[AVIONICS] %s\n", data);
}

PLUGIN_API int XPluginStart(char *name, char *sig, char *desc)
{
	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    
	strcpy(name, PLUGIN_NAME);
	strcpy(sig, PLUGIN_SIG);
	strcpy(desc, PLUGIN_DESC);
    
    int xp_ver = 0, xplm_ver = 0;
    XPLMGetVersions(&xp_ver, &xplm_ver, NULL);
    log_msg("XP Version: %d, XPLM Version: %d", xp_ver, xplm_ver);
    
    return 1;
}

PLUGIN_API void XPluginStop(void)
{
	
}


PLUGIN_API int XPluginEnable(void)
{
    // Create a menu to stuff all of our test commands
    XPLMMenuID plugins_menu = XPLMFindPluginsMenu();
    menu_item = XPLMAppendMenuItem(plugins_menu, "Avionics Test", NULL, 0);
    menu = XPLMCreateMenu("Avionics Tests", plugins_menu, menu_item, NULL, NULL);
    
	stock_overrides_init(menu);
	custom_device_init(menu);
    return 1;
}

PLUGIN_API void XPluginDisable(void)
{
	stock_overrides_fini();
	custom_device_fini();
    XPLMClearAllMenuItems(menu);
    XPLMDestroyMenu(menu);

    XPLMMenuID plugins_menu = XPLMFindPluginsMenu();
    XPLMRemoveMenuItem(plugins_menu, menu_item);
    
    menu = NULL;
    menu_item = -1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID from, int msg, void *param)
{
    (void)from;
    (void)msg;
    (void)param;
}

#if	IBM
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID resvd)
{
	(void)hinst;
	(void)resvd;
    (void)reason;
	return (TRUE);
}
#endif	/* IBM */

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

void stock_overrides_init();
void stock_overrides_fini();

void custom_device_init();
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
    
    return 1;
}

PLUGIN_API void XPluginStop(void)
{
	
}

PLUGIN_API int XPluginEnable(void)
{
	stock_overrides_init();
	custom_device_init();
    return 1;
}

PLUGIN_API void XPluginDisable(void)
{
	stock_overrides_fini();
	custom_device_fini();
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

/*===--------------------------------------------------------------------------------------------===
 * SystemGL.h
 *
 * Created by Amy Parent <amy@amyparent.com>
 *===--------------------------------------------------------------------------------------------===
*/
#ifndef _SYSTEMGL_H_
#define _SYSTEMGL_H_

#if IBM
#include <GL/gl.h>
// #include <GL/glu.h>
#elif LIN
#define TRUE 1
#define FALSE 0
#include <GL/gl.h>
// #include <GL/glu.h>
#else
#define TRUE 1
#define FALSE 0
#if __GNUC__
#include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
#else
#include <gl.h>
// #include <glu.h>
#endif
#endif

#endif /* ifndef _SYSTEMGL_H_ */


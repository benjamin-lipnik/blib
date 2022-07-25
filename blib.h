#ifndef BLIB_H
#define BLIB_H

#ifndef WINDOWS
	#include <GLES3/gl3.h>
#else
	#include <GL/glew.h>
#endif

#include "bint.h"
#include "butil.h"
#include "assetloader.h"
#include "ppmloader.h"
#include "sload.h"
#include "brender.h"
#include "ui.h"

#endif

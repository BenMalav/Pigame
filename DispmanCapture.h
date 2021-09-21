//
// Created by APel on 19/09/21.
//

#ifndef PI_GAME_DISPMANCAPTURE_H
#define PI_GAME_DISPMANCAPTURE_H

#include <cstdio>
#include "bcm_host.h"

class DispmanCapture {
public:
    DispmanCapture();
    ~DispmanCapture(){}

    void initDispman();


private:
    u_int32_t requestedWidth = 0;
    u_int32_t requestedHeight = 0;
    u_int32_t defDisplay = 0;
    int8_t dmxBytesPerPixel  = 4;
    VC_IMAGE_TYPE_T imageType = VC_IMAGE_RGBA32;
    DISPMANX_DISPLAY_HANDLE_T displayHandle;
};


#endif //PI_GAME_DISPMANCAPTURE_H

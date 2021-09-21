//
// Created by APel on 19/09/21.
//

#include "DispmanCapture.h"

DispmanCapture::DispmanCapture() {
    initDispman();
}


void DispmanCapture::initDispman() {
    bcm_host_init();
    displayHandle = vc_dispmanx_display_open(defDisplay);


    if (displayHandle == 0)
    {
        fprintf(stderr,
                "unable to open eglDisplay id: %d\n",
                defDisplay);
    }
}
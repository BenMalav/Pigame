//
// Created by APel on 19/09/21.
//

#ifndef PI_GAME_GRAPHICSCONTEXT_H
#define PI_GAME_GRAPHICSCONTEXT_H

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <stdexcept>
#include <memory>

class GraphicsContext {
public:
    GraphicsContext();
    ~GraphicsContext();

    void initDRM();
    void initEGL();
    void swapBuffers();
    drmModeConnector *findConnector() noexcept;

private:

    // Used for the smart pointer deleter implementation
    static void freeDrmModeRes(drmModeRes* ptr)
    {
        drmModeFreeResources(ptr);
    }

    static void freeDrmModeCon(drmModeConnector* ptr)
    {
        drmModeFreeConnector(ptr);
    }

    static void freeDrmModeEnc(drmModeEncoder* ptr)
    {
        drmModeFreeEncoder(ptr);
    }

    static void freeDrmModeCrtc(drmModeCrtc* ptr)
    {
        drmModeFreeCrtc(ptr);
    }

    const char* drmDevice = "/dev/dri/card1";
    int drmDeviceFd;
    std::shared_ptr<drmModeRes> resources;
    std::shared_ptr<drmModeConnector> connector;
    std::shared_ptr<drmModeEncoder> encoder;
    std::shared_ptr<drmModeCrtc> crtc;

    drmModeModeInfo modeInfo;

    uint32_t connectorId;
    struct gbm_device* gbmDevice;
    EGLDisplay eglDisplay;
    EGLContext context;
    struct gbm_surface* gbmSurface;
    EGLSurface eglSurface;

};


#endif //PI_GAME_GRAPHICSCONTEXT_H

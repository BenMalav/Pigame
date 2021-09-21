//
// Created by APel on 19/09/21.
//

#include <fcntl.h>
#include <cstdio>
#include <cerrno>
#include <iostream>
#include <cstring>
#include "GraphicsContext.h"

GraphicsContext::GraphicsContext()
{
    try {
        initDRM();
        initEGL();
    }catch (std::runtime_error& e){
        throw;
    }
}

GraphicsContext::~GraphicsContext()
{
    // close the opened DRM fd
    close(drmDeviceFd);
}

void GraphicsContext::swapBuffers() {
    eglSwapBuffers(eglDisplay, eglSurface);
    struct gbm_bo *bo = gbm_surface_lock_front_buffer(gbmSurface);
    uint32_t handle = gbm_bo_get_handle(bo).u32;
    uint32_t pitch = gbm_bo_get_stride(bo);
    uint32_t fb;
    drmModeAddFB(drmDeviceFd, modeInfo.hdisplay, modeInfo.vdisplay, 24, 32, pitch, handle, &fb);
    drmModeSetCrtc(drmDeviceFd, crtc->crtc_id, fb, 0, 0, &connectorId, 1, &modeInfo);

    struct gbm_bo *previous_bo;
    uint32_t previous_fb;

    if (previous_bo) {
        drmModeRmFB (drmDeviceFd, previous_fb);
        gbm_surface_release_buffer (gbmSurface, previous_bo);
    }
    previous_bo = bo;
    previous_fb = fb;
}


void GraphicsContext::initEGL()
{
    // GBM device and then create the EGL eglDisplay
    gbmDevice = gbm_create_device(drmDeviceFd);
    if (gbmDevice == nullptr)
        throw std::runtime_error("Failed creating the GBM device");

    eglDisplay = eglGetDisplay(gbmDevice);
    if (eglDisplay == EGL_NO_DISPLAY)
        throw std::runtime_error("Failed creating the EGL display");


    eglInitialize(eglDisplay, NULL, NULL);
    eglBindAPI(EGL_OPENGL_ES_API);

    // Get the first matching EGL config
    const EGLint attributes[13] =
            {
                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                    EGL_RED_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE, 8,
                    EGL_ALPHA_SIZE, 0,
                    EGL_RENDERABLE_TYPE,
                    EGL_OPENGL_ES2_BIT,
                    EGL_NONE
            };
    EGLConfig config;
    EGLint count;
    EGLint numConfig;
    if(eglChooseConfig(eglDisplay, attributes, &config, 1, &numConfig) == EGL_FALSE)
        throw std::runtime_error("Failed getting a matching EGL config");


    const EGLint context_attribs[3] =
            {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE
            };
    context = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, context_attribs);
    eglSurface = eglCreateWindowSurface(eglDisplay, config, gbmSurface, NULL);
    gbmSurface = gbm_surface_create(gbmDevice, modeInfo.hdisplay, modeInfo.vdisplay,
                                    GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, context);
    printf("%s \n", glGetString(GL_RENDERER));
    printf("%s \n", glGetString(GL_VERSION));
}

void GraphicsContext::initDRM()
{
    if (drmAvailable() == 0)
        throw std::runtime_error("DRM driver not loaded? wtf");

    //try to open the drmDeviceFd, if fails throw an exception
    drmDeviceFd = open(drmDevice, O_RDWR);
    if(drmDeviceFd < 0)
        throw std::runtime_error("Failed to open the DRM eglDisplay! check the path..");

    auto resPtr = std::shared_ptr<drmModeRes>(drmModeGetResources(drmDeviceFd), freeDrmModeRes);
    resources = resPtr;
    if (resources == nullptr)
        throw std::runtime_error("No DRM resources found.");

    auto conPtr = std::shared_ptr<drmModeConnector> (findConnector(), freeDrmModeCon);
    connector = conPtr;
    if (connector == nullptr)
        throw std::runtime_error("No DRM connector found.");

    connectorId = connector->connector_id;
    modeInfo = connector->modes[0];
    std::cout << "DRM resolution " << modeInfo.hdisplay << ' ' << modeInfo.vdisplay << std::endl;

    auto encPtr = std::shared_ptr<drmModeEncoder> (drmModeGetEncoder(drmDeviceFd, connector->encoder_id),
                                                   freeDrmModeEnc);
    encoder = encPtr;
    if (!encoder)
        throw std::runtime_error("No DRM encoder!");

    auto crtcPtr = std::shared_ptr<drmModeCrtc> (drmModeGetCrtc(drmDeviceFd, encoder->crtc_id),
                                                 freeDrmModeCrtc);
    crtc = crtcPtr;
    if (!crtc)
        throw std::runtime_error("No DRM crtc!");

}

// go through connector and grab the first attached connector
drmModeConnector * GraphicsContext::findConnector() noexcept
{
    for (int i = 0; i < resources->count_connectors; i++){
        drmModeConnector *tmpConnector = drmModeGetConnector(drmDeviceFd, resources->connectors[i]);
        if (tmpConnector->connection == DRM_MODE_CONNECTED){
            return tmpConnector;
        }
    }
    return nullptr; // if no connector found
}
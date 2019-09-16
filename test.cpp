// gcc -o drm-gbm test.c -ldrm -lgbm -lEGL -lGL -I/usr/include/libdrm

//----------------------------------------------------------------------
//--------  Trying to get OpenGL ES screen on RPi4 without X
//--------  based on drm-gbm https://github.com/eyelash/tutorials/blob/master/drm-gbm.c
//--------  and kmscube https://github.com/robclark/kmscube
//--------  pik33@o2.pl
//----------------------------------------------------------------------

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "Shader.h"
#include "Model.h"
#include "ShapeGenerator.h"
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>

#define EXIT(msg)           \
    {                       \
        fputs(msg, stderr); \
        exit(EXIT_FAILURE); \
    }

// global variables declarations

static int device;
static drmModeRes *resources;
static drmModeConnector *connector;
static uint32_t connector_id;
static drmModeEncoder *encoder;
static drmModeModeInfo mode_info;
static drmModeCrtc *crtc;
static struct gbm_device *gbm_device;
static EGLDisplay display;
static EGLContext context;
static struct gbm_surface *gbm_surface;
static EGLSurface egl_surface;
EGLConfig config;
EGLint num_config;
EGLint count = 0;
EGLConfig *configs;
int config_index;
int i;

static struct gbm_bo *previous_bo = NULL;
static uint32_t previous_fb;

static EGLint attributes[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 0,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

static const EGLint context_attribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

struct gbm_bo *bo;
uint32_t handle;
uint32_t pitch;
int32_t fb;
uint64_t modifier;

static drmModeConnector *find_connector(drmModeRes *resources)
{

    for (i = 0; i < resources->count_connectors; i++)
    {
        drmModeConnector *connector = drmModeGetConnector(device, resources->connectors[i]);
        if (connector->connection == DRM_MODE_CONNECTED)
        {
            return connector;
        }
        drmModeFreeConnector(connector);
    }

    return NULL; // if no connector found
}

static drmModeEncoder *find_encoder(drmModeRes *resources, drmModeConnector *connector)
{

    if (connector->encoder_id)
    {
        return drmModeGetEncoder(device, connector->encoder_id);
    }
    return NULL; // if no encoder found
}

static void swap_buffers()
{

    eglSwapBuffers(display, egl_surface);
    bo = gbm_surface_lock_front_buffer(gbm_surface);
    handle = gbm_bo_get_handle(bo).u32;
    pitch = gbm_bo_get_stride(bo);
    drmModeAddFB(device, mode_info.hdisplay, mode_info.vdisplay, 24, 32, pitch, handle, &fb);
    drmModeSetCrtc(device, crtc->crtc_id, fb, 0, 0, &connector_id, 1, &mode_info);
    if (previous_bo)
    {
        drmModeRmFB(device, previous_fb);
        gbm_surface_release_buffer(gbm_surface, previous_bo);
    }
    previous_bo = bo;
    previous_fb = fb;
}

static void draw(float progress)
{
    glClearColor(1.0f - progress, progress, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    swap_buffers();
}

static int match_config_to_visual(EGLDisplay egl_display, EGLint visual_id, EGLConfig *configs, int count)
{

    EGLint id;
    for (i = 0; i < count; ++i)
    {
        if (!eglGetConfigAttrib(egl_display, configs[i], EGL_NATIVE_VISUAL_ID, &id))
            continue;
        if (id == visual_id)
            return i;
    }
    return -1;
}

void Render(Model &m)
{
    // First, render a square without any colors ( all vertexes will be black )
    // ===================
    // Make our background grey
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, m.getNumIndices(), GL_UNSIGNED_INT, 0);

    swap_buffers();
}

int main()
{
    device = open("/dev/dri/card1", O_RDWR);
    resources = drmModeGetResources(device);
    connector = find_connector(resources);
    connector_id = connector->connector_id;
    mode_info = connector->modes[0];
    encoder = find_encoder(resources, connector);
    crtc = drmModeGetCrtc(device, encoder->crtc_id);
    drmModeFreeEncoder(encoder);
    drmModeFreeConnector(connector);
    drmModeFreeResources(resources);
    gbm_device = gbm_create_device(device);
    gbm_surface = gbm_surface_create(gbm_device, mode_info.hdisplay, mode_info.vdisplay, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    display = eglGetDisplay(gbm_device);
    eglInitialize(display, NULL, NULL);
    eglBindAPI(EGL_OPENGL_ES_API);
    eglGetConfigs(display, NULL, 0, &count);
    configs = malloc(count * sizeof *configs);
    eglChooseConfig(display, attributes, configs, count, &num_config);
    config_index = match_config_to_visual(display, GBM_FORMAT_XRGB8888, configs, num_config);
    context = eglCreateContext(display, configs[config_index], EGL_NO_CONTEXT, context_attribs);
    egl_surface = eglCreateWindowSurface(display, configs[config_index], gbm_surface, NULL);
    free(configs);
    eglMakeCurrent(display, egl_surface, egl_surface, context);
    printf("%s \n", glGetString(GL_RENDERER));
    printf("%s \n", glGetString(GL_VERSION));

    Shader shader;
    shader.UseProgram();

    glm::mat4 Projection = glm::perspective(glm::radians(160.0f), (float)1920 / (float)1080, 0.1f, 100.0f);

	glm::mat4 View = glm::lookAt(
		glm::vec3(2, 0, 0), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	glm::mat4 mvp = Projection * View;

    GLint uniformLoc = glGetUniformLocation(shader.getshaderID(), "vp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, &mvp[0][0]);

    IcosoSphere s(1.0f, 2);
    Model m = s.buildSphere();
    m.genBufferObjects();

    Render(m);

    //sleep(10);

    drmModeSetCrtc(device, crtc->crtc_id, crtc->buffer_id, crtc->x, crtc->y, &connector_id, 1, &crtc->mode);
    drmModeFreeCrtc(crtc);

    if (previous_bo)
    {
        drmModeRmFB(device, previous_fb);
        gbm_surface_release_buffer(gbm_surface, previous_bo);
    }

    eglDestroySurface(display, egl_surface);
    gbm_surface_destroy(gbm_surface);
    eglDestroyContext(display, context);
    eglTerminate(display);
    //gbm_device_destroy(gbm_device);

    close(device);
    return 0;
}

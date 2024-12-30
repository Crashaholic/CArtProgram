#include <stdbool.h>
#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h> // will uncomment when i do meet a problematic evildoer
#include <glad/glad.h>
#include <stdio.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include "cap_layer.h"
#include "cap_shader.h"
#include "cap_math.h"
#include "cap_logging.h"

#include <stb_image.h>
#include <stb_image_write.h>

// resources:
// https://www.codingwiththomas.com/blog/rendering-an-opengl-framebuffer-into-a-dear-imgui-window
// https://uysalaltas.github.io/2022/01/09/OpenGL_Imgui.html

#define INITIAL_WINDOW_WIDTH 1280
#define INITIAL_WINDOW_HEIGHT 720

SDL_Window* window = NULL;
SDL_GLContext glCxt;

ImVec2 lastCanvasSize = { 0, 0 };
bool canvasWindowSizeChanged = false;

unsigned VBO, VAO, EBO;
unsigned vshader, fshader, sprogram;
unsigned FBO, FBT, RBO;

CAP_Layer canvasMainLayer;

bool testExport = false;

typedef struct
{
    ImVec2 pos;
    float zoom;
} Cap_Camera ;

typedef struct
{
    ImVec2 pos;
    ImVec2 motion;
    int scroll;
    int buttonDown;
} Cap_ForwardedMouse;

//typedef struct
//{
//
//} Cap_FowardedPen;

Cap_Camera capcam;

typedef enum CAP_WINDOW_OPEN_STATUS
{
    WOS_TOOLBAR,
    WOS_BRUSHES,
    WOS_BRUSHSETTINGS,
    WOS_COLORPICKER,
    WOS_CANVAS,
    WOS_LAYERS,
    WOS_PREVIEW,
    WOS_HISTORY,
    WOS_COUNT
} WINDOW_OPEN_STATUS; // shorted to WOS

static bool* windowOpenStatus;

typedef enum CAP_MENU_TRIGGER_ITEM
{
    MTI_EXIT = 1,
    MTI_NEW_IMAGE,
    MTI_OPEN_IMAGE,
    MTI_EXPORT_CANVAS,
    MTI_UNDO,
    MTI_REDO,
    MTI_ABOUT_PROGRAM,
    MTI_COUNT,

} MENU_TRIGGER_ITEM;

static bool* menuTrigger;

void Exit();

void ShowToolbarWindow(bool* p_open)
{
    //https://github.com/ocornut/imgui/issues/2648
    if (igBegin("Toolbar##WindowToolbar", p_open, 0))
    {
        igText("IM A TOOLBAR!!!");
    }
    igEnd();
}

void ShowBrushWindow(bool* p_open)
{ 
    if (igBegin("Brushes##WindowBrushes", p_open, 0))
    {
        igText("IM BRUSHES!!!");
    }
    igEnd();
}

void ShowBrushSettingsWindow(bool* p_open)
{
    if (igBegin("Brush Settings##WindowBrushSettings", p_open, 0))
    {
        igText("IM BRUSH SETTINGS!!!");
    }
    igEnd();
}

void ShowColorPickerWindow(bool* p_open)
{
    if (igBegin("Color Picker##WindowColorPicker", p_open, 0))
    {
        igText("IM COLOR PICKER!!!");
    }
    igEnd();
}

void ShowLayersWindow(bool* p_open)
{
    if (igBegin("Layers##WindowLayers", p_open, 0))
    {
        igText("IM LAYERS!!!");
    }
    igEnd();
}

void ShowPreviewWindow(bool* p_open)
{
    if (igBegin("Preview##WindowPreview", p_open, 0))
    {
        igText("IM PREVIEW!!!");
    }
    igEnd();
}

void ShowHistoryWindow(bool* p_open)
{
    if (igBegin("History##WindowHistory", p_open, 0))
    {
        igText("IM HISTORY!!!");
    }
    igEnd();
}

void ShowCanvasWindow(bool* p_open)
{
    ImGuiIO* io = igGetIO(); // Access ImGui's IO system for mouse position
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){ 0.0f, 0.0f });
    if (igBegin("Canvas##WindowCanvas", p_open, 0))
    {
        ImVec2 getCursorScreenPos;
        igGetCursorScreenPos(&getCursorScreenPos);

        ImVec2 boundsMin = { 0.0f, 0.0f };
        ImVec2 boundsMax = { 0.0f, 0.0f };

        ImVec2 regionAvail; igGetContentRegionAvail(&regionAvail);

        boundsMin.x = getCursorScreenPos.x;
        boundsMin.y = getCursorScreenPos.y;
        boundsMax.x = getCursorScreenPos.x + regionAvail.x;
        boundsMax.y = getCursorScreenPos.y + regionAvail.y;

        ImVec2 mp = io->MousePos;

        SDL_Cursor* DRAG_CURSOR = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
        SDL_Cursor* DFLT_CURSOR = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
        if (mp.x > boundsMin.x && mp.x < boundsMax.x)
        {
            if (mp.y > boundsMin.y && mp.y < boundsMax.y)
            {
                if (io->MouseDown[2])
                {
                    SDL_SetCursor(DRAG_CURSOR);
                    capcam.pos.x += io->MouseDelta.x;
                    capcam.pos.y -= io->MouseDelta.y;
                }
                else
                {
                    SDL_SetCursor(DFLT_CURSOR);
                }

                if (io->MouseWheel > 0.f && io->MouseWheel > FLT_EPSILON)
                {
                    if (io->KeyCtrl)
                    {
                        if (capcam.zoom + 0.5f > 20.0f)
                            capcam.zoom = 20.0f;
                        else
                            capcam.zoom += 0.5f;
                    }
                }
                else if (io->MouseWheel < 0.f && io->MouseWheel < FLT_EPSILON)
                {
                    if (io->KeyCtrl)
                    {
                        if ((capcam.zoom - 0.5f) > 0.0f)
                        {
                            capcam.zoom -= 0.5f;
                        }
                        else
                        {
                            capcam.zoom = 0.1f;
                        }
                    }
                }
            }
            else
            {
                SDL_SetCursor(DFLT_CURSOR);
            }
        }
        else
        {
            SDL_SetCursor(DFLT_CURSOR);
        }

        if (lastCanvasSize.x != regionAvail.x || lastCanvasSize.y != regionAvail.y)
        {
            canvasWindowSizeChanged = true;
            lastCanvasSize.x = regionAvail.x;
            lastCanvasSize.y = regionAvail.y;
        }

        ImDrawList_AddImage(igGetWindowDrawList(), FBT, boundsMin, boundsMax, (ImVec2) { 0, 1 }, (ImVec2) { 1, 0 },(255 << 24) | (255 << 16) | (255 << 8) | (255));
    }
    igEnd();
    igPopStyleVar(1);
}

// Initialize all systems
int Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        print_err("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }
    // Set OpenGL version and profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("OpenGL + SDL3",
        INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        print_err("Failed to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    glCxt = SDL_GL_CreateContext(window);
    if (!glCxt)
    {
        printf("Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        SDL_GL_DestroyContext(glCxt);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    //printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    // setup imgui
    igCreateContext(NULL);
    // Initialize ImGui context
    ImGuiIO* io = igGetIO();
    io->DisplaySize.x = INITIAL_WINDOW_WIDTH; // Set these dynamically based on window size
    io->DisplaySize.y = INITIAL_WINDOW_HEIGHT;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    ImGuiStyle* style = igGetStyle();
    igStyleColorsDark(style);
    style->WindowMenuButtonPosition = ImGuiDir_None;
    ImGui_ImplSDL3_InitForOpenGL(window, &glCxt);
    ImGui_ImplOpenGL3_Init(glsl_version);

    windowOpenStatus = calloc(WOS_COUNT, sizeof(bool));
    if (windowOpenStatus)
    {
        int i = 0;
        for (i = 0; i < WOS_COUNT; ++i)
        {
            windowOpenStatus[i] = false;
        }
        windowOpenStatus[WOS_TOOLBAR] = true;
        windowOpenStatus[WOS_BRUSHES] = true;
        windowOpenStatus[WOS_BRUSHSETTINGS] = true;
        windowOpenStatus[WOS_COLORPICKER] = true;
        windowOpenStatus[WOS_CANVAS] = true;
        windowOpenStatus[WOS_LAYERS] = true;
        windowOpenStatus[WOS_PREVIEW] = true;
        windowOpenStatus[WOS_HISTORY] = true;
    }
    else
    {
        printf("Could not allocate windowOpenStatus[%i]", WOS_COUNT);
        SDL_GL_DestroyContext(glCxt);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    menuTrigger = calloc(MTI_COUNT, sizeof(bool));
    if (menuTrigger)
    {
        int i = 0;
        for (i = 1; i < MTI_COUNT; ++i)
        {
            menuTrigger[i] = false;
        }
    }
    else
    {
        printf("Could not allocate menuTrigger[%i]", MTI_COUNT);
        SDL_GL_DestroyContext(glCxt);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    char* s = Cap_ShaderGetContentFromFile("VERTEX_SHADER.vert");
    if (!s)
    {
        printf("Could not open VERTEX shader file!\n");
        Exit();
        return -1;
    }

    vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &s, NULL);
    glCompileShader(vshader);
    {
        int  success;
        char infoLog[512] = {0};
        glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vshader, 512, NULL, infoLog);
            printf("Could not compile VERTEX shader: \n%s\n", infoLog);
        }
    }

    s = Cap_ShaderGetContentFromFile("FRAGMENT_SHADER.frag");
    if (!s)
    {
        printf("Could not open FRAGMENT shader file!\n");
        Exit();
        return -1;
    }

    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &s, NULL);
    glCompileShader(fshader);
    {
        int  success;
        char infoLog[512] = { 0 };
        glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fshader, 512, NULL, infoLog);
            printf("Could not compile FRAGMENT shader: \n%s\n", infoLog);
        }
    }

    sprogram = glCreateProgram();
    glAttachShader(sprogram, vshader);
    glAttachShader(sprogram, fshader);
    glLinkProgram(sprogram);
    {
        int  success;
        char infoLog[512] = { 0 };
        glGetProgramiv(sprogram, GL_LINK_STATUS, &success);
        if (!success) 
        {
            glGetProgramInfoLog(sprogram, 512, NULL, infoLog);
            printf("Could not LINK SHADER: \n%s\n", infoLog);
        }
    }
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    };

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glGenFramebuffers(1, &FBO);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenTextures(1, &FBT);
    glBindTexture(GL_TEXTURE_2D, FBT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBT, 0);

    capcam.pos = (ImVec2){0.0f,0.0f};
    capcam.zoom = 1.0f;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        // it works!
        print_wng("[INIT] Framebuffer complete.\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glmc_translate(model, (vec3) { 0.0f, 0.0f, 0.0f });

    mat4 view = GLM_MAT4_IDENTITY_INIT;

    mat4 proj = GLM_MAT4_IDENTITY_INIT;
    glmc_ortho(-(INITIAL_WINDOW_WIDTH / 2.0f), INITIAL_WINDOW_WIDTH / 2.0f, -(INITIAL_WINDOW_HEIGHT / 2.0f), (INITIAL_WINDOW_HEIGHT / 2.0f), -0.1f, 100.f, proj);

    glUseProgram(sprogram);

    glUniformMatrix4fv(glGetUniformLocation(sprogram, "model"), 1, GL_FALSE, model[0]);
    glUniformMatrix4fv(glGetUniformLocation(sprogram, "view"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(sprogram, "proj"), 1, GL_FALSE, proj[0]);

    glUseProgram(0);

    canvasMainLayer = Cap_CreateLayer(200, 100);
    unsigned temp1 = canvasMainLayer.width * canvasMainLayer.height;
    unsigned int temp;
    for (temp = 0; temp < temp1; temp++)
    {
        canvasMainLayer.data[temp].r = 0.00f + 0.0001f * temp;
        canvasMainLayer.data[temp].g = 0.00f;
        canvasMainLayer.data[temp].b = 0.05f + 0.005f * (temp % canvasMainLayer.width);
        canvasMainLayer.data[temp].a = 1.00f;
    }

    glGenTextures(1, &canvasMainLayer.textureId);
    glBindTexture(GL_TEXTURE_2D, canvasMainLayer.textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvasMainLayer.width, canvasMainLayer.height, 0, GL_RGBA, GL_FLOAT, (void*)canvasMainLayer.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return 0;
}

void Run()
{
    ImGuiIO* io = igGetIO();
    int running = 1;
    SDL_Event ev;
    ImGuiWindowFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags defaultWindowFlags = 0;

    float pressure = 0.0f;
    float previous_touch_x = -1.0f;
    float previous_touch_y = -1.0f;

    /*
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );

    if ( result == NFD_OKAY ) {
        puts("Success!");
        puts(outPath);
        free(outPath);
    }
    else if ( result == NFD_CANCEL ) {
        puts("User pressed cancel.");
    }
    else {
        printf("Error: %s\n", NFD_GetError() );
    }
    */

    while (running)
    {
        while (SDL_PollEvent(&ev))
        {
            ImGui_ImplSDL3_ProcessEvent(&ev);
            if (ev.type == SDL_EVENT_QUIT)
            {
                running = 0;
            }
            else if (ev.window.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && ev.window.windowID == SDL_GetWindowID(window) || menuTrigger[MTI_EXIT])
            {
                // check if unsaved
                //     show modal
                // else
                running = 0;
            }
            //else if (ev.type == SDL_EVENT_PEN_MOTION)
            //{
            //    if (pressure > 0.0f) 
            //    {
            //        if (previous_touch_x >= 0.0f) 
            //        {  /* only draw if we're moving while touching */
            //            /* draw with the alpha set to the pressure, so you effectively get a fainter line for lighter presses. */
            //            //SDL_SetRenderTarget(renderer, render_target);
            //            //SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, pressure);
            //            //SDL_RenderLine(renderer, previous_touch_x, previous_touch_y, event->pmotion.x, event->pmotion.y);
            //            printf("HES DRAWING!!!!");
            //        }
            //        previous_touch_x = ev.pmotion.x;
            //        previous_touch_y = ev.pmotion.y;
            //    }
            //    else {
            //        previous_touch_x = previous_touch_y = -1.0f;
            //    }

            //}
            //else if (ev.type == SDL_EVENT_PEN_AXIS)
            //{
            //    if (ev.paxis.axis == SDL_PEN_AXIS_PRESSURE) 
            //    {
            //        pressure = ev.paxis.value;  /* remember new pressure for later draws. */
            //    }
            //}
        }

        if (canvasWindowSizeChanged)
        {
            glBindTexture(GL_TEXTURE_2D, FBT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)lastCanvasSize.x, (int)lastCanvasSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBT, 0);

            glUseProgram(sprogram);
            mat4 proj = GLM_MAT4_IDENTITY_INIT;
            glmc_ortho(-(lastCanvasSize.x / 2.0f), lastCanvasSize.x / 2.0f, -(lastCanvasSize.y / 2.0f), (lastCanvasSize.y / 2.0f), -0.1f, 100.f, proj);
            glUniformMatrix4fv(glGetUniformLocation(sprogram, "proj"), 1, GL_FALSE, proj[0]);
            glUseProgram(0);
            canvasWindowSizeChanged = false;
        }

        if (menuTrigger[MTI_EXPORT_CANVAS])
        {
            printf("EXPORTING!!!!\n");
            nfdchar_t* location = NULL;
            nfdresult_t r = NFD_SaveDialog("png,jpg;psd", NULL, &location);
            if (r == NFD_OKAY)
            {
                Cap_WriteCanvasToFile(location, canvasMainLayer.data, canvasMainLayer.width, canvasMainLayer.height);
                puts(location);
                free(location);
            }
            else if (r == NFD_CANCEL)
            {
                printf("user cancelled.\n");
            }
            else 
            {
                printf("Error: %s\n", NFD_GetError());
            }
            menuTrigger[MTI_EXPORT_CANVAS] = false;
        }
        if (menuTrigger[MTI_OPEN_IMAGE])
        {
            printf("IMPORTING!!!!\n");
            nfdchar_t* location = NULL;
            nfdresult_t r = NFD_OpenDialog("png,jpg;psd", NULL, &location);
            if (r == NFD_OKAY)
            {
                int w, h, channels;
                unsigned char* imageData = stbi_load(location, &w, &h, &channels, 4); // Force 4 channels (RGBA)
                if (!imageData) 
                {
                    fprintf(stderr, "Failed to load image: %s\n", location);
                }
                CAP_PixelRGBA* result = (CAP_PixelRGBA*)malloc(w * h * sizeof(CAP_PixelRGBA));
                if (!result) 
                {
                    fprintf(stderr, "Failed to allocate memory for pixel array\n");
                    stbi_image_free(imageData);
                }
                for (int i = 0; i < w * h; ++i) 
                {
                    result[i].r = imageData[i * 4 + 0] / 255.0f; // Red
                    result[i].g = imageData[i * 4 + 1] / 255.0f; // Green
                    result[i].b = imageData[i * 4 + 2] / 255.0f; // Blue
                    result[i].a = imageData[i * 4 + 3] / 255.0f; // Alpha
                }
                // Set the width and height for the caller
                //*width = (unsigned)w;
                //*height = (unsigned)h;

                Cap_ReplaceLayer(&canvasMainLayer, w, h);
                free(canvasMainLayer.data);
                canvasMainLayer.data = result;

                // Free the raw image data from stb_image
                stbi_image_free(imageData);
            }
            else if (r == NFD_CANCEL)
            {
                printf("user cancelled.\n");
            }
            else
            {
                printf("Error: %s\n", NFD_GetError());
            }
            menuTrigger[MTI_OPEN_IMAGE] = false;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        igNewFrame();

        const ImGuiViewport* viewport = igGetMainViewport();

        // Set the parent window's position, size, and viewport to match that of the main viewport. This is so the parent window
        // completely covers the main viewport, giving it a "full-screen" feel.
        igSetNextWindowPos(viewport->WorkPos, 0, (ImVec2) { 0, 0 });
        igSetNextWindowSize(viewport->WorkSize, 0);
        igSetNextWindowViewport(viewport->ID);

        // Set the parent window's styles to match that of the main viewport:
        igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0f); // No corner rounding on the window
        igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f); // No border around the window

        // Manipulate the window flags to make it inaccessible to the user (no titlebar, resize/move, or navigation)
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) {0, 0});
        igBegin("DockSpace Demo##DockspaceParentWindow", 0, windowFlags);
        {
            igPopStyleVar(3);
            if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                // If it is, draw the Dockspace with the DockSpace() function.
                // The GetID() function is to give a unique identifier to the Dockspace - here, it's "MyDockSpace".
                ImGuiID dockspace_id = igGetID_Str("MyDockSpace");
                igDockSpace(dockspace_id, (ImVec2) { 0, 0 }, dockspaceFlags, 0);
            }
            if (igBeginMenuBar())
            {
                if (igBeginMenu("File", 1))
                {
                    igMenuItem_BoolPtr("New", "Ctrl + N", NULL, 1);
                    igMenuItem_BoolPtr("Open", "Ctrl + O", &menuTrigger[MTI_OPEN_IMAGE], 1);
                    igMenuItem_BoolPtr("Export Image", NULL, &menuTrigger[MTI_EXPORT_CANVAS], 1);
                    igMenuItem_BoolPtr("Exit", NULL, &menuTrigger[MTI_EXIT], 1);
                    igEndMenu();
                }
                if (igBeginMenu("Edit", 1))
                {
                    igMenuItem_BoolPtr("Undo", "Ctrl + Z", NULL, 1);
                    igMenuItem_BoolPtr("Redo", "Ctrl + Y", NULL, 1);
                    igEndMenu();
                }
                if (igBeginMenu("Image", 1))
                {
                    igMenuItem_BoolPtr("Rotate 90 Degrees", NULL, NULL, 1);
                    igEndMenu();
                }
                if (igBeginMenu("Layer", 1))
                {
                    igMenuItem_BoolPtr("Rotate 90 Degrees", NULL, NULL, 1);
                    igEndMenu();
                }
                if (igBeginMenu("Window", 1))
                {
                    igMenuItem_BoolPtr("Canvas", NULL, &windowOpenStatus[WOS_CANVAS], 1);
                    igMenuItem_BoolPtr("Toolbar", NULL, &windowOpenStatus[WOS_TOOLBAR], 1);
                    igMenuItem_BoolPtr("Brushes", NULL, &windowOpenStatus[WOS_BRUSHES], 1);
                    igMenuItem_BoolPtr("Brush Settings", NULL, &windowOpenStatus[WOS_BRUSHSETTINGS], 1);
                    igMenuItem_BoolPtr("Color Picker", NULL, &windowOpenStatus[WOS_COLORPICKER], 1);
                    igMenuItem_BoolPtr("Layers", NULL, &windowOpenStatus[WOS_LAYERS], 1);
                    igMenuItem_BoolPtr("Preview", NULL, &windowOpenStatus[WOS_PREVIEW], 1);
                    igMenuItem_BoolPtr("History", NULL, &windowOpenStatus[WOS_HISTORY], 1);
                    igEndMenu();
                }
                if (igBeginMenu("Help", 1))
                {
                    igMenuItem_BoolPtr("About...", NULL, &windowOpenStatus[WOS_TOOLBAR], 1);
                    igEndMenu();
                }
                igEndMenuBar();
            }
        }
        igEnd();
        
        if (windowOpenStatus[WOS_TOOLBAR])
        {
            ShowToolbarWindow(&windowOpenStatus[WOS_TOOLBAR]);
        }

        if (windowOpenStatus[WOS_BRUSHES])
        {
            ShowBrushWindow(&windowOpenStatus[WOS_BRUSHES]);
        }

        if (windowOpenStatus[WOS_BRUSHSETTINGS])
        {
            ShowBrushSettingsWindow(&windowOpenStatus[WOS_BRUSHSETTINGS]);
        }

        if (windowOpenStatus[WOS_COLORPICKER])
        {
            ShowColorPickerWindow(&windowOpenStatus[WOS_COLORPICKER]);
        }

        if (windowOpenStatus[WOS_CANVAS])
        {
            ShowCanvasWindow(&windowOpenStatus[WOS_CANVAS]);
        }

        if (windowOpenStatus[WOS_LAYERS])
        {
            ShowLayersWindow(&windowOpenStatus[WOS_LAYERS]);
        }

        if (windowOpenStatus[WOS_PREVIEW])
        {
            ShowPreviewWindow(&windowOpenStatus[WOS_PREVIEW]);
        }

        if (windowOpenStatus[WOS_HISTORY])
        {
            ShowHistoryWindow(&windowOpenStatus[WOS_HISTORY]);
        }

        // render
        igRender();
        SDL_GL_MakeCurrent(window, glCxt);
        glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
        if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            igUpdatePlatformWindows();
            igRenderPlatformWindowsDefault(NULL, NULL);
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        glViewport(0, 0, (int)lastCanvasSize.x, (int)lastCanvasSize.y);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glClearColor(0.11f, 0.123f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(sprogram);
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glmc_scale(model, (vec3) { canvasMainLayer.width, canvasMainLayer.height, 1.0f });
        glUniformMatrix4fv(glGetUniformLocation(sprogram, "model"), 1, GL_FALSE, model[0]);
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        glmc_translate(view, (vec3) { capcam.pos.x, capcam.pos.y, 0.0f });
        glmc_scale(view, (vec3) { 1.0f * capcam.zoom, 1.0f * capcam.zoom, 1.0f });
        glUniformMatrix4fv(glGetUniformLocation(sprogram, "view"), 1, GL_FALSE, view[0]);
        glBindTexture(GL_TEXTURE_2D, canvasMainLayer.textureId);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        SDL_GL_SwapWindow(window);
    }
}

// Clean up the program's memory usage
void Exit()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(sprogram);
    glDeleteFramebuffers(1, &FBO);
    // clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    igDestroyContext(NULL);

    SDL_GL_DestroyContext(glCxt);
    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();
    free(windowOpenStatus);
}

int main(int argc, char** args)
{
    int status = Init();
    if (status != 0) 
        return status;
    Run();
    Exit();
    return 0;
}

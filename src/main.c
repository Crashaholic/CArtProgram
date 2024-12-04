#include <stdbool.h>
#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h> // will uncomment when i do meet a problematic evildoer
#include <glad/glad.h>
#include <stdio.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>
#include <stdint.h>

#include "cap_layer.h"

// resources:
// https://www.codingwiththomas.com/blog/rendering-an-opengl-framebuffer-into-a-dear-imgui-window
// https://uysalaltas.github.io/2022/01/09/OpenGL_Imgui.html

#define INITIAL_WINDOW_WIDTH 1280
#define INITIAL_WINDOW_HEIGHT 720

SDL_Window* window = NULL;
SDL_GLContext glCxt;

const char* vertex_shader_code = "#version 330\nlayout (location = 0) in vec3 pos;\n\nvoid main()\n{\n\tgl_Position = vec4(0.9*pos.x, 0.9*pos.y, 0.5*pos.z, 1.0);\n}";
const char* fragment_shader_code = "#version 330\n\nout vec4 color;\n\nvoid main()\n{\n\tcolor = vec4(0.0, 1.0, 0.0, 1.0);\n}\n";

typedef enum
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
        unsigned int color = (255 << 24) | (255 << 16) | (255); // Magenta

        ImVec2 regionAvail; igGetContentRegionAvail(&regionAvail);

        boundsMin.x = getCursorScreenPos.x;
        boundsMin.y = getCursorScreenPos.y;
        boundsMax.x = getCursorScreenPos.x + regionAvail.x;
        boundsMax.y = getCursorScreenPos.y + regionAvail.y;

        ImVec2 mp = io->MousePos;
#if 0
        igText("Mouse: [%.2f, %.2f]", mp.x, mp.y);
        igText("getCursorScreenPos: [%.2f, %.2f]", getCursorScreenPos.x, getCursorScreenPos.y);
        igText("regionAvail: [%.2f, %.2f]", regionAvail.x, regionAvail.y);
        igText("Bounds Min: [%.2f, %.2f]", boundsMin.x, boundsMin.y);
        igText("Bounds Max: [%.2f, %.2f]", boundsMax.x, boundsMax.y);
#endif
        if (mp.x > boundsMin.x && mp.x < boundsMax.x)
        {
            if (mp.y > boundsMin.y && mp.y < boundsMax.y)
            {
#if 0
                ImVec2 tempRectMin = { mp.x - boundsMin.x - 5.0f, mp.y - boundsMin.y - 5.0f };
                ImVec2 tempRectMax = { mp.x - boundsMin.x + 5.0f, mp.y - boundsMin.y + 5.0f };
                ImDrawList_AddRect(igGetWindowDrawList(), tempRectMin, tempRectMax, color, 0.0f, 0, 1.0f);
#endif
                
            }
        }
    }
    igEnd();
    igPopStyleVar(1);
}

// Initialize all systems
int Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
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
        printf("Failed to create SDL window: %s\n", SDL_GetError());
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

    windowOpenStatus = malloc(sizeof(bool) * WOS_COUNT);
    if (windowOpenStatus)
    {
        int i = 0;
        for (i = 0; i < WOS_COUNT; i++)
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
        return -1;
    }
    return 0;
}

void Run()
{
    ImGuiIO* io = igGetIO();
    int running = 1;
    SDL_Event ev;
    ImGuiWindowFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags defaultWindowFlags = 0;

    bool wantToQuit = false;

    while (running)
    {
        while (SDL_PollEvent(&ev))
        {
            ImGui_ImplSDL3_ProcessEvent(&ev);
            if (ev.type == SDL_EVENT_QUIT)
            {
                running = 0;
            }
            if (ev.window.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && ev.window.windowID == SDL_GetWindowID(window) || wantToQuit)
            {
                // check if unsaved
                //     show modal
                // else
                running = 0;
            }
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
                    igMenuItem_BoolPtr("Exit", NULL, &wantToQuit, 1);
                    igEndMenu();
                }
                if (igBeginMenu("Edit", 1))
                {
                    igEndMenu();
                }
                if (igBeginMenu("Image", 1))
                {
                    igEndMenu();
                }
                if (igBeginMenu("Layer", 1))
                {
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
        SDL_GL_SwapWindow(window);
    }
}

// Clean up the program's memory usage
void Exit()
{
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

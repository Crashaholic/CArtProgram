#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "cap_progwindows.h"
#include "cap_math.h"
#include "cap_logging.h"
//#include <SDL3/SDL_main.h> // will uncomment when i do meet a problematic evildoer

// resources:
// https://www.codingwiththomas.com/blog/rendering-an-opengl-framebuffer-into-a-dear-imgui-window
// https://uysalaltas.github.io/2022/01/09/OpenGL_Imgui.html

#define INITIAL_WINDOW_WIDTH 1280
#define INITIAL_WINDOW_HEIGHT 720

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
    WOS_STATUS,
    WOS_COUNT
} WINDOW_OPEN_STATUS; // shorted to WOS

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

void Exit();
void SetupQuad();
void SetupFramebuffer();
void RenderCanvasContents();
void DisplayWindowByStatus();

SDL_Window* window = NULL;
SDL_GLContext glCxt;

ImVec2 lastCanvasSize = { 0, 0 };
ImVec2 tempDebug;

unsigned VBO, VAO, EBO;
unsigned vshader, fshader, sprogram;
unsigned FBO, FBT, RBO;

unsigned checkerboardPattern;

CAP_Layer canvasMainLayer;

bool testExport = false;

bool* windowOpenStatus;
bool* menuTrigger;
Cap_Camera capcam;

// Initialize all systems
int Init()
{
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
        windowOpenStatus[WOS_STATUS] = true;
    }
    else
    {
        printf("Could not allocate windowOpenStatus[%i]", WOS_COUNT);
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
        return -1;
    }

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
    io->DisplaySize.x = INITIAL_WINDOW_WIDTH; 
    io->DisplaySize.y = INITIAL_WINDOW_HEIGHT;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGuiStyle* style = igGetStyle();
    igStyleColorsDark(style);
    // probably have a style file or smth
    style->WindowMenuButtonPosition = ImGuiDir_None;
    ImGui_ImplSDL3_InitForOpenGL(window, &glCxt);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (Cap_ShaderSetup("res/VERTEX_SHADER.vert", "res/FRAGMENT_SHADER.frag", &vshader, &fshader, &sprogram))
    {
        Exit();
        return -1;
    }

    SetupQuad();
    SetupFramebuffer();

    capcam.pos = (ImVec2){ 0.0f,0.0f };
    capcam.zoom = 1.0f;
    
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    mat4 proj = GLM_MAT4_IDENTITY_INIT;
    glmc_ortho(-(INITIAL_WINDOW_WIDTH / 2.0f), INITIAL_WINDOW_WIDTH / 2.0f, -(INITIAL_WINDOW_HEIGHT / 2.0f), (INITIAL_WINDOW_HEIGHT / 2.0f), -0.1f, 100.f, proj);

    glUseProgram(sprogram);
    Cap_ShaderSetMat4(sprogram, "model", model[0]);
    Cap_ShaderSetMat4(sprogram, "view", view[0]);
    Cap_ShaderSetMat4(sprogram, "proj", proj[0]);
    glUseProgram(0);

    canvasMainLayer = Cap_LayerCreate(11, 11);
    unsigned temp1 = canvasMainLayer.width * canvasMainLayer.height;
    unsigned int temp;
    for (temp = 0; temp < temp1; temp++)
    {
        canvasMainLayer.data[temp].r = 1.f * ((temp % 2));
        canvasMainLayer.data[temp].g = 1.f * ((temp % 2));
        canvasMainLayer.data[temp].b = 1.f * ((temp % 2));
        canvasMainLayer.data[temp].a = 1.f * ((temp % 2));
    }
    Cap_LayerRefreshImage(&canvasMainLayer);

    return 0;
}

void Run()
{
    ImGuiIO* io = igGetIO();
    int running = 1;
    SDL_Event ev;
    ImGuiWindowFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags defaultWindowFlags = 0;

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
        }

        if (menuTrigger[MTI_EXPORT_CANVAS])
        {
            Cap_FileIOExportFromLayer(&canvasMainLayer);
            menuTrigger[MTI_EXPORT_CANVAS] = false;
        }
        if (menuTrigger[MTI_OPEN_IMAGE])
        {
            // int r = Cap_FileIOOpenImageToLayer(Layer);
            // if (!r) Cap_LayerRefreshImage(&Layer);
            Cap_FileIOImportToLayer(&canvasMainLayer);
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
                    igMenuItem_BoolPtr("Status", NULL, &windowOpenStatus[WOS_STATUS], 1);
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
        
        DisplayWindowByStatus();

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

        RenderCanvasContents();

        SDL_GL_SwapWindow(window);
    }
}

void SetupQuad()
{
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
}

void SetupFramebuffer()
{
    glGenTextures(1, &FBT);
    glBindTexture(GL_TEXTURE_2D, FBT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBT, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        // it works!
        print_wng("[INIT] Framebuffer complete.\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DisplayWindowByStatus()
{
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
        ShowCanvasWindow(&windowOpenStatus[WOS_CANVAS]
            , sprogram
            , &canvasMainLayer
            , FBT
            , &lastCanvasSize
            , &capcam
            , &tempDebug);
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

    if (windowOpenStatus[WOS_STATUS])
    {
        ShowStatusWindow(&windowOpenStatus[WOS_STATUS]);
    }
}

void RenderCanvasContents()
{
    glViewport(0, 0, (int)lastCanvasSize.x, (int)lastCanvasSize.y);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.11f, 0.123f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(sprogram);
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glmc_scale(model, (vec3) { (float)canvasMainLayer.width, (float)canvasMainLayer.height, 1.0f });
    Cap_ShaderSetMat4(sprogram, "model", model[0]);
    glBindTexture(GL_TEXTURE_2D, canvasMainLayer.textureId);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

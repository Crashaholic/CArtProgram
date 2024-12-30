#include "cap_progwindows.h"

#include <float.h>

#include <glad/glad.h>
#include <SDL3/SDL.h>

void ToolbarWindowSizeCallback(ImGuiSizeCallbackData* data)
{

}

void ShowToolbarWindow(bool* p_open)
{
    ImGuiSizeCallbackData* data = NULL;
    igSetNextWindowSize((ImVec2) {50.f, 1080.f}, ImGuiCond_Always);
    igSetNextWindowSizeConstraints((ImVec2) { -1, 0.0f }, (ImVec2){ -1, FLT_MAX }, NULL, NULL);
    //https://github.com/ocornut/imgui/issues/2648
    if (igBegin("Toolbar##WindowToolbar", p_open, ImGuiWindowFlags_NoTitleBar))
    {
        //igText("IM A TOOLBAR!!!");
        igButton("icn1", (ImVec2) { 30, 30 });
        igButton("icn2", (ImVec2) { 30, 30 });
        igButton("icn3", (ImVec2) { 30, 30 });
        igButton("icn4", (ImVec2) { 30, 30 });
        igButton("icn5", (ImVec2) { 30, 30 });
        igButton("icn6", (ImVec2) { 30, 30 });
        igButton("icn7", (ImVec2) { 30, 30 });
        igButton("icn8", (ImVec2) { 30, 30 });
        igButton("icn9", (ImVec2) { 30, 30 });
    }
    igEnd();
}

void ShowBrushWindow(bool* p_open)
{
    if (igBegin("Brushes##WindowBrushes", p_open, 0))
    {
        if (igBeginTable("brushes_table", 5, ImGuiTableFlags_SizingStretchSame, (ImVec2) { 0.0f, 0.0f }, 0.0f))
        {
            igTableNextColumn();
            igButton("icn1", (ImVec2) { 30, 30 });
            igTableNextColumn();
            igButton("icn2", (ImVec2) { 30, 30 });
            igTableNextColumn();
            igButton("icn3", (ImVec2) { 30, 30 });
            igTableNextColumn();
            igButton("icn4", (ImVec2) { 30, 30 });
            igTableNextColumn();
            igButton("icn5", (ImVec2) { 30, 30 });
            igTableNextColumn();
            igButton("icn6", (ImVec2) { 30, 30 });
            igTableNextColumn();
            igButton("icn7", (ImVec2) { 30, 30 });
            igEndTable();
        }
    }
    igEnd();
}

void ShowBrushSettingsWindow(bool* p_open)
{
    if (igBegin("Brush Settings##WindowBrushSettings", p_open, 0))
    {
        //igText("IM BRUSH SETTINGS!!!");
    }
    igEnd();
}

void ShowColorPickerWindow(bool* p_open)
{
    if (igBegin("Color Picker##WindowColorPicker", p_open, 0))
    {
        //igText("IM COLOR PICKER!!!");
    }
    igEnd();
}

void ShowLayersWindow(bool* p_open)
{
    if (igBegin("Layers##WindowLayers", p_open, 0))
    {
        //igText("IM LAYERS!!!");
    }
    igEnd();
}

void ShowPreviewWindow(bool* p_open)
{
    if (igBegin("Preview##WindowPreview", p_open, 0))
    {
        //igText("IM PREVIEW!!!");
    }
    igEnd();
}

void ShowHistoryWindow(bool* p_open)
{
    if (igBegin("History##WindowHistory", p_open, 0))
    {
        //igText("IM HISTORY!!!");
    }
    igEnd();
}

void ShowCanvasWindow(bool* p_open, bool* canvasWindowSizeChanged, unsigned FBT, ImVec2* lastCanvasSize, Cap_Camera* capcam)
{
    ImGuiIO* io = igGetIO(); // Access ImGui's IO system for mouse position
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) { 0.0f, 0.0f });
    if (igBegin("Canvas##WindowCanvas", p_open, 0))
    {
        ImVec2 getCursorScreenPos;
        igGetCursorScreenPos(&getCursorScreenPos);

        ImVec2 boundsMin = { 0.0f, 0.0f };
        ImVec2 boundsMax = { 0.0f, 0.0f };
        unsigned int color = (255 << 24) | (255 << 16) | (255); // Magenta

        ImVec2 regionAvail; 
        igGetContentRegionAvail(&regionAvail);

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
                    capcam->pos.x += io->MouseDelta.x;
                    capcam->pos.y -= io->MouseDelta.y;
                }
                else
                {
                    SDL_SetCursor(DFLT_CURSOR);
                }

                // TODO: RELATIVE ZOOMING
                if (io->MouseWheel > 0.f && io->MouseWheel > FLT_EPSILON)
                {
                    if (io->KeyCtrl)
                    {
                        if (capcam->zoom + 0.5f > 20.0f)
                            capcam->zoom = 20.0f;
                        else
                            capcam->zoom += 0.5f;
                    }
                }
                else if (io->MouseWheel < 0.f && io->MouseWheel < FLT_EPSILON)
                {
                    if (io->KeyCtrl)
                    {
                        if ((capcam->zoom - 0.5f) > 0.0f)
                        {
                            capcam->zoom -= 0.5f;
                        }
                        else
                        {
                            capcam->zoom = 0.1f;
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

        if (lastCanvasSize->x != regionAvail.x || lastCanvasSize->y != regionAvail.y)
        {
            *canvasWindowSizeChanged = true;
            lastCanvasSize->x = regionAvail.x;
            lastCanvasSize->y = regionAvail.y;
        }

        //ImDrawList_AddRectFilled(igGetWindowDrawList(), boundsMin, boundsMax, 0xFFFFFFFF, 0.0f, 0);
        ImDrawList_AddImage(igGetWindowDrawList(), FBT, boundsMin, boundsMax, (ImVec2) { 0, 1 }, (ImVec2) { 1, 0 }, 0xFFFFFFFF);
    }
    igEnd();
    igPopStyleVar(1);
}



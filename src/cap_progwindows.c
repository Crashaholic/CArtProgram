#include "cap_progwindows.h"

#include <float.h>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <cglm/call.h>

ImVec2 prevMousePos = { 0.0f, 0.0f };
ImVec2 cursorCoords = { 0.0f, 0.0f};
float currentColorPri[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float currentColorSec[4] = {1.0f, 1.0f, 1.0f, 1.0f};

void ShowToolbarWindow(bool* p_open)
{
    ImGuiSizeCallbackData* data = NULL;
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
        igColorPicker4("Main Color", currentColorPri, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview, NULL);
    }
    igEnd();
}

void ShowLayersWindow(bool* p_open)
{
    if (igBegin("Layers##WindowLayers", p_open, 0))
    {
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

void ShowStatusWindow(bool* p_open)
{
    if (igBegin("Status##WindowStatus", p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
    {
        igText("%.0f, %.0f\n", cursorCoords.x, cursorCoords.y);
    }
    igEnd();
}

void ShowCanvasWindow(bool* p_open, unsigned shaderProgramId, CAP_Layer* layer, unsigned FBT, ImVec2* lastCanvasSize, Cap_Camera* capcam, ImVec2* tempDebuga)
{
    ImGuiIO* io = igGetIO();
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) { 0.0f, 0.0f });
    if (igBegin("Canvas##WindowCanvas", p_open, 0))
    {
        // this is when imgui STARTS to draw the window's contents
        // it will be used to get the WHOLE region avail
        // i.e. the canvas imgui window size
        //
        // since imgui just started drawing, its at the top left
        // corner of the window
        ImVec2 getCursorScreenPos;
        igGetCursorScreenPos(&getCursorScreenPos);

        ImVec2 boundsMin = { 0.0f, 0.0f };
        ImVec2 boundsMax = { 0.0f, 0.0f };

        // we then try to get the region available reported by
        // imgui's own API
        ImVec2 regionAvail; 
        igGetContentRegionAvail(&regionAvail);

        // using the top left + the region avail,
        // we can get the positions that will be used to draw
        // the window's contents
        boundsMin.x = getCursorScreenPos.x;
        boundsMin.y = getCursorScreenPos.y;
        boundsMax.x = getCursorScreenPos.x + regionAvail.x;
        boundsMax.y = getCursorScreenPos.y + regionAvail.y;

        // here we get the mouse position
        ImVec2 mp = io->MousePos;
        // and setup the cursors that will be used
        // TODO: OPEN AND CLOSED HAND ICON FOR PANNING
        SDL_Cursor* DRAG_CURSOR = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
        SDL_Cursor* DFLT_CURSOR = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);

        // bool for whether the projection matrix should be updated
        // if not, skip the step to not waste any computational time
        // which might not be that much of a save but w/e
        bool updateProj = false;
        if (lastCanvasSize->x != regionAvail.x || lastCanvasSize->y != regionAvail.y)
        {
            lastCanvasSize->x = regionAvail.x;
            lastCanvasSize->y = regionAvail.y;
            glBindTexture(GL_TEXTURE_2D, FBT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)lastCanvasSize->x, (int)lastCanvasSize->y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBT, 0);
            updateProj = true;
        }
        // setting the view matrix in the shader program every frame.
        // im lazy, and i dont want to try to make a
        // bool check for whether the camera updated
        // sure it MIGHT help save some time
        // but is it negligible...
        glUseProgram(shaderProgramId);
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        glmc_translate(view, (vec3) { capcam->pos.x, capcam->pos.y, 0.0f });
        glmc_scale(view, (vec3) { 1.0f * capcam->zoom, 1.0f * capcam->zoom, 1.0f });
        Cap_ShaderSetMat4(shaderProgramId, "view", view[0]);
        mat4 proj = GLM_MAT4_IDENTITY_INIT;
        glmc_ortho(-(lastCanvasSize->x / 2.0f), lastCanvasSize->x / 2.0f, -(lastCanvasSize->y / 2.0f), (lastCanvasSize->y / 2.0f), -0.1f, 100.f, proj);
        if (updateProj)
        {
            Cap_ShaderSetMat4(shaderProgramId, "proj", proj[0]);
            updateProj = false;
        }
        glUseProgram(0);
        
        if (mp.x > boundsMin.x && mp.x < boundsMax.x)
        {
            if (mp.y > boundsMin.y && mp.y < boundsMax.y)
            {
                if (io->MouseDown[2])
                {
                    ImVec2 panningBoundsMin = { (layer->width * -0.5f - 100.f) * capcam->zoom, (layer->height * -0.5f - 100.f) * capcam->zoom };
                    ImVec2 panningBoundsMax = { (layer->width * 0.5f + 100.f) * capcam->zoom, (layer->height * 0.5f + 100.f) * capcam->zoom };
                    SDL_SetCursor(DRAG_CURSOR);
                    capcam->pos.x += io->MouseDelta.x;
                    capcam->pos.x = fminf(fmaxf(panningBoundsMin.x, capcam->pos.x), panningBoundsMax.x);
                    capcam->pos.y -= io->MouseDelta.y;
                    capcam->pos.y = fminf(fmaxf(panningBoundsMin.y, capcam->pos.y), panningBoundsMax.y);
                }
                else
                {
                    SDL_SetCursor(DFLT_CURSOR);
                }

                float midX = (boundsMax.x - boundsMin.x) / 2.f;
                float midY = (boundsMax.y - boundsMin.y) / 2.f;

                // Mouse position relative to the window
                float centerX = (mp.x - boundsMin.x) - midX;
                float centerY = (mp.y - boundsMin.y) - midY;

                // Calculate the visible region of the image in world space (including panning and zoom)
                float halfImageX = ((float)layer->width / 2.0f) * capcam->zoom;
                float halfImageY = ((float)layer->height / 2.0f) * capcam->zoom;

                ImVec2 imageMin, imageMax;
                imageMin.x = -halfImageX + capcam->pos.x;
                imageMax.x = halfImageX + capcam->pos.x;
                imageMin.y = -halfImageY - capcam->pos.y;
                imageMax.y = halfImageY - capcam->pos.y;

                // Normalize mouse position to the image's world space
                float normX = (centerX - imageMin.x) / (imageMax.x - imageMin.x);
                float normY = (centerY - imageMin.y) / (imageMax.y - imageMin.y);

                // Map normalized coordinates to image space
                float imageSpaceX = normX * layer->width;
                float imageSpaceY = normY * layer->height;

                // Convert to integer pixel coordinates
                int pixelX = (int)(imageSpaceX);
                int pixelY = (int)(imageSpaceY);

                cursorCoords.x = imageSpaceX;
                cursorCoords.y = imageSpaceY;

                // Check if the mouse is within the image bounds
                if (io->MouseDown[0])
                {
                    float step = 0.01f;
                    //ImVec2 scaledMouseDelta = { mp.x - prevMousePos.x / capcam->zoom, mp.y - prevMousePos.y / capcam->zoom };
                    for (float s = 0.0f; s < 1.0f; s += step)
                    {
                        ImVec2 itp = { prevMousePos.x + s * (mp.x - prevMousePos.x), prevMousePos.y + s * (mp.y - prevMousePos.y) };
                        float centerX1 = (itp.x - boundsMin.x) - midX;
                        float centerY1 = (itp.y - boundsMin.y) - midY;
                        // Normalize mouse position to the image's world space
                        float normX1 = (centerX1 - imageMin.x) / (imageMax.x - imageMin.x);
                        float normY1 = (centerY1 - imageMin.y) / (imageMax.y - imageMin.y);

                        // Map normalized coordinates to image space
                        float imageSpaceX1 = normX1 * layer->width;
                        float imageSpaceY1 = normY1 * layer->height;

                        // Convert to integer pixel coordinates
                        int pixelX1 = (int)(imageSpaceX1);
                        int pixelY1 = (int)(imageSpaceY1);

                        // Check if the pixel coordinates are within bounds
                        if (pixelX1 >= 0 && pixelX1 < layer->width && pixelY1 >= 0 && pixelY1 < layer->height)
                        {
                            // Paint the pixel (brush size is optional here)
                            // TODO: EXPOSE THIS SHID
                            int brushSize = 3; // Example brush size
                            //for (int dy = -brushSize; dy <= brushSize; dy++)
                            //{
                            //    for (int dx = -brushSize; dx <= brushSize; dx++)
                            //    {
                            //        int nx = pixelX + dx;
                            //        int ny = pixelY + dy;

                            //        if (nx >= 0 && nx < layer->width && ny >= 0 && ny < layer->height)
                            //        {
                            //            CAP_PixelRGBA* pixel = &layer->data[ny * layer->width + nx];
                            //            pixel->r = 1.0f; // Red
                            //            pixel->g = 0.0f;
                            //            pixel->b = 0.0f;
                            //            pixel->a = 1.0f; // Opaque
                            //        }
                            //    }
                            //}
                            CAP_PixelRGBA* pixel = &layer->data[pixelY1 * layer->width + pixelX1];
                            pixel->r = currentColorPri[0];
                            pixel->g = currentColorPri[1];
                            pixel->b = currentColorPri[2];
                            pixel->a = currentColorPri[3];
                        }
                    }
                    // Refresh the texture
                    Cap_LayerRefreshImage(layer);
                }
                prevMousePos.x = mp.x;
                prevMousePos.y = mp.y;

                // TODO: RELATIVE ZOOMING
                if (io->MouseWheel > 0.f && io->MouseWheel > FLT_EPSILON)
                {
                    if (io->KeyCtrl)
                        if (capcam->zoom + 0.5f > 20.0f)
                            capcam->zoom = 20.0f;
                        else
                            capcam->zoom += 0.5f;
                }
                else if (io->MouseWheel < 0.f && io->MouseWheel < FLT_EPSILON)
                {
                    if (io->KeyCtrl)
                        if ((capcam->zoom - 0.5f) > 0.0f)
                            capcam->zoom -= 0.5f;
                        else
                            capcam->zoom = 0.1f;
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

        //ImDrawList_AddRectFilled(igGetWindowDrawList(), boundsMin, boundsMax, 0xFFFFFFFF, 0.0f, 0);
        ImDrawList_AddImage(igGetWindowDrawList(), FBT, boundsMin, boundsMax, (ImVec2) { 0, 1 }, (ImVec2) { 1, 0 }, 0xFFFFFFFF);
    }
    igEnd();
    igPopStyleVar(1);
}



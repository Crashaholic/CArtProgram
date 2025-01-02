#ifndef CAP_PROGWINDOWS_H
#define CAP_PROGWINDOWS_H

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include "cap_layer.h"
#include "cap_shader.h"

typedef struct Cap_CanvasCamera
{
    ImVec2 pos;
    float zoom;
} Cap_Camera;

void ShowToolbarWindow(bool* p_open);
void ShowBrushWindow(bool* p_open);
void ShowBrushSettingsWindow(bool* p_open);
void ShowColorPickerWindow(bool* p_open);
void ShowLayersWindow(bool* p_open);
void ShowPreviewWindow(bool* p_open);
void ShowHistoryWindow(bool* p_open);
void ShowStatusWindow(bool* p_open);
void ShowCanvasWindow(bool* p_open, unsigned shaderProgramId, CAP_Layer* layer, unsigned FBT, ImVec2* lastCanvasSize, Cap_Camera* capcam, ImVec2* tempDebug);

#endif

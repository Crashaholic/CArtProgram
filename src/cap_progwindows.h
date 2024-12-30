#ifndef CAP_PROGWINDOWS_H
#define CAP_PROGWINDOWS_H

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

void ShowToolbarWindow(bool* p_open);
void ShowBrushWindow(bool* p_open);
void ShowBrushSettingsWindow(bool* p_open);
void ShowColorPickerWindow(bool* p_open);
void ShowLayersWindow(bool* p_open);
void ShowPreviewWindow(bool* p_open);
void ShowHistoryWindow(bool* p_open);
void ShowCanvasWindow(bool* p_open);

#endif

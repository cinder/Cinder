#pragma once

#include "Configuration.h"
#include <Windows.h>

extern BOOL mswDoConfigureDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
void loadConfig( Configuration *config );
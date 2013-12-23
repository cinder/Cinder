#include "WindowsConfig.h"


#include <stdio.h>

#define ID_DRAW_CINDER_LOGO		2000 // used in Resources.rc

static const wchar_t *REGISTRY_PATH = L"Software\\libcinder\\TestScreensaver";

void loadConfig( Configuration *config )
{
	HKEY key;
	LONG result = ::RegOpenKeyEx( HKEY_CURRENT_USER, REGISTRY_PATH, 0, KEY_READ, &key );
	if( result != ERROR_SUCCESS )
		return;	

	DWORD dwLen = sizeof(DWORD);
	DWORD dwDrawLogo;
	::RegQueryValueEx( key, L"DrawCinderLogo", 0, NULL, (LPBYTE)&dwDrawLogo, &dwLen );
	config->mDrawCinderLogo = dwDrawLogo == 1;

	::RegCloseKey( key );
}

// Saves our settings to the registry
void saveConfig( const Configuration &config )
{
	HKEY key;
	LONG result = ::RegCreateKeyEx( HKEY_CURRENT_USER, REGISTRY_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL );
	if( result != ERROR_SUCCESS )
		return;

	DWORD dwDrawLogo = config.mDrawCinderLogo;
	result = ::RegSetValueEx( key, L"DrawCinderLogo", 0, REG_DWORD, (PBYTE)&dwDrawLogo, sizeof(DWORD) );

	::RegCloseKey( key );
}

BOOL mswDoConfigureDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) {
		case WM_INITDIALOG: {
			Configuration tempConfig;
			loadConfig( &tempConfig );
			::CheckDlgButton( hDlg, ID_DRAW_CINDER_LOGO, (UINT)tempConfig.mDrawCinderLogo );
			return TRUE;
		}
		break;
		case WM_COMMAND: {
			long command = LOWORD( wParam );
			switch( command ) {
				case ID_DRAW_CINDER_LOGO:
					::CheckDlgButton( hDlg, ID_DRAW_CINDER_LOGO, ! ::IsDlgButtonChecked( hDlg, ID_DRAW_CINDER_LOGO ) );
					return TRUE;
				break;
				default:
					if( command == IDOK ) {
						Configuration tempConfig;
						tempConfig.mDrawCinderLogo = ::IsDlgButtonChecked( hDlg, ID_DRAW_CINDER_LOGO ) == 1;
						saveConfig( tempConfig );
					}
					::EndDialog( hDlg, command );					
					return TRUE;
			}
		}
		break;
	}
	
	return FALSE;
}
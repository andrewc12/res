//
// Memecode Res
// (C) 2006 Matthew Allen
// fret@memecode.com
//
// This software is covered by the LGPL license available here:
//		http://www.gnu.org/copyleft/lesser.html
//
// Created: 31/1/2006
//
#define WINVER 0x500
#include <stdio.h>
#include <conio.h>
#include <windows.h>

#ifndef DISP_CHANGE_BADDUALVIEW
#define DISP_CHANGE_BADDUALVIEW     -6
#endif

typedef unsigned int uint;
typedef unsigned char uchar;
int Args = 0;
char **Arg = 0;

bool GetOpt(char *opt, char **str = 0)
{
	if (Arg && opt)
	{
		for (int i=1; i<Args; i++)
		{
			if (Arg[i] &&
				(Arg[i][0] == '-' || Arg[i][0] == '/'))
			{
				if (stricmp(opt, Arg[i] + 1) == 0)
				{
					if (str && Arg[i+1] && i < Args - 1)
					{
						*str = Arg[i+1];
					}

					return true;
				}
			}
		}
	}

	return false;
}

bool GetOpt(char *opt, uint &i)
{
	char *s = 0;
	if (GetOpt(opt, &s))
	{
		uint n = atoi(s);
		if (n)
		{
			i = n;
			return true;
		}
	}

	return false;
}

bool GetOpt(char *opt, int &i)
{
	char *s = 0;
	if (GetOpt(opt, &s))
	{
		int n = atoi(s);
		if (n)
		{
			i = n;
			return true;
		}
	}

	return false;
}

int main(int args, char **arg)
{
	printf("Memecode Res v1.00\n\n");

	Args = args;
	Arg = arg;
	HWND hwnd = GetDesktopWindow();
	bool error = false;

	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	HDC hdc = GetDC(hwnd);
	int bits = GetDeviceCaps(hdc, BITSPIXEL);
	int refresh = GetDeviceCaps(hdc, VREFRESH);

	printf("Current Desktop Resolution: %i x %i @ %i bits per pixel, refresh=%ihz.\n\n", x, y, bits, refresh);
	
	int size = sizeof(DEVMODE) * 2;
	DEVMODE *mode = (DEVMODE*)malloc(size);
	if (mode)
	{
		int ddevice = -1;
		GetOpt("device", ddevice);

		char *devname = 0;
		DISPLAY_DEVICE disp;
		memset(&disp, 0, sizeof(disp));
		disp.cb = sizeof(disp);
		for (int i=0; EnumDisplayDevices(0, i, &disp, 0); i++)
		{
			printf("Device[%i] is '%s' (%s)\n", i, disp.DeviceString, disp.DeviceName);
			
			if
			(
				!devname
				&&
				(
					(
						ddevice >= 0
						&&
						ddevice == i
					)
					||
					(
						ddevice < 0
						&&
						(disp.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) != 0
					)
				)
			)
			{
				devname = strdup((char*)disp.DeviceName);
			}

			DISPLAY_DEVICE temp = disp;
			if (EnumDisplayDevices(temp.DeviceName, 0, &disp, 0))
			{
				printf("\tAttached monitor is '%s' (%s)\n", disp.DeviceString, disp.DeviceName);
			}

			memset(mode, 0, size);
			mode->dmSize = sizeof(DEVMODE);
			mode->dmDriverExtra = sizeof(DEVMODE);
			if (EnumDisplaySettings((char*)temp.DeviceName, ENUM_CURRENT_SETTINGS, mode))
			{
				printf("\tCurrent mode: %i x %i @ %i bit per pixel, refresh %i hz.\n",
					mode->dmPelsWidth, mode->dmPelsHeight, mode->dmBitsPerPel, mode->dmDisplayFrequency);
			}

			memset(&disp, 0, sizeof(disp));
			disp.cb = sizeof(disp);
		}

		printf("\n");

		uint dx, dy, dbpp, drefresh;
		if (GetOpt("x", dx) &&
			GetOpt("y", dy) &&
			GetOpt("bpp", dbpp) &&
			GetOpt("refresh", drefresh))
		{
			memset(mode, 0, size);
			mode->dmSize = sizeof(DEVMODE);
			mode->dmDriverExtra = sizeof(DEVMODE);
			if (EnumDisplaySettings((char*)devname, ENUM_CURRENT_SETTINGS, mode))
			{
				if (mode->dmBitsPerPel != dbpp ||
					mode->dmPelsWidth != dx ||
					mode->dmPelsHeight != dy ||
					mode->dmDisplayFrequency != drefresh)
				{
					mode->dmBitsPerPel = dbpp;
					mode->dmPelsWidth = dx;
					mode->dmPelsHeight = dy;
					mode->dmDisplayFrequency = drefresh;
					mode->dmFields |= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

					#if 1
					LONG result = ChangeDisplaySettingsEx(devname, mode, 0, CDS_RESET, 0);
					#else					
					LONG result = ChangeDisplaySettings(mode, CDS_RESET);
					#endif

					switch (result)
					{
						case DISP_CHANGE_SUCCESSFUL:
							printf("ChangeDisplaySettings: The settings change was successful.\n");
							break;
						case DISP_CHANGE_BADDUALVIEW:
							error = true;
							printf("ChangeDisplaySettings: Windows XP - The settings change was unsuccessful because system is DualView capable.\n");
							break;
						case DISP_CHANGE_BADFLAGS:
							error = true;
							printf("ChangeDisplaySettings: An invalid set of flags was passed in.\n");
							break;
						case DISP_CHANGE_BADMODE:
							error = true;
							printf("ChangeDisplaySettings: The graphics mode is not supported.\n");
							break;
						case DISP_CHANGE_BADPARAM:
							error = true;
							printf("ChangeDisplaySettings: An invalid parameter was passed in. This can include an invalid flag or combination of flags.\n");
							break;
						case DISP_CHANGE_FAILED:
							error = true;
							printf("ChangeDisplaySettings: The display driver failed the specified graphics mode.\n");
							break;
						case DISP_CHANGE_NOTUPDATED:
							error = true;
							printf("ChangeDisplaySettings: Windows NT/2000/XP - Unable to write settings to the registry.\n");
							break;
						case DISP_CHANGE_RESTART:
							error = true;
							printf("ChangeDisplaySettings: The computer must be restarted in order for the graphics mode to work.\n");
							break;
						default:
							error = true;
							printf("ChangeDisplaySettings: Unknown error %i\n", result);
							break;
					}
				}
				else
				{
					printf("Video mode is already correct.\n");
				}
			}
			else
			{
				printf("EnumDisplaySettings(\"%s\", ...) failed.\n", devname);
				error = true;
			}
		}
		else
		{
			printf(	"\n"
					"Usage: res -x <width> -y <height> -bpp <bits_per_pixel> -refresh <refresh_rate> [-device <index>]\n"
					"\n"
					"Example: res -x 1280 -y 1024 -bpp 32 -refresh 75\n");
		}
	}

	ReleaseDC(hwnd, hdc);

	// if (error)
	{
		printf("\nPress any key to continue...\n");
		getch();
	}

	return 0;
}
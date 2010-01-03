/**************************************************************************
*
* Tint2 : common windows function
*
* Copyright (C) 2007 Pål Staurland (staura@gmail.com)
* Modified (C) 2008 thierry lorthiois (lorthiois@bbsoft.fr)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "common.h"



void copy_file(const char *pathSrc, const char *pathDest)
{
	FILE *fileSrc, *fileDest;
	char line[100];
	int  nb;

	fileSrc = fopen(pathSrc, "rb");
	if (fileSrc == NULL) return;

	fileDest = fopen(pathDest, "wb");
	if (fileDest == NULL) return;

	while ((nb = fread(line, 1, 100, fileSrc)) > 0) fwrite(line, 1, nb, fileDest);

	fclose (fileDest);
	fclose (fileSrc);
}


int parse_line (const char *line, char **key, char **value)
{
	char *a, *b;

	/* Skip useless lines */
	if ((line[0] == '#') || (line[0] == '\n')) return 0;
	if (!(a = strchr (line, '='))) return 0;

	/* overwrite '=' with '\0' */
	a[0] = '\0';
	*key = strdup (line);
	a++;

	/* overwrite '\n' with '\0' if '\n' present */
	if ((b = strchr (a, '\n'))) b[0] = '\0';

	*value = strdup (a);

	g_strstrip(*key);
	g_strstrip(*value);
	return 1;
}


int hex_char_to_int (char c)
{
	int r;

	if (c >= '0' && c <= '9')  r = c - '0';
	else if (c >= 'a' && c <= 'f')  r = c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')  r = c - 'A' + 10;
	else  r = 0;

	return r;
}


int hex_to_rgb (char *hex, int *r, int *g, int *b)
{
	int len;

	if (hex == NULL || hex[0] != '#') return (0);

	len = strlen (hex);
	if (len == 3 + 1) {
		*r = hex_char_to_int (hex[1]);
		*g = hex_char_to_int (hex[2]);
		*b = hex_char_to_int (hex[3]);
	}
	else if (len == 6 + 1) {
		*r = hex_char_to_int (hex[1]) * 16 + hex_char_to_int (hex[2]);
		*g = hex_char_to_int (hex[3]) * 16 + hex_char_to_int (hex[4]);
		*b = hex_char_to_int (hex[5]) * 16 + hex_char_to_int (hex[6]);
	}
	else if (len == 12 + 1) {
		*r = hex_char_to_int (hex[1]) * 16 + hex_char_to_int (hex[2]);
		*g = hex_char_to_int (hex[5]) * 16 + hex_char_to_int (hex[6]);
		*b = hex_char_to_int (hex[9]) * 16 + hex_char_to_int (hex[10]);
	}
	else return 0;

	return 1;
}


void get_color (char *hex, double *rgb)
{
	int r, g, b;
	hex_to_rgb (hex, &r, &g, &b);

	rgb[0] = (r / 255.0);
	rgb[1] = (g / 255.0);
	rgb[2] = (b / 255.0);
}


void adjust_asb(DATA32 *data, int w, int h, int alpha, float satur, float bright)
{
	unsigned int x, y;
	unsigned int a, r, g, b, argb;
	unsigned long id;
	int cmax, cmin;
	float h2, f, p, q, t;
	float hue, saturation, brightness;
	float redc, greenc, bluec;

	for(y = 0; y < h; y++) {
		for(id = y * w, x = 0; x < w; x++, id++) {
			argb = data[id];
			a = (argb >> 24) & 0xff;
			// transparent => nothing to do.
			if (a == 0) continue;
			r = (argb >> 16) & 0xff;
			g = (argb >> 8) & 0xff;
			b = (argb) & 0xff;

			// convert RGB to HSB
			cmax = (r > g) ? r : g;
			if (b > cmax) cmax = b;
			cmin = (r < g) ? r : g;
			if (b < cmin) cmin = b;
			brightness = ((float)cmax) / 255.0f;
			if (cmax != 0)
				saturation = ((float)(cmax - cmin)) / ((float)cmax);
			else
				saturation = 0;
			if (saturation == 0)
				hue = 0;
			else {
				redc = ((float)(cmax - r)) / ((float)(cmax - cmin));
				greenc = ((float)(cmax - g)) / ((float)(cmax - cmin));
				bluec = ((float)(cmax - b)) / ((float)(cmax - cmin));
				if (r == cmax)
					hue = bluec - greenc;
				else if (g == cmax)
					hue = 2.0f + redc - bluec;
				else
					hue = 4.0f + greenc - redc;
				hue = hue / 6.0f;
				if (hue < 0)
					hue = hue + 1.0f;
			}

			// adjust
			saturation += satur;
			if (saturation < 0.0) saturation = 0.0;
			if (saturation > 1.0) saturation = 1.0;
			brightness += bright;
			if (brightness < 0.0) brightness = 0.0;
			if (brightness > 1.0) brightness = 1.0;
			if (alpha != 100)
				a = (a * alpha)/100;

			// convert HSB to RGB
			if (saturation == 0) {
				r = g = b = (int)(brightness * 255.0f + 0.5f);
			} else {
				h2 = (hue - (int)hue) * 6.0f;
				f = h2 - (int)(h2);
				p = brightness * (1.0f - saturation);
				q = brightness * (1.0f - saturation * f);
				t = brightness * (1.0f - (saturation * (1.0f - f)));
				switch ((int) h2) {
				case 0:
					r = (int)(brightness * 255.0f + 0.5f);
					g = (int)(t * 255.0f + 0.5f);
					b = (int)(p * 255.0f + 0.5f);
					break;
				case 1:
					r = (int)(q * 255.0f + 0.5f);
					g = (int)(brightness * 255.0f + 0.5f);
					b = (int)(p * 255.0f + 0.5f);
					break;
				case 2:
					r = (int)(p * 255.0f + 0.5f);
					g = (int)(brightness * 255.0f + 0.5f);
					b = (int)(t * 255.0f + 0.5f);
					break;
				case 3:
					r = (int)(p * 255.0f + 0.5f);
					g = (int)(q * 255.0f + 0.5f);
					b = (int)(brightness * 255.0f + 0.5f);
					break;
				case 4:
					r = (int)(t * 255.0f + 0.5f);
					g = (int)(p * 255.0f + 0.5f);
					b = (int)(brightness * 255.0f + 0.5f);
					break;
				case 5:
					r = (int)(brightness * 255.0f + 0.5f);
					g = (int)(p * 255.0f + 0.5f);
					b = (int)(q * 255.0f + 0.5f);
					break;
				}
			}

			argb = a;
			argb = (argb << 8) + r;
			argb = (argb << 8) + g;
			argb = (argb << 8) + b;
			data[id] = argb;
		}
	}
}


void createHeuristicMask(DATA32* data, int w, int h)
{
	unsigned char* udata = (unsigned char*)data;
	int b = udata[0];
	int g = udata[1];
	int r = udata[2];
	int i;
	for (i=0; i<h*w; ++i) {
		if ( abs(b-*udata)<5 && abs(g-*(udata+1))<5 && abs(r-*(udata+2))<5 ) {
			*(udata+3) = 0;
		}
		udata += 4;
	}
}

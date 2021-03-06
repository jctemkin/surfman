/*
 * Copyright (c) 2012 Citrix Systems, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef VESA_H
#define VESA_H

typedef struct {
    unsigned int height, width; /* Surface's width and height in pixels. */
    unsigned int stride;        /* Number of bytes in one line. */
    unsigned int Bpp;           /* bytes per pixels. */

    uint8_t *mapped_fb;
    unsigned int mapped_fb_size;
    int pages_domid;
    surfman_surface_t *surfman_surface;
} fb_surface;

#endif


/*
 * Copyright (c) 2013 Citrix Systems, Inc.
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

#include "project.h"

#define ENABLE_AVX

#include <mmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

#ifdef ENABLE_AVX
#include <immintrin.h>
#endif


#ifdef ENABLE_AVX
const int gAVXEnabled = 1;
const int BYTE_ALIGNMENT = 32;
#else
const int gAVXEnabled = 0;
const int BYTE_ALIGNMENT = 16;
#endif

// Jam in displayhandler's memcpy fns
// Written by Rian Quinn (quinnr@ainfosec.com)
// ============================================================================
void slow_memcpy(void *dst, const void *src, int32_t len)
{
    asm("cld;"
        "rep;"
        "movsb"
        :
        :"S"(src), "D"(dst), "c"((size_t)len));
}

void fast_memcpy(void *dst, const void *src, int32_t len)
{
    uint8_t *uint8_dst = (uint8_t *)dst;
    uint8_t *uint8_src = (uint8_t *)src;

    // ------------------------------------------------------------------------
    // Copy unaligned portion of the destination

    int32_t i = 0;
    int32_t alignment_dst = (uintptr_t)(&uint8_dst[i]) & (BYTE_ALIGNMENT - 1);

    if(alignment_dst != 0)
    {
        if (BYTE_ALIGNMENT - alignment_dst > len)
            i = len;
        else
            i = BYTE_ALIGNMENT - alignment_dst;

        slow_memcpy(dst, src, i);
    }

    if(i >= len)
        return;

    int32_t alignment_src = (uintptr_t)(&uint8_src[i]) & (BYTE_ALIGNMENT - 1);

    // ------------------------------------------------------------------------
    // Copy aligned portion of the destination

#ifndef ENABLE_AVX

    if(alignment_src != 0)
    {
        for(; i < len - 127; i += 128)
        {
            const __m128i xmm0 = _mm_loadu_si128((__m128i *)(&uint8_src[i]));
            const __m128i xmm1 = _mm_loadu_si128((__m128i *)(&uint8_src[i + 16]));
            const __m128i xmm2 = _mm_loadu_si128((__m128i *)(&uint8_src[i + 32]));
            const __m128i xmm3 = _mm_loadu_si128((__m128i *)(&uint8_src[i + 48]));
            const __m128i xmm4 = _mm_loadu_si128((__m128i *)(&uint8_src[i + 64]));
            const __m128i xmm5 = _mm_loadu_si128((__m128i *)(&uint8_src[i + 80]));
            const __m128i xmm6 = _mm_loadu_si128((__m128i *)(&uint8_src[i + 96]));
            const __m128i xmm7 = _mm_loadu_si128((__m128i *)(&uint8_src[i + 112]));

            _mm_stream_si128((__m128i *)(&uint8_dst[i]), xmm0);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 16]), xmm1);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 32]), xmm2);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 48]), xmm3);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 64]), xmm4);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 80]), xmm5);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 96]), xmm6);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 112]), xmm7);
        }

        for(; i < len - 15; i += 16)
        {
            const __m128i xmm0 = _mm_loadu_si128((__m128i *)(&uint8_src[i]));
            _mm_stream_si128((__m128i *)(&uint8_dst[i]), xmm0);
        }
    }
    else
    {
        for(; i < len - 127; i += 128)
        {
            const __m128i xmm0 = _mm_load_si128((__m128i *)(&uint8_src[i]));
            const __m128i xmm1 = _mm_load_si128((__m128i *)(&uint8_src[i + 16]));
            const __m128i xmm2 = _mm_load_si128((__m128i *)(&uint8_src[i + 32]));
            const __m128i xmm3 = _mm_load_si128((__m128i *)(&uint8_src[i + 48]));
            const __m128i xmm4 = _mm_load_si128((__m128i *)(&uint8_src[i + 64]));
            const __m128i xmm5 = _mm_load_si128((__m128i *)(&uint8_src[i + 80]));
            const __m128i xmm6 = _mm_load_si128((__m128i *)(&uint8_src[i + 96]));
            const __m128i xmm7 = _mm_load_si128((__m128i *)(&uint8_src[i + 112]));

            _mm_stream_si128((__m128i *)(&uint8_dst[i]), xmm0);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 16]), xmm1);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 32]), xmm2);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 48]), xmm3);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 64]), xmm4);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 80]), xmm5);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 96]), xmm6);
            _mm_stream_si128((__m128i *)(&uint8_dst[i + 112]), xmm7);
        }

        for(; i < len - 15; i += 16)
        {
            const __m128i xmm0 = _mm_load_si128((__m128i *)(&uint8_src[i]));
            _mm_stream_si128((__m128i *)(&uint8_dst[i]), xmm0);
        }
    }

#else

    if(alignment_src != 0)
    {
        for(; i < len - 255; i += 256)
        {
            const __m256i xmm0 = _mm256_loadu_si256((__m256i *)(&uint8_src[i]));
            const __m256i xmm1 = _mm256_loadu_si256((__m256i *)(&uint8_src[i + 32]));
            const __m256i xmm2 = _mm256_loadu_si256((__m256i *)(&uint8_src[i + 64]));
            const __m256i xmm3 = _mm256_loadu_si256((__m256i *)(&uint8_src[i + 96]));
            const __m256i xmm4 = _mm256_loadu_si256((__m256i *)(&uint8_src[i + 128]));
            const __m256i xmm5 = _mm256_loadu_si256((__m256i *)(&uint8_src[i + 160]));
            const __m256i xmm6 = _mm256_loadu_si256((__m256i *)(&uint8_src[i + 192]));
            const __m256i xmm7 = _mm256_loadu_si256((__m256i *)(&uint8_src[i + 224]));

            _mm256_stream_si256((__m256i *)(&uint8_dst[i]), xmm0);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 32]), xmm1);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 64]), xmm2);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 96]), xmm3);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 128]), xmm4);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 160]), xmm5);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 192]), xmm6);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 224]), xmm7);
        }

        for(; i < len - 31; i += 32)
        {
            const __m256i xmm0 = _mm256_loadu_si256((__m256i *)(&uint8_src[i]));
            _mm256_stream_si256((__m256i *)(&uint8_dst[i]), xmm0);
        }
    }
    else
    {
        for(; i < len - 255; i += 256)
        {
            const __m256i xmm0 = _mm256_load_si256((__m256i *)(&uint8_src[i]));
            const __m256i xmm1 = _mm256_load_si256((__m256i *)(&uint8_src[i + 32]));
            const __m256i xmm2 = _mm256_load_si256((__m256i *)(&uint8_src[i + 64]));
            const __m256i xmm3 = _mm256_load_si256((__m256i *)(&uint8_src[i + 96]));
            const __m256i xmm4 = _mm256_load_si256((__m256i *)(&uint8_src[i + 128]));
            const __m256i xmm5 = _mm256_load_si256((__m256i *)(&uint8_src[i + 160]));
            const __m256i xmm6 = _mm256_load_si256((__m256i *)(&uint8_src[i + 192]));
            const __m256i xmm7 = _mm256_load_si256((__m256i *)(&uint8_src[i + 224]));

            _mm256_stream_si256((__m256i *)(&uint8_dst[i]), xmm0);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 32]), xmm1);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 64]), xmm2);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 96]), xmm3);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 128]), xmm4);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 160]), xmm5);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 192]), xmm6);
            _mm256_stream_si256((__m256i *)(&uint8_dst[i + 224]), xmm7);
        }

        for(; i < len - 31; i += 32)
        {
            const __m256i xmm0 = _mm256_load_si256((__m256i *)(&uint8_src[i]));
            _mm256_stream_si256((__m256i *)(&uint8_dst[i]), xmm0);
        }
    }

#endif

    // ------------------------------------------------------------------------
    // Copy remaining portion

    if(len - i > 0)
    {
        slow_memcpy(&uint8_dst[i], &uint8_src[i], len - i);
    }
} 
// ============================================================================


//Allocates the struct containing metadata (useful only to surfman) about a DRM framebuffer
static inline struct drm_framebuffer *
__i915_framebuffer_alloc(unsigned int width, unsigned int height,
                         unsigned int depth, unsigned int bpp,
                         unsigned int pitch, unsigned int size,
                         uint32_t handle, uint32_t id)
{
    struct drm_framebuffer *drm;

    drm = calloc(1, sizeof (*drm));
    if (!drm) {
        return NULL;
    }
    //Contains fn ptrs to surfman fns that perform operations on that framebuffer
    drm->ops = &framebuffer_i915_ops;
    drm->handle = handle;
    drm->id = id;

    drm->fb.width = width;
    drm->fb.height = height;
    drm->fb.depth = depth;
    drm->fb.bpp = bpp;
    drm->fb.pitch = pitch;
    drm->fb.size = size;

    drm->fb.offset = -1;

    return drm;
}

static struct drm_framebuffer *
i915_framebuffer_create(struct drm_device *device, const struct drm_surface *surface)
{
    const struct framebuffer *sfb = &surface->fb;
    struct drm_framebuffer *dfb;
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    int err;
    uint32_t id;

    memset(&creq, 0, sizeof (creq));
    creq.height = sfb->height;
    creq.width = sfb->width;
    creq.bpp = sfb->bpp;

    //Creates gem buffer object
    if (drmIoctl(device->fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq)) {
        DRM_DBG("drmIoctl(%s, DRM_IOCTL_MODE_CREATE_DUMB, %ux%u:%u) failed (%s).", device->devnode,
                sfb->width, sfb->height, sfb->bpp, strerror(errno));
        return NULL;
    }

    //Binds it to DRM for use as a scanout buffer
    if (drmModeAddFB(device->fd, sfb->width, sfb->height, sfb->depth, sfb->bpp,
                     creq.pitch, creq.handle, &id)) {
        err = errno;
        DRM_DBG("drmModeAddFB(%s, %ux%u:%u/%u) failed (%s).", device->devnode,
                sfb->width, sfb->height, sfb->depth, sfb->bpp, strerror(errno));
        goto fail_fb;
    }

    //Allocates struct drm_framebuffer
    dfb = __i915_framebuffer_alloc(sfb->width, sfb->height, sfb->depth, sfb->bpp,
                                   creq.pitch, creq.size, creq.handle, id);
    if (!dfb) {
        err = errno;
        DRM_DBG("i915_framebuffer_alloc(%s, %ux%u:%u/%u) failed (%s).", device->devnode,
                sfb->width, sfb->height, sfb->depth, sfb->bpp, strerror(errno));
        goto fail_alloc;
    }
    dfb->device = device;
    return dfb;

fail_alloc:
    if (drmModeRmFB(device->fd, id)) {
        DRM_DBG("drmModeRmFB(%s, %u) failed (%s).", device->devnode, id, strerror(errno));
    }
fail_fb:
    memset(&dreq, 0, sizeof (dreq));
    dreq.handle = creq.handle;
    if (drmIoctl(device->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq)) {
        DRM_DBG("drmIoctl(%s, DRM_IOCTL_MODE_DESTROY_DUMB, %u) failed (%s).", device->devnode,
                creq.handle, strerror(errno));
    }
    errno = err;
    return NULL;
}

static int i915_framebuffer_map(struct drm_framebuffer *framebuffer)
{
    struct framebuffer *fb = &framebuffer->fb;
    const struct drm_device *dev = framebuffer->device;
    struct drm_i915_gem_mmap map_arg;
    int rc;
    uintptr_t ptr;

    //I915_MMAP_WC is only defined in mmap versions >=1.
    //Consider checking I915_PARAM_MMAP_VERSION.
    memset(&map_arg, 0, sizeof(map_arg));
    map_arg.handle = framebuffer->handle;
    map_arg.size = fb->size;
    map_arg.flags = I915_MMAP_WC;

    if(drmIoctl(dev->fd, DRM_IOCTL_I915_GEM_MMAP, &map_arg))
    {
        rc = -errno;
        DRM_DBG("drmIoctl(%s, DRM_IOCTL_I915_GEM_MMAP, {%u, %u, I915_MMAP_WC}) failed (%s).", 
                dev->devnode, framebuffer->handle, fb->size, strerror(errno));
        return rc;
    }
    ptr = (uintptr_t)map_arg.addr_ptr;
    fb->map = (void *)ptr;
    //fb->map = (void *)map_arg.addr_ptr;

    return 0;
}

static void i915_framebuffer_unmap(struct drm_framebuffer *framebuffer)
{
    if (munmap(framebuffer->fb.map, framebuffer->fb.size)) {
        DRM_DBG("munmap() failed (%s).", strerror(errno));
    }
}

static void i915_framebuffer_refresh(struct drm_framebuffer *drm,
                                     const struct framebuffer *source,
                                     const struct rect *r)
{
    struct framebuffer *dfb = &drm->fb;
    const struct framebuffer *sfb = source;
    unsigned int i;

    uint8_t *src = sfb->map + (r->y * sfb->pitch) + (r->x * (sfb->bpp / 8));
    uint8_t *dst = dfb->map + (r->y * dfb->pitch) + (r->x * (dfb->bpp / 8));

    if ((sfb->map == MAP_FAILED || sfb->map == NULL) ||
        (dfb->map == MAP_FAILED || dfb->map == NULL)) {
        DRM_WRN("Trying to refresh unmapped framebuffer: source:%p, sink:%p",
                sfb->map, dfb->map);
        return;
    }
    if ((sfb->depth != dfb->depth) || (sfb->bpp != dfb->bpp)) {
        DRM_WRN("Invalid geometry between Surfman and libDRM: %u/%u vs %u/%u (depth/bpp)",
                sfb->depth, sfb->bpp, dfb->depth, dfb->bpp);
        return;
    }
    if (((r->y + r->h) > sfb->height) || ((r->x + r->w) > sfb->width)) {
        DRM_WRN("Dirty rectangle out of framebuffer bounds: %ux%u vs %u,%u:%ux%u",
                sfb->width, sfb->height, r->x, r->y, r->w, r->h);
        return;
    }

    int rect_width_bytes = r->w * (dfb->bpp / 8);
    for (i = 0; i < r->h; ++i) {
        fast_memcpy(dst, src, rect_width_bytes);
        src += sfb->pitch;
        dst += dfb->pitch;
    }
}


static void i915_framebuffer_release(struct drm_framebuffer *framebuffer)
{
    struct drm_mode_destroy_dumb dreq = { 0 };

    if (framebuffer->fb.map && framebuffer->fb.map != MAP_FAILED) {
        i915_framebuffer_unmap(framebuffer);
    }

    if (drmModeRmFB(framebuffer->device->fd, framebuffer->id)) {
        DRM_DBG("drmModeRmFB(%s, %u) failed (%s).",
                framebuffer->device->devnode, framebuffer->id, strerror(errno));
    }

    dreq.handle = framebuffer->handle;
    if (drmIoctl(framebuffer->device->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq)) {
        DRM_DBG("drmIoctl(%s, DRM_IOCTL_MODE_DESTROY_DUMB, %u) failed (%s).",
                framebuffer->device->devnode, framebuffer->handle, strerror(errno));
    }
}

INTERNAL const struct drm_framebuffer_ops framebuffer_i915_ops = {
    .name = "i915",
    .create = i915_framebuffer_create,
    .map = i915_framebuffer_map,
    .unmap = i915_framebuffer_unmap,
    .refresh = i915_framebuffer_refresh,
    .release = i915_framebuffer_release //frees resources associated with this struct drm_framebuffer
};


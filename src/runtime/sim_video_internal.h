/* sim_video_internal.h: Internal video interfaces for tests */
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: X11

#ifndef SIM_VIDEO_INTERNAL_H_
#define SIM_VIDEO_INTERNAL_H_ 0

#include "sim_video.h"

/* Clear test-installed gamepad callbacks between unit tests. */
void sim_video_reset_test_gamepad_callbacks(void);

#endif

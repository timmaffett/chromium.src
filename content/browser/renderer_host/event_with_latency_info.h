// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_EVENT_WITH_LATENCY_INFO_H_
#define CONTENT_BROWSER_RENDERER_HOST_EVENT_WITH_LATENCY_INFO_H_

#include "ui/base/latency_info.h"

namespace WebKit {
class WebGestureEvent;
class WebMouseEvent;
class WebMouseWheelEvent;
}

namespace content {

template <typename T>
class EventWithLatencyInfo {
 public:
  T event;
  ui::LatencyInfo latency;

  EventWithLatencyInfo(const T& e, const ui::LatencyInfo& l)
      : event(e), latency(l) {}

  EventWithLatencyInfo() {}
};

typedef EventWithLatencyInfo<WebKit::WebGestureEvent>
    GestureEventWithLatencyInfo;
typedef EventWithLatencyInfo<WebKit::WebMouseWheelEvent>
    MouseWheelEventWithLatencyInfo;
typedef EventWithLatencyInfo<WebKit::WebMouseEvent>
    MouseEventWithLatencyInfo;

}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_EVENT_WITH_LATENCY_INFO_H_

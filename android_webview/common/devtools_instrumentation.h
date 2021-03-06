// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ANDROID_WEBVIEW_COMMON_DEVTOOLS_INSTRUMENTATION_H_
#define ANDROID_WEBVIEW_COMMON_DEVTOOLS_INSTRUMENTATION_H_

#include "base/debug/trace_event.h"

namespace android_webview {
namespace devtools_instrumentation {

namespace internal {
const char kCategory[] = "Java,devtools";
const char kEmbedderCallback[] = "EmbedderCallback";
const char kCallbackNameArgument[] = "callbackName";
}  // namespace internal

class ScopedEmbedderCallbackTask {
 public:
  ScopedEmbedderCallbackTask(const char* callback_name) {
    TRACE_EVENT_BEGIN1(internal::kCategory,
                       internal::kEmbedderCallback,
                       internal::kCallbackNameArgument,
                       callback_name);
  }
  ~ScopedEmbedderCallbackTask() {
    TRACE_EVENT_END0(internal::kCategory,
                     internal::kEmbedderCallback);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ScopedEmbedderCallbackTask);
};

}  // namespace devtools_instrumentation
}  // namespace android_webview

#endif  // ANDROID_WEBVIEW_COMMON_DEVTOOLS_INSTRUMENTATION_H_

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/serial/async_waiter.h"

namespace device {

AsyncWaiter::AsyncWaiter(mojo::Handle handle,
                         MojoHandleSignals signals,
                         const Callback& callback)
    : waiter_(mojo::Environment::GetDefaultAsyncWaiter()),
      id_(0),
      callback_(callback) {
  id_ = waiter_->AsyncWait(handle.value(),
                           signals,
                           MOJO_DEADLINE_INDEFINITE,
                           &AsyncWaiter::WaitComplete,
                           this);
}

AsyncWaiter::~AsyncWaiter() {
  if (id_)
    waiter_->CancelWait(id_);
}

// static
void AsyncWaiter::WaitComplete(void* waiter, MojoResult result) {
  static_cast<AsyncWaiter*>(waiter)->WaitCompleteInternal(result);
}

void AsyncWaiter::WaitCompleteInternal(MojoResult result) {
  id_ = 0;
  callback_.Run(result);
}

}  // namespace device

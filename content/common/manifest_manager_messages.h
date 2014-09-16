// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// IPC messages for the web manifest manager.
// Multiply-included message file, hence no include guard.

#include "content/common/content_export.h"
#include "content/public/common/manifest.h"
#include "ipc/ipc_message_macros.h"

#undef IPC_MESSAGE_EXPORT
#define IPC_MESSAGE_EXPORT CONTENT_EXPORT

#define IPC_MESSAGE_START ManifestManagerMsgStart

IPC_STRUCT_TRAITS_BEGIN(content::Manifest)
  IPC_STRUCT_TRAITS_MEMBER(name)
  IPC_STRUCT_TRAITS_MEMBER(short_name)
IPC_STRUCT_TRAITS_END()

// The browser process requests for the manifest linked with the associated
// render frame. The renderer process will respond via a RequestManifestResponse
// IPC message with a Manifest object attached to it and the associated
// |request_id| that was initially given.
IPC_MESSAGE_ROUTED1(ManifestManagerMsg_RequestManifest,
                    int /* request_id */)

// The renderer process' response to a RequestManifest. The |request_id| will
// match the one that was initially received. The |manifest| object will be an
// empty manifest in case of any failure.
IPC_MESSAGE_ROUTED2(ManifestManagerHostMsg_RequestManifestResponse,
                    int, /* request_id */
                    content::Manifest /* manifest */)
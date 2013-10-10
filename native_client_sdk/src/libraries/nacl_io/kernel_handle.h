// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIBRARIES_NACL_IO_KERNEL_HANDLE_H_
#define LIBRARIES_NACL_IO_KERNEL_HANDLE_H_

#include <fcntl.h>
#include <pthread.h>

#include "nacl_io/error.h"
#include "nacl_io/mount.h"
#include "nacl_io/mount_node.h"
#include "nacl_io/ostypes.h"

#include "sdk_util/macros.h"
#include "sdk_util/ref_object.h"
#include "sdk_util/scoped_ref.h"
#include "sdk_util/simple_lock.h"

namespace nacl_io {

class MountNodeSocket;

// HandleAttr struct is passed the MountNode in calls
// to Read and Write.  It contains handle specific state
// such as the file offset and the open flags.
struct HandleAttr {
  HandleAttr() : offs(0), flags(0) {}
  bool IsBlocking() const { return !(flags & O_NONBLOCK); }

  size_t offs;
  int flags;
};

// KernelHandle provides a reference counted container for the open
// file information, such as it's mount, node, access type and offset.
// KernelHandle can only be referenced when the KernelProxy lock is held.
class KernelHandle : public sdk_util::RefObject {
 public:

  KernelHandle();
  KernelHandle(const ScopedMount& mnt, const ScopedMountNode& node);
  ~KernelHandle();

  Error Init(int open_flags);

  // Assumes |out_offset| is non-NULL.
  Error Seek(off_t offset, int whence, off_t* out_offset);

  // Dispatches Read, Write, GetDents to atomically update offs_.
  Error Read(void* buf, size_t nbytes, int* bytes_read);
  Error Write(const void* buf, size_t nbytes, int* bytes_written);
  Error GetDents(struct dirent* pdir, size_t count, int* bytes_written);
  Error Fcntl(int request, int* result, ...);
  Error VFcntl(int request, int* result, va_list args);

  const ScopedMountNode& node() { return node_; }
  const ScopedMount& mount() { return mount_; }

  // Returns the MountNodeSocket* if this node is a socket otherwise returns
  // NULL.
  MountNodeSocket* socket_node();

  const HandleAttr& Data() { return handle_data_; }
private:
  ScopedMount mount_;
  ScopedMountNode node_;
  sdk_util::SimpleLock handle_lock_;
  HandleAttr handle_data_;

  friend class KernelProxy;
  DISALLOW_COPY_AND_ASSIGN(KernelHandle);
};

typedef sdk_util::ScopedRef<KernelHandle> ScopedKernelHandle;

}  // namespace nacl_io

#endif  // LIBRARIES_NACL_IO_KERNEL_HANDLE_H_

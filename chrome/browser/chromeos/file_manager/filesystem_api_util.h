// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// The file contains utility functions to implement chrome.fileSystem API for
// file paths that do not directly map to host machine's file system path, such
// as Google Drive or virtual volumes provided by fileSystemProvider extensions.

#ifndef CHROME_BROWSER_CHROMEOS_FILE_MANAGER_FILESYSTEM_API_UTIL_H_
#define CHROME_BROWSER_CHROMEOS_FILE_MANAGER_FILESYSTEM_API_UTIL_H_

#include <string>
#include <vector>

#include "base/callback_forward.h"

class Profile;

namespace base {
class FilePath;
}  // namespace base

namespace fileapi {
class FileSystemURL;
}  // namespace fileapi

namespace file_manager {
namespace util {

// Checks whether the given |path| points to a non-local filesystem that
// requires special handling.
bool IsUnderNonNativeLocalPath(Profile* profile, const base::FilePath& path);

// Returns the mime type of the file pointed by |path|, and asynchronously sends
// the result to |callback|.
void GetNonNativeLocalPathMimeType(
    Profile* profile,
    const base::FilePath& path,
    const base::Callback<void(bool, const std::string&)>& callback);

// Checks whether the |path| points to a directory, and asynchronously sends
// the result to |callback|.
void IsNonNativeLocalPathDirectory(
    Profile* profile,
    const base::FilePath& path,
    const base::Callback<void(bool)>& callback);

// Ensures a file exists at |path|, i.e., it does nothing if a file is already
// present, or creates a file there if it isn't, and asynchronously sends to
// |callback| whether it succeeded.
void PrepareNonNativeLocalFileForWritableApp(
    Profile* profile,
    const base::FilePath& path,
    const base::Callback<void(bool)>& callback);

}  // namespace util
}  // namespace file_manager

#endif  // CHROME_BROWSER_CHROMEOS_FILE_MANAGER_FILESYSTEM_API_UTIL_H_

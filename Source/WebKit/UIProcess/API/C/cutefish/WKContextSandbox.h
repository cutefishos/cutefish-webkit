// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKContextSandbox_h
#define WKContextSandbox_h

#include <WebKit/WKBase.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Web processes run in a bubblewrap sandbox unless it is turned off. Both
// setters must be called before the context starts its first process; a
// later call is a fatal error. The caller chooses what the web processes may
// read or write besides what the sandbox always grants: each path is mounted
// at the same place, and a path that does not exist is skipped.
WK_EXPORT void WKContextSetSandboxEnabled(WKContextRef, bool enabled);
WK_EXPORT bool WKContextGetSandboxEnabled(WKContextRef);
WK_EXPORT void WKContextAddPathToSandbox(WKContextRef, const char* path, bool readOnly);

#ifdef __cplusplus
}
#endif

#endif // WKContextSandbox_h

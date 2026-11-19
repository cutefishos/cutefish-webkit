// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKWebsiteDataRecords_h
#define WKWebsiteDataRecords_h

#include <WebKit/WKBase.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Kinds of website data, combined as bits.
enum {
    kWKWebsiteDataCookies = 1 << 0,
    kWKWebsiteDataDiskCache = 1 << 1,
    kWKWebsiteDataMemoryCache = 1 << 2,
    kWKWebsiteDataSessionStorage = 1 << 4,
    kWKWebsiteDataLocalStorage = 1 << 5,
    kWKWebsiteDataWebSQLDatabases = 1 << 6,
    kWKWebsiteDataIndexedDBDatabases = 1 << 7,
    kWKWebsiteDataMediaKeys = 1 << 8,
    kWKWebsiteDataHSTSCache = 1 << 9,
    kWKWebsiteDataSearchFieldRecentSearches = 1 << 10,
    kWKWebsiteDataServiceWorkerRegistrations = 1 << 14,
    kWKWebsiteDataDOMCache = 1 << 15,
    kWKWebsiteDataFileSystem = 1 << 19,
};

// The data a store holds for one site, named as a person would read it
// (a host name or registrable domain), with the kinds it has.
typedef struct WKWebsiteDataRecord {
    const char* displayName;
    uint32_t types;
} WKWebsiteDataRecord;

// The records live until the function returns.
typedef void (*WKWebsiteDataStoreFetchDataRecordsFunction)(const WKWebsiteDataRecord* records, size_t count, void* context);
WK_EXPORT void WKWebsiteDataStoreFetchDataRecords(WKWebsiteDataStoreRef, uint32_t types, void* context, WKWebsiteDataStoreFetchDataRecordsFunction);

// Removes the kinds of data of the sites with these display names.
typedef void (*WKWebsiteDataStoreRemoveDataRecordsFunction)(void* context);
WK_EXPORT void WKWebsiteDataStoreRemoveDataRecords(WKWebsiteDataStoreRef, uint32_t types, const char* const* displayNames, size_t count, void* context, WKWebsiteDataStoreRemoveDataRecordsFunction);

#ifdef __cplusplus
}
#endif

#endif // WKWebsiteDataRecords_h

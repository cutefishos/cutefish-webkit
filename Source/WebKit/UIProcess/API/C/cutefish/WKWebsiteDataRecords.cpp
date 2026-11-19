// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKWebsiteDataRecords.h"

#include "WKAPICast.h"
#include "WebsiteDataFetchOption.h"
#include "WebsiteDataRecord.h"
#include "WebsiteDataStore.h"
#include <wtf/HashSet.h>
#include <wtf/text/CString.h>

static constexpr uint32_t knownTypes = kWKWebsiteDataCookies | kWKWebsiteDataDiskCache | kWKWebsiteDataMemoryCache
    | kWKWebsiteDataSessionStorage | kWKWebsiteDataLocalStorage | kWKWebsiteDataWebSQLDatabases
    | kWKWebsiteDataIndexedDBDatabases | kWKWebsiteDataMediaKeys | kWKWebsiteDataHSTSCache
    | kWKWebsiteDataSearchFieldRecentSearches | kWKWebsiteDataServiceWorkerRegistrations | kWKWebsiteDataDOMCache
    | kWKWebsiteDataFileSystem;

static OptionSet<WebKit::WebsiteDataType> dataTypes(uint32_t types)
{
    return OptionSet<WebKit::WebsiteDataType>::fromRaw(types & knownTypes);
}

void WKWebsiteDataStoreFetchDataRecords(WKWebsiteDataStoreRef dataStoreRef, uint32_t types, void* context, WKWebsiteDataStoreFetchDataRecordsFunction function)
{
    protect(WebKit::toImpl(dataStoreRef))->fetchData(dataTypes(types), { }, [context, function](Vector<WebKit::WebsiteDataRecord> records) {
        Vector<CString> names;
        Vector<WKWebsiteDataRecord> results;
        names.reserveInitialCapacity(records.size());
        for (auto& record : records)
            names.append(record.displayName.utf8());
        for (size_t i = 0; i < records.size(); ++i)
            results.append({ names[i].data(), records[i].types.toRaw() });
        function(results.span().data(), results.size(), context);
    });
}

void WKWebsiteDataStoreRemoveDataRecords(WKWebsiteDataStoreRef dataStoreRef, uint32_t types, const char* const* displayNames, size_t count, void* context, WKWebsiteDataStoreRemoveDataRecordsFunction function)
{
    HashSet<String> wanted;
    for (size_t i = 0; i < count; ++i)
        wanted.add(String::fromUTF8(displayNames[i]));
    Ref dataStore = *WebKit::toImpl(dataStoreRef);
    auto removedTypes = dataTypes(types);
    dataStore->fetchData(removedTypes, { }, [dataStore, removedTypes, wanted = WTF::move(wanted), context, function](Vector<WebKit::WebsiteDataRecord> records) {
        Vector<WebKit::WebsiteDataRecord> chosen;
        for (auto& record : records) {
            if (wanted.contains(record.displayName))
                chosen.append(WTF::move(record));
        }
        dataStore->removeData(removedTypes, chosen, [context, function] {
            function(context);
        });
    });
}

/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Portions Copyright (c) 2011 Motorola Mobility, Inc. All rights reserved.
 * Copyright (C) 2026 Reion Wong <reionwong@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY MOTOROLA INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MOTOROLA INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebProcess.h"

#include "SystemSettingsManager.h"
#include "WebPage.h"
#include "WebProcessCreationParameters.h"
#include <WebCore/FontRenderOptions.h>
#include <WebCore/MemoryCache.h>
#include <WebCore/PlatformDisplaySurfaceless.h>
#include <WebCore/PlatformScreen.h>
#include <WebCore/ScreenProperties.h>
#include <wtf/SetForScope.h>

#if USE(GBM)
#include <WebCore/DRMDeviceManager.h>
#include <WebCore/GBMDevice.h>
#include <WebCore/PlatformDisplayGBM.h>
#endif

#if HAVE(MALLOC_TRIM)
#include <malloc.h>
#endif

#if OS(LINUX)
#include <wtf/linux/HighPriorityThreads.h>
#endif

namespace WebKit {

using namespace WebCore;

void WebProcess::stopRunLoop()
{
    // Closing the last page terminates the process, which calls back here.
    if (m_isStoppingRunLoop)
        return;
    SetForScope isStoppingRunLoop(m_isStoppingRunLoop, true);

    // The connection can close before the Close message is processed, so close
    // any pending page here to invalidate the threaded compositor and release
    // its GL resources.
    for (auto& webPage : copyToVector(m_pageMap.values()))
        webPage->close([] { });

    PlatformDisplay::destroySharedDisplay();

    AuxiliaryProcess::stopRunLoop();
}

void WebProcess::platformSetCacheModel(CacheModel cacheModel)
{
    WebCore::MemoryCache::singleton().setDisabled(cacheModel == CacheModel::DocumentViewer);
}

void WebProcess::platformInitializeProcess(const AuxiliaryProcessInitializationParameters&)
{
#if OS(LINUX)
    // Disable HighPriorityThreads in WebProcess initially, since it depends on having a visible web page.
    HighPriorityThreads::singleton().setEnabled(false);
#endif

    addSupplementWithoutRefCountedCheck<SystemSettingsManager>();
}

void WebProcess::initializePlatformDisplayIfNeeded() const
{
    if (PlatformDisplay::sharedDisplayIfExists())
        return;

#if USE(GBM)
    if (m_rendererBufferTransportMode.contains(RendererBufferTransportMode::Hardware)) {
        if (auto device = DRMDeviceManager::singleton().mainGBMDevice(DRMDeviceManager::NodeType::Render)) {
            PlatformDisplay::setSharedDisplay(PlatformDisplayGBM::create(device->device()));
            return;
        }
    }
#endif

    if (auto display = PlatformDisplaySurfaceless::create()) {
        PlatformDisplay::setSharedDisplay(WTF::move(display));
        return;
    }

    WTFLogAlways("Could not create EGL display: no supported platform available. Aborting...");
    CRASH();
}

void WebProcess::initializeVulkanIfNeeded()
{
}

void WebProcess::platformInitializeWebProcess(WebProcessCreationParameters& parameters)
{
#if USE(GBM)
    DRMDeviceManager::singleton().initializeMainDevice(WTF::move(parameters.drmDevice));
#endif

    m_rendererBufferTransportMode = parameters.rendererBufferTransportMode;
    initializePlatformDisplayIfNeeded();

    m_availableInputDevices = parameters.availableInputDevices;

    if (parameters.memoryPressureHandlerConfiguration)
        MemoryPressureHandler::singleton().setConfiguration(WTF::move(*parameters.memoryPressureHandlerConfiguration));

    if (parameters.disableFontHintingForTesting)
        FontRenderOptions::singleton().disableHintingForTesting();

    WebCore::PlatformScreen::updateSingletonProperties(WTF::move(parameters.screenProperties));
}

void WebProcess::platformSetWebsiteDataStoreParameters(WebProcessDataStoreParameters&&)
{
}

void WebProcess::platformTerminate()
{
}

void WebProcess::grantAccessToAssetServices(Vector<WebKit::SandboxExtension::Handle>&&)
{
}

void WebProcess::revokeAccessToAssetServices()
{
}

void WebProcess::switchFromStaticFontRegistryToUserFontRegistry(Vector<WebKit::SandboxExtension::Handle>&&)
{
}

void WebProcess::releaseSystemMallocMemory()
{
#if HAVE(MALLOC_TRIM)
    malloc_trim(0);
#endif
}

void WebProcess::setScreenProperties(WebCore::ScreenProperties&& properties)
{
    WebCore::PlatformScreen::updateSingletonProperties(WTF::move(properties));
    for (auto& page : m_pageMap.values())
        page->screenPropertiesDidChange();
}

std::optional<AvailableInputDevices> WebProcess::primaryPointingDevice() const
{
    if (m_availableInputDevices.contains(AvailableInputDevices::Mouse))
        return AvailableInputDevices::Mouse;
    if (m_availableInputDevices.contains(AvailableInputDevices::Touchscreen))
        return AvailableInputDevices::Touchscreen;
    return std::nullopt;
}

void WebProcess::setAvailableInputDevices(OptionSet<AvailableInputDevices> availableInputDevices)
{
    m_availableInputDevices = availableInputDevices;
}

} // namespace WebKit

/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Portions Copyright (c) 2010 Motorola Mobility, Inc. All rights reserved.
 * Copyright (C) 2012 Samsung Electronics Ltd. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebProcessPool.h"

#include "DRMMainDevice.h"
#include "LegacyGlobalSettings.h"
#include "MemoryPressureMonitor.h"
#include "NetworkProcessCreationParameters.h"
#include "WebMemoryPressureHandler.h"
#include "WebProcessCreationParameters.h"
#include <WebCore/NotImplemented.h>
#include <WebCore/SystemSettings.h>

namespace WebKit {

void WebProcessPool::platformInitialize(NeedsGlobalStaticInitialization)
{
#if OS(LINUX)
    if (!MemoryPressureMonitor::disabled())
        installMemoryPressureHandler();
#endif
}

void WebProcessPool::platformInitializeNetworkProcess(NetworkProcessCreationParameters&)
{
}

void WebProcessPool::platformInitializeWebProcess(const WebProcessProxy&, WebProcessCreationParameters& parameters)
{
#if USE(GBM)
    parameters.drmDevice = drmMainDevice();
    if (!parameters.drmDevice.isNull())
        parameters.rendererBufferTransportMode.add(RendererBufferTransportMode::Hardware);
#endif
    parameters.rendererBufferTransportMode.add(RendererBufferTransportMode::SharedMemory);

    parameters.availableInputDevices = { AvailableInputDevices::Mouse, AvailableInputDevices::Keyboard };
    parameters.memoryCacheDisabled = m_memoryCacheDisabled || LegacyGlobalSettings::singleton().cacheModel() == CacheModel::DocumentViewer;

#if OS(LINUX)
    if (MemoryPressureMonitor::disabled())
        parameters.shouldSuppressMemoryPressureHandler = true;
#endif

    parameters.memoryPressureHandlerConfiguration = m_configuration->memoryPressureHandlerConfiguration();
    parameters.disableFontHintingForTesting = m_configuration->disableFontHintingForTesting();
    parameters.systemSettings = WebCore::SystemSettings::singleton().settingsState();
}

void WebProcessPool::platformInvalidateContext()
{
}

void WebProcessPool::platformResolvePathsForSandboxExtensions()
{
}

} // namespace WebKit

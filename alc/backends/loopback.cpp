/**
 * OpenAL cross platform audio library
 * Copyright (C) 2011 by Chris Robinson
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "config.h"

#include "loopback.h"

#include "core/device.h"


namespace {

struct LoopbackBackend final : public BackendBase {
    explicit LoopbackBackend(gsl::strict_not_null<DeviceBase*> device) noexcept
        : BackendBase{device}
    { }

    void open(std::string_view name) override;
    bool reset() override;
    void start() override;
    void stop() override;
};


void LoopbackBackend::open(std::string_view name)
{
    mDeviceName = name;
}

bool LoopbackBackend::reset()
{
    setDefaultWFXChannelOrder();
    return true;
}

void LoopbackBackend::start()
{ }

void LoopbackBackend::stop()
{ }

} // namespace


bool LoopbackBackendFactory::init()
{ return true; }

bool LoopbackBackendFactory::querySupport(BackendType)
{ return true; }

auto LoopbackBackendFactory::enumerate(BackendType) -> std::vector<std::string>
{ return {}; }

auto LoopbackBackendFactory::createBackend(gsl::strict_not_null<DeviceBase*> device, BackendType)
    -> BackendPtr
{ return BackendPtr{new LoopbackBackend{device}}; }

BackendFactory &LoopbackBackendFactory::getFactory()
{
    static LoopbackBackendFactory factory{};
    return factory;
}

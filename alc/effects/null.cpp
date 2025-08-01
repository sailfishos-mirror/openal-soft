
#include "config.h"

#include <cstddef>
#include <span>

#include "base.h"
#include "core/bufferline.h"
#include "core/effects/base.h"
#include "intrusive_ptr.h"

struct BufferStorage;
struct ContextBase;
struct DeviceBase;
struct EffectSlot;


namespace {

struct NullState final : public EffectState {
    NullState();
    ~NullState() override;

    void deviceUpdate(const DeviceBase *device, const BufferStorage *buffer) override;
    void update(const ContextBase *context, const EffectSlot *slot, const EffectProps *props,
        const EffectTarget target) override;
    void process(const size_t samplesToDo, const std::span<const FloatBufferLine> samplesIn,
        const std::span<FloatBufferLine> samplesOut) override;
};

/* This constructs the effect state. It's called when the object is first
 * created.
 */
NullState::NullState() = default;

/* This destructs the effect state. It's called only when the effect instance
 * is no longer used.
 */
NullState::~NullState() = default;

/* This updates the device-dependant effect state. This is called on state
 * initialization and any time the device parameters (e.g. playback frequency,
 * format) have been changed. Will always be followed by a call to the update
 * method, if successful.
 */
void NullState::deviceUpdate(const DeviceBase* /*device*/, const BufferStorage* /*buffer*/)
{
}

/* This updates the effect state with new properties. This is called any time
 * the effect is (re)loaded into a slot.
 */
void NullState::update(const ContextBase* /*context*/, const EffectSlot* /*slot*/,
    const EffectProps* /*props*/, const EffectTarget /*target*/)
{
}

/* This processes the effect state, for the given number of samples from the
 * input to the output buffer. The result should be added to the output buffer,
 * not replace it.
 */
void NullState::process(const size_t/*samplesToDo*/,
    const std::span<const FloatBufferLine> /*samplesIn*/,
    const std::span<FloatBufferLine> /*samplesOut*/)
{
}


struct NullStateFactory final : public EffectStateFactory {
    al::intrusive_ptr<EffectState> create() override;
};

/* Creates EffectState objects of the appropriate type. */
al::intrusive_ptr<EffectState> NullStateFactory::create()
{ return al::intrusive_ptr<EffectState>{new NullState{}}; }

} // namespace

auto NullStateFactory_getFactory() -> gsl::strict_not_null<EffectStateFactory*>
{
    static NullStateFactory NullFactory{};
    return gsl::make_not_null(&NullFactory);
}

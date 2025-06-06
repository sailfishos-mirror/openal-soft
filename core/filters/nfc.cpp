
#include "config.h"

#include "nfc.h"

#include <algorithm>


/* Near-field control filters are the basis for handling the near-field effect.
 * The near-field effect is a bass-boost present in the directional components
 * of a recorded signal, created as a result of the wavefront curvature (itself
 * a function of sound distance). Proper reproduction dictates this be
 * compensated for using a bass-cut given the playback speaker distance, to
 * avoid excessive bass in the playback.
 *
 * For real-time rendered audio, emulating the near-field effect based on the
 * sound source's distance, and subsequently compensating for it at output
 * based on the speaker distances, can create a more realistic perception of
 * sound distance beyond a simple 1/r attenuation.
 *
 * These filters do just that. Each one applies a low-shelf filter, created as
 * the combination of a bass-boost for a given sound source distance (near-
 * field emulation) along with a bass-cut for a given control/speaker distance
 * (near-field compensation).
 *
 * Note that it is necessary to apply a cut along with the boost, since the
 * boost alone is unstable in higher-order ambisonics as it causes an infinite
 * DC gain (even first-order ambisonics requires there to be no DC offset for
 * the boost to work). Consequently, ambisonics requires a control parameter to
 * be used to avoid an unstable boost-only filter. NFC-HOA defines this control
 * as a reference delay, calculated with:
 *
 * reference_delay = control_distance / speed_of_sound
 *
 * This means w0 (for input) or w1 (for output) should be set to:
 *
 * wN = 1 / (reference_delay * sample_rate)
 *
 * when dealing with NFC-HOA content. For FOA input content, which does not
 * specify a reference_delay variable, w0 should be set to 0 to apply only
 * near-field compensation for output. It's important that w1 be a finite,
 * positive, non-0 value or else the bass-boost will become unstable again.
 * Also, w0 should not be too large compared to w1, to avoid excessively loud
 * low frequencies.
 */

namespace {

constexpr auto B1 = std::array{   1.0f};
constexpr auto B2 = std::array{   3.0f,     3.0f};
constexpr auto B3 = std::array{3.6778f,  6.4595f, 2.3222f};
constexpr auto B4 = std::array{4.2076f, 11.4877f, 5.7924f, 9.1401f};

NfcFilter1 NfcFilterCreate1(const float w0, const float w1) noexcept
{
    auto nfc = NfcFilter1{};

    /* Calculate bass-cut coefficients. */
    auto r = 0.5f * w1;
    auto b_00 = B1[0] * r;
    auto g_0 = 1.0f + b_00;

    nfc.base_gain = 1.0f / g_0;
    nfc.a1 = 2.0f * b_00 / g_0;

    /* Calculate bass-boost coefficients. */
    r = 0.5f * w0;
    b_00 = B1[0] * r;
    g_0 = 1.0f + b_00;

    nfc.gain = nfc.base_gain * g_0;
    nfc.b1 = 2.0f * b_00 / g_0;

    return nfc;
}

void NfcFilterAdjust1(NfcFilter1 *nfc, const float w0) noexcept
{
    const auto r = 0.5f * w0;
    const auto b_00 = B1[0] * r;
    const auto g_0 = 1.0f + b_00;

    nfc->gain = nfc->base_gain * g_0;
    nfc->b1 = 2.0f * b_00 / g_0;
}


NfcFilter2 NfcFilterCreate2(const float w0, const float w1) noexcept
{
    auto nfc = NfcFilter2{};

    /* Calculate bass-cut coefficients. */
    auto r = 0.5f * w1;
    auto b_10 = B2[0] * r;
    auto b_11 = B2[1] * (r*r);
    auto g_1 = 1.0f + b_10 + b_11;

    nfc.base_gain = 1.0f / g_1;
    nfc.a1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc.a2 = 4.0f * b_11 / g_1;

    /* Calculate bass-boost coefficients. */
    r = 0.5f * w0;
    b_10 = B2[0] * r;
    b_11 = B2[1] * r * r;
    g_1 = 1.0f + b_10 + b_11;

    nfc.gain = nfc.base_gain * g_1;
    nfc.b1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc.b2 = 4.0f * b_11 / g_1;

    return nfc;
}

void NfcFilterAdjust2(NfcFilter2 *nfc, const float w0) noexcept
{
    const auto r = 0.5f * w0;
    const auto b_10 = B2[0] * r;
    const auto b_11 = B2[1] * (r*r);
    const auto g_1 = 1.0f + b_10 + b_11;

    nfc->gain = nfc->base_gain * g_1;
    nfc->b1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc->b2 = 4.0f * b_11 / g_1;
}


NfcFilter3 NfcFilterCreate3(const float w0, const float w1) noexcept
{
    auto nfc = NfcFilter3{};

    /* Calculate bass-cut coefficients. */
    auto r = 0.5f * w1;
    auto b_10 = B3[0] * r;
    auto b_11 = B3[1] * (r*r);
    auto b_00 = B3[2] * r;
    auto g_1 = 1.0f + b_10 + b_11;
    auto g_0 = 1.0f + b_00;

    nfc.base_gain = 1.0f / (g_1 * g_0);
    nfc.a1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc.a2 = 4.0f * b_11 / g_1;
    nfc.a3 = 2.0f * b_00 / g_0;

    /* Calculate bass-boost coefficients. */
    r = 0.5f * w0;
    b_10 = B3[0] * r;
    b_11 = B3[1] * (r*r);
    b_00 = B3[2] * r;
    g_1 = 1.0f + b_10 + b_11;
    g_0 = 1.0f + b_00;

    nfc.gain = nfc.base_gain * (g_1 * g_0);
    nfc.b1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc.b2 = 4.0f * b_11 / g_1;
    nfc.b3 = 2.0f * b_00 / g_0;

    return nfc;
}

void NfcFilterAdjust3(NfcFilter3 *nfc, const float w0) noexcept
{
    const auto r = 0.5f * w0;
    const auto b_10 = B3[0] * r;
    const auto b_11 = B3[1] * (r*r);
    const auto b_00 = B3[2] * r;
    const auto g_1 = 1.0f + b_10 + b_11;
    const auto g_0 = 1.0f + b_00;

    nfc->gain = nfc->base_gain * (g_1 * g_0);
    nfc->b1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc->b2 = 4.0f * b_11 / g_1;
    nfc->b3 = 2.0f * b_00 / g_0;
}


NfcFilter4 NfcFilterCreate4(const float w0, const float w1) noexcept
{
    auto nfc = NfcFilter4{};

    /* Calculate bass-cut coefficients. */
    auto r = 0.5f * w1;
    auto b_10 = B4[0] * r;
    auto b_11 = B4[1] * (r*r);
    auto b_00 = B4[2] * r;
    auto b_01 = B4[3] * (r*r);
    auto g_1 = 1.0f + b_10 + b_11;
    auto g_0 = 1.0f + b_00 + b_01;

    nfc.base_gain = 1.0f / (g_1 * g_0);
    nfc.a1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc.a2 = 4.0f * b_11 / g_1;
    nfc.a3 = (2.0f*b_00 + 4.0f*b_01) / g_0;
    nfc.a4 = 4.0f * b_01 / g_0;

    /* Calculate bass-boost coefficients. */
    r = 0.5f * w0;
    b_10 = B4[0] * r;
    b_11 = B4[1] * (r*r);
    b_00 = B4[2] * r;
    b_01 = B4[3] * (r*r);
    g_1 = 1.0f + b_10 + b_11;
    g_0 = 1.0f + b_00 + b_01;

    nfc.gain = nfc.base_gain * (g_1 * g_0);
    nfc.b1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc.b2 = 4.0f * b_11 / g_1;
    nfc.b3 = (2.0f*b_00 + 4.0f*b_01) / g_0;
    nfc.b4 = 4.0f * b_01 / g_0;

    return nfc;
}

void NfcFilterAdjust4(NfcFilter4 *nfc, const float w0) noexcept
{
    const auto r = 0.5f * w0;
    const auto b_10 = B4[0] * r;
    const auto b_11 = B4[1] * (r*r);
    const auto b_00 = B4[2] * r;
    const auto b_01 = B4[3] * (r*r);
    const auto g_1 = 1.0f + b_10 + b_11;
    const auto g_0 = 1.0f + b_00 + b_01;

    nfc->gain = nfc->base_gain * (g_1 * g_0);
    nfc->b1 = (2.0f*b_10 + 4.0f*b_11) / g_1;
    nfc->b2 = 4.0f * b_11 / g_1;
    nfc->b3 = (2.0f*b_00 + 4.0f*b_01) / g_0;
    nfc->b4 = 4.0f * b_01 / g_0;
}

} // namespace

void NfcFilter::init(const float w1) noexcept
{
    first = NfcFilterCreate1(0.0f, w1);
    second = NfcFilterCreate2(0.0f, w1);
    third = NfcFilterCreate3(0.0f, w1);
    fourth = NfcFilterCreate4(0.0f, w1);
}

void NfcFilter::adjust(const float w0) noexcept
{
    NfcFilterAdjust1(&first, w0);
    NfcFilterAdjust2(&second, w0);
    NfcFilterAdjust3(&third, w0);
    NfcFilterAdjust4(&fourth, w0);
}


void NfcFilter::process1(const std::span<const float> src, const std::span<float> dst)
{
    const auto gain = first.gain;
    const auto b1 = first.b1;
    const auto a1 = first.a1;
    auto z1 = first.z[0];
    std::ranges::transform(src, dst.begin(), [gain,b1,a1,&z1](const float in) noexcept -> float
    {
        const auto y = in*gain - a1*z1;
        const auto out = y + b1*z1;
        z1 += y;
        return out;
    });
    first.z[0] = z1;
}

void NfcFilter::process2(const std::span<const float> src, const std::span<float> dst)
{
    const auto gain = second.gain;
    const auto b1 = second.b1;
    const auto b2 = second.b2;
    const auto a1 = second.a1;
    const auto a2 = second.a2;
    auto z1 = second.z[0];
    auto z2 = second.z[1];
    std::ranges::transform(src, dst.begin(),
        [gain,b1,b2,a1,a2,&z1,&z2](const float in) noexcept -> float
    {
        const auto y = in*gain - a1*z1 - a2*z2;
        const auto out = y + b1*z1 + b2*z2;
        z2 += z1;
        z1 += y;
        return out;
    });
    second.z[0] = z1;
    second.z[1] = z2;
}

void NfcFilter::process3(const std::span<const float> src, const std::span<float> dst)
{
    const auto gain = third.gain;
    const auto b1 = third.b1;
    const auto b2 = third.b2;
    const auto b3 = third.b3;
    const auto a1 = third.a1;
    const auto a2 = third.a2;
    const auto a3 = third.a3;
    auto z1 = third.z[0];
    auto z2 = third.z[1];
    auto z3 = third.z[2];
    std::ranges::transform(src, dst.begin(),
        [gain,b1,b2,b3,a1,a2,a3,&z1,&z2,&z3](const float in) noexcept -> float
    {
        auto y = in*gain - a1*z1 - a2*z2;
        auto out = y + b1*z1 + b2*z2;
        z2 += z1;
        z1 += y;

        y = out - a3*z3;
        out = y + b3*z3;
        z3 += y;
        return out;
    });
    third.z[0] = z1;
    third.z[1] = z2;
    third.z[2] = z3;
}

void NfcFilter::process4(const std::span<const float> src, const std::span<float> dst)
{
    const auto gain = fourth.gain;
    const auto b1 = fourth.b1;
    const auto b2 = fourth.b2;
    const auto b3 = fourth.b3;
    const auto b4 = fourth.b4;
    const auto a1 = fourth.a1;
    const auto a2 = fourth.a2;
    const auto a3 = fourth.a3;
    const auto a4 = fourth.a4;
    auto z1 = fourth.z[0];
    auto z2 = fourth.z[1];
    auto z3 = fourth.z[2];
    auto z4 = fourth.z[3];
    std::ranges::transform(src, dst.begin(),
        [gain,b1,b2,b3,b4,a1,a2,a3,a4,&z1,&z2,&z3,&z4](const float in) noexcept -> float
    {
        auto y = in*gain - a1*z1 - a2*z2;
        auto out = y + b1*z1 + b2*z2;
        z2 += z1;
        z1 += y;

        y = out - a3*z3 - a4*z4;
        out = y + b3*z3 + b4*z4;
        z4 += z3;
        z3 += y;
        return out;
    });
    fourth.z[0] = z1;
    fourth.z[1] = z2;
    fourth.z[2] = z3;
    fourth.z[3] = z4;
}

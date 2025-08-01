#ifndef AL_NUMERIC_H
#define AL_NUMERIC_H

#include "config_simd.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>
#ifdef HAVE_INTRIN_H
#include <intrin.h>
#endif
#if HAVE_SSE_INTRINSICS
#include <emmintrin.h>
#endif

#include "gsl/gsl"
#include "opthelpers.h"


consteval auto operator "" _i64(unsigned long long n) noexcept { return gsl::narrow<std::int64_t>(n); }
consteval auto operator "" _u64(unsigned long long n) noexcept { return gsl::narrow<std::uint64_t>(n); }

consteval auto operator "" _z(unsigned long long n) noexcept
{ return gsl::narrow<std::make_signed_t<std::size_t>>(n); }
consteval auto operator "" _uz(unsigned long long n) noexcept { return gsl::narrow<std::size_t>(n); }
consteval auto operator "" _zu(unsigned long long n) noexcept { return gsl::narrow<std::size_t>(n); }


namespace al {

#if HAS_BUILTIN(__builtin_add_overflow)
template<std::integral T>
constexpr auto add_sat(T a, T b) -> T
{
    T c;
    if(!__builtin_add_overflow(a, b, &c))
        return c;
    if constexpr(std::is_signed_v<T>)
    {
        if(b < 0)
            return std::numeric_limits<T>::min();
    }
    return std::numeric_limits<T>::max();
}

#else

template<std::integral T>
constexpr auto add_sat(T a, T b) -> T
{
    if constexpr(std::is_signed_v<T>)
    {
        if(b < 0)
        {
            if(a < std::numeric_limits<T>::min()-b)
                return std::numeric_limits<T>::min();
            return a + b;
        }
        if(a > std::numeric_limits<T>::max()-b)
            return std::numeric_limits<T>::max();
        return a + b;
    }
    else
    {
        const auto c = a + b;
        if(c < a)
            return std::numeric_limits<T>::max();
        return c;
    }
}
#endif

} /* namespace al */

template<std::integral T>
constexpr auto as_unsigned(T value) noexcept
{
    using UT = std::make_unsigned_t<T>;
    return static_cast<UT>(value);
}

template<std::integral T>
constexpr auto as_signed(T value) noexcept
{
    using ST = std::make_signed_t<T>;
    return static_cast<ST>(value);
}


constexpr auto GetCounterSuffix(size_t count) noexcept -> std::string_view
{
    using namespace std::string_view_literals;
    return (((count%100)/10) == 1) ? "th"sv :
        ((count%10) == 1) ? "st"sv :
        ((count%10) == 2) ? "nd"sv :
        ((count%10) == 3) ? "rd"sv : "th"sv;
}


constexpr auto lerpf(float val1, float val2, float mu) noexcept -> float
{ return val1 + (val2-val1)*mu; }


/** Find the next power-of-2 for non-power-of-2 numbers. */
constexpr auto NextPowerOf2(uint32_t value) noexcept -> uint32_t
{
    if(value > 0)
    {
        value--;
        value |= value>>1;
        value |= value>>2;
        value |= value>>4;
        value |= value>>8;
        value |= value>>16;
    }
    return value+1;
}

/**
 * If the value is not already a multiple of r, round toward zero to the next
 * multiple.
 */
template<std::integral T>
constexpr auto RoundToZero(T value, std::type_identity_t<T> r) noexcept -> T
{ return value - (value%r); }

/**
 * If the value is not already a multiple of r, round away from zero to the
 * next multiple.
 */
template<std::integral T>
constexpr auto RoundFromZero(T value, std::type_identity_t<T> r) noexcept -> T
{
    if(value >= 0)
        return RoundToZero(value + r-1, r);
    return RoundToZero(value - r+1, r);
}


/**
 * Fast float-to-int conversion. No particular rounding mode is assumed; the
 * IEEE-754 default is round-to-nearest with ties-to-even, though an app could
 * change it on its own threads. On some systems, a truncating conversion may
 * always be the fastest method.
 */
inline int fastf2i(float f) noexcept
{
#if HAVE_SSE_INTRINSICS
    return _mm_cvt_ss2si(_mm_set_ss(f));

#elif defined(_MSC_VER) && defined(_M_IX86_FP) && _M_IX86_FP == 0

    int i;
    __asm fld f
    __asm fistp i
    return i;

#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__)) \
    && !defined(__SSE_MATH__)

    int i;
    __asm__ __volatile__("fistpl %0" : "=m"(i) : "t"(f) : "st");
    return i;

#else

    return gsl::narrow_cast<int>(f);
#endif
}
inline unsigned int fastf2u(float f) noexcept
{ return gsl::narrow_cast<unsigned int>(fastf2i(f)); }

/** Converts float-to-int using standard behavior (truncation). */
inline int float2int(float f) noexcept
{
#if HAVE_SSE_INTRINSICS
    return _mm_cvtt_ss2si(_mm_set_ss(f));

#elif (defined(_MSC_VER) && defined(_M_IX86_FP) && _M_IX86_FP == 0) \
    || ((defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__)) \
        && !defined(__SSE_MATH__))
    const auto conv_i = std::bit_cast<int>(f);

    const auto sign = (conv_i>>31) | 1;
    const auto shift = ((conv_i>>23)&0xff) - (127+23);

    /* Over/underflow */
    if(shift >= 31 || shift < -23) [[unlikely]]
        return 0;

    const auto mant = (conv_i&0x7fffff) | 0x800000;
    if(shift < 0) [[likely]]
        return (mant >> -shift) * sign;
    return (mant << shift) * sign;

#else

    return gsl::narrow_cast<int>(f);
#endif
}
inline unsigned int float2uint(float f) noexcept
{ return gsl::narrow_cast<unsigned int>(float2int(f)); }

/** Converts double-to-int using standard behavior (truncation). */
inline int double2int(double d) noexcept
{
#if HAVE_SSE_INTRINSICS
    return _mm_cvttsd_si32(_mm_set_sd(d));

#elif (defined(_MSC_VER) && defined(_M_IX86_FP) && _M_IX86_FP < 2) \
    || ((defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__)) \
        && !defined(__SSE2_MATH__))
    const auto conv_i64 = std::bit_cast<int64_t>(d);

    const auto sign = gsl::narrow_cast<int>(conv_i64 >> 63) | 1;
    const auto shift = (gsl::narrow_cast<int>(conv_i64 >> 52) & 0x7ff) - (1023 + 52);

    /* Over/underflow */
    if(shift >= 63 || shift < -52) [[unlikely]]
        return 0;

    const auto mant = (conv_i64 & 0xfffffffffffff_i64) | 0x10000000000000_i64;
    if(shift < 0) [[likely]]
        return gsl::narrow_cast<int>(mant >> -shift) * sign;
    return gsl::narrow_cast<int>(mant << shift) * sign;

#else

    return gsl::narrow_cast<int>(d);
#endif
}

/**
 * Rounds a float to the nearest integral value, according to the current
 * rounding mode. This is essentially an inlined version of rintf, although
 * makes fewer promises (e.g. -0 or -0.25 rounded to 0 may result in +0).
 */
inline float fast_roundf(float f) noexcept
{
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__)) \
    && !defined(__SSE_MATH__)

    float out;
    __asm__ __volatile__("frndint" : "=t"(out) : "0"(f));
    return out;

#elif (defined(__GNUC__) || defined(__clang__)) && defined(__aarch64__)

    float out;
    __asm__ volatile("frintx %s0, %s1" : "=w"(out) : "w"(f));
    return out;

#else

    /* Integral limit, where sub-integral precision is not available for
     * floats.
     */
    static constexpr auto ilim = std::array{
         8388608.0f /*  0x1.0p+23 */,
        -8388608.0f /* -0x1.0p+23 */
    };
    const auto conv_u = std::bit_cast<unsigned int>(f);

    const auto sign = (conv_u>>31u)&0x01u;
    const auto expo = (conv_u>>23u)&0xffu;

    if(expo >= 150/*+23*/) [[unlikely]]
    {
        /* An exponent (base-2) of 23 or higher is incapable of sub-integral
         * precision, so it's already an integral value. We don't need to worry
         * about infinity or NaN here.
         */
        return f;
    }
    /* Adding the integral limit to the value (with a matching sign) forces a
     * result that has no sub-integral precision, and is consequently forced to
     * round to an integral value. Removing the integral limit then restores
     * the initial value rounded to the integral. The compiler should not
     * optimize this out because of non-associative rules on floating-point
     * math (as long as you don't use -fassociative-math,
     * -funsafe-math-optimizations, -ffast-math, or -Ofast, in which case this
     * may break without __builtin_assoc_barrier support).
     */
#if HAS_BUILTIN(__builtin_assoc_barrier)
    return __builtin_assoc_barrier(f + ilim[sign]) - ilim[sign];
#else
    f += ilim[sign];
    return f - ilim[sign];
#endif
#endif
}


// Converts level (mB) to gain.
inline float level_mb_to_gain(float x)
{
    if(x <= -10'000.0f)
        return 0.0f;
    return std::pow(10.0f, x / 2'000.0f);
}

// Converts gain to level (mB).
inline float gain_to_level_mb(float x)
{
    if(x <= 1e-05f)
        return -10'000.0f;
    return std::max(std::log10(x) * 2'000.0f, -10'000.0f);
}

#endif /* AL_NUMERIC_H */

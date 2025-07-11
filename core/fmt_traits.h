#ifndef CORE_FMT_TRAITS_H
#define CORE_FMT_TRAITS_H

#include <array>
#include <cstddef>
#include <cstdint>

#include "gsl/gsl"
#include "storage_formats.h"


inline constexpr auto muLawDecompressionTable = std::array<int16_t,256>{{
    -32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956,
    -23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764,
    -15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412,
    -11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316,
    -7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140,
    -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092,
    -3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004,
    -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980,
    -1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436,
    -1372, -1308, -1244, -1180, -1116, -1052,  -988,  -924,
    -876,  -844,  -812,  -780,  -748,  -716,  -684,  -652,
    -620,  -588,  -556,  -524,  -492,  -460,  -428,  -396,
    -372,  -356,  -340,  -324,  -308,  -292,  -276,  -260,
    -244,  -228,  -212,  -196,  -180,  -164,  -148,  -132,
    -120,  -112,  -104,   -96,   -88,   -80,   -72,   -64,
    -56,   -48,   -40,   -32,   -24,   -16,    -8,     0,
    32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956,
    23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764,
    15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412,
    11900, 11388, 10876, 10364,  9852,  9340,  8828,  8316,
    7932,  7676,  7420,  7164,  6908,  6652,  6396,  6140,
    5884,  5628,  5372,  5116,  4860,  4604,  4348,  4092,
    3900,  3772,  3644,  3516,  3388,  3260,  3132,  3004,
    2876,  2748,  2620,  2492,  2364,  2236,  2108,  1980,
    1884,  1820,  1756,  1692,  1628,  1564,  1500,  1436,
    1372,  1308,  1244,  1180,  1116,  1052,   988,   924,
    876,   844,   812,   780,   748,   716,   684,   652,
    620,   588,   556,   524,   492,   460,   428,   396,
    372,   356,   340,   324,   308,   292,   276,   260,
    244,   228,   212,   196,   180,   164,   148,   132,
    120,   112,   104,    96,    88,    80,    72,    64,
    56,    48,    40,    32,    24,    16,     8,     0
}};

inline constexpr auto aLawDecompressionTable = std::array<int16_t,256>{{
    -5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736,
    -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784,
    -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368,
    -3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392,
    -22016,-20992,-24064,-23040,-17920,-16896,-19968,-18944,
    -30208,-29184,-32256,-31232,-26112,-25088,-28160,-27136,
    -11008,-10496,-12032,-11520, -8960, -8448, -9984, -9472,
    -15104,-14592,-16128,-15616,-13056,-12544,-14080,-13568,
    -344,  -328,  -376,  -360,  -280,  -264,  -312,  -296,
    -472,  -456,  -504,  -488,  -408,  -392,  -440,  -424,
    -88,   -72,  -120,  -104,   -24,    -8,   -56,   -40,
    -216,  -200,  -248,  -232,  -152,  -136,  -184,  -168,
    -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184,
    -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696,
    -688,  -656,  -752,  -720,  -560,  -528,  -624,  -592,
    -944,  -912, -1008,  -976,  -816,  -784,  -880,  -848,
    5504,  5248,  6016,  5760,  4480,  4224,  4992,  4736,
    7552,  7296,  8064,  7808,  6528,  6272,  7040,  6784,
    2752,  2624,  3008,  2880,  2240,  2112,  2496,  2368,
    3776,  3648,  4032,  3904,  3264,  3136,  3520,  3392,
    22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944,
    30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136,
    11008, 10496, 12032, 11520,  8960,  8448,  9984,  9472,
    15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568,
    344,   328,   376,   360,   280,   264,   312,   296,
    472,   456,   504,   488,   408,   392,   440,   424,
    88,    72,   120,   104,    24,     8,    56,    40,
    216,   200,   248,   232,   152,   136,   184,   168,
    1376,  1312,  1504,  1440,  1120,  1056,  1248,  1184,
    1888,  1824,  2016,  1952,  1632,  1568,  1760,  1696,
    688,   656,   752,   720,   560,   528,   624,   592,
    944,   912,  1008,   976,   816,   784,   880,   848
}};


struct MulawSample { uint8_t value; };
struct AlawSample { uint8_t value; };
struct IMA4Data { std::byte value; };
struct MSADPCMData { std::byte value; };

template<typename T>
struct SampleInfo;

template<>
struct SampleInfo<uint8_t> {
    static constexpr auto format() noexcept { return FmtUByte; }

    static constexpr auto silence() noexcept { return uint8_t{128}; }

    static constexpr auto to_float(const uint8_t sample) noexcept -> float
    { return gsl::narrow_cast<float>(sample-128) * (1.0f/128.0f); }
};

template<>
struct SampleInfo<int16_t> {
    static constexpr auto format() noexcept { return FmtShort; }

    static constexpr auto silence() noexcept { return int16_t{}; }

    static constexpr auto to_float(const int16_t sample) noexcept -> float
    { return gsl::narrow_cast<float>(sample) * (1.0f/32768.0f); }
};

template<>
struct SampleInfo<int32_t> {
    static constexpr auto format() noexcept { return FmtInt; }

    static constexpr auto silence() noexcept { return int32_t{}; }

    static constexpr auto to_float(const int32_t sample) noexcept -> float
    { return gsl::narrow_cast<float>(sample)*(1.0f/2147483648.0f); }
};

template<>
struct SampleInfo<float> {
    static constexpr auto format() noexcept { return FmtFloat; }

    static constexpr auto silence() noexcept { return float{}; }

    static constexpr auto to_float(const float sample) noexcept -> float { return sample; }
};

template<>
struct SampleInfo<double> {
    static constexpr auto format() noexcept { return FmtDouble; }

    static constexpr auto silence() noexcept { return double{}; }

    static constexpr auto to_float(const double sample) noexcept -> float
    { return gsl::narrow_cast<float>(sample); }
};

template<>
struct SampleInfo<MulawSample> {
    static constexpr auto format() noexcept { return FmtMulaw; }

    static constexpr auto silence() noexcept { return MulawSample{uint8_t{127}}; }

    static constexpr auto to_float(const MulawSample sample) noexcept -> float
    { return gsl::narrow_cast<float>(muLawDecompressionTable[sample.value]) * (1.0f/32768.0f); }
};

template<>
struct SampleInfo<AlawSample> {
    static constexpr auto format() noexcept { return FmtAlaw; }

    /* Technically not "silence", it's a value of +8 as int16, but +/-8 is the
     * closest to 0.
     */
    static constexpr auto silence() noexcept { return AlawSample{uint8_t{213}}; }

    static constexpr auto to_float(const AlawSample sample) noexcept -> float
    { return gsl::narrow_cast<float>(aLawDecompressionTable[sample.value]) * (1.0f/32768.0f); }
};

template<>
struct SampleInfo<IMA4Data> {
    static constexpr auto format() noexcept { return FmtIMA4; }

    static constexpr auto silence() noexcept { return IMA4Data{std::byte{}}; }
};

template<>
struct SampleInfo<MSADPCMData> {
    static constexpr auto format() noexcept { return FmtMSADPCM; }

    static constexpr auto silence() noexcept { return MSADPCMData{std::byte{}}; }
};

#endif /* CORE_FMT_TRAITS_H */

#ifndef _RANDOM_H_
#define _RANDOM_H_

namespace Twin2Engine::Core
{
    class Random
    {
    private:
        static std::random_device _randomDevice;
        static std::mt19937 _generator;

    public:
        template<typename T>
        static std::enable_if<std::is_integral<T>::value, T>::type Generate();

        template<typename T>
        static std::enable_if<std::is_floating_point<T>::value, T>::type Generate();

        template<typename T>
        static std::enable_if<std::is_integral<T>::value, T>::type Range(T minInclusive, T maxInclusive);

        template<typename T>
        static std::enable_if<std::is_floating_point<T>::value, T>::type Range(T minInclusive, T maxExclusive);
    };

    template<typename T>
    static std::enable_if<std::is_integral<T>::value, T>::type Random::Generate()
    {
        //std::uniform_int_distribution<T> distribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::uniform_int_distribution<T> distribution();
        return distribution(_generator);
    }

    template<typename T>
    static std::enable_if<std::is_floating_point<T>::value, T>::type Random::Generate()
    {
        std::uniform_real_distribution<T> distribution(0.0, 1.0);
        return distribution(_generator);
    }

    template<typename T>
    std::enable_if<std::is_integral<T>::value, T>::type Random::Range(T minInclusive, T maxInclusive)
    {
        if (minInclusive > maxInclusive)
        {
            T temp = minInclusive;
            minInclusive = maxInclusive;
            maxInclusive = temp;
        }

        std::uniform_int_distribution<T> distribution(minInclusive, maxInclusive);
        return distribution(_generator);
    }

    template<typename T>
    std::enable_if<std::is_floating_point<T>::value, T>::type Random::Range(T minInclusive, T maxExclusive)
    {
        //static_assert(std::is_trivial<T>::value, "Type must be trivial");
        if (minInclusive > maxExclusive)
        {
            T temp = minInclusive;
            minInclusive = maxExclusive;
            maxExclusive = temp;
            //throw std::invalid_argument("Invalid range");
        }

        std::uniform_real_distribution<T> distribution(minInclusive, maxExclusive);
        return distribution(_generator);
        //if constexpr (std::is_integral<T>::value)
        //{
        //    std::uniform_int_distribution<T> distribution(minInclusive, maxExclusive - 1);
        //    return distribution(_generator);
        //    //return GenerateIntegralInRange<T>(minInclusive, maxExclusive);
        //}
        //else if constexpr (std::is_floating_point<T>::value)
        //{
        //    std::uniform_real_distribution<T> distribution(minInclusive, maxExclusive);
        //    return distribution(_generator);
        //    //return GenerateFloatingPointInRange<T>(minInclusive, maxExclusive);
        //}
        //return GenerateInRangeRandom<T>(minInclusive, maxExclusive);
    }
}

#endif // !_RANDOM_H_
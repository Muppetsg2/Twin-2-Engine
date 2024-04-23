#ifndef _VECTOR_SHUFFLER_H_
#define _VECTOR_SHUFFLER_H_

#include <core/Random.h>

namespace Generation
{
    class VectorShuffler
    {

    public:
        template<class T>
        static void Shuffle(std::vector<T>& toShuffle);
    };

    template<class T>
    void VectorShuffler::Shuffle(std::vector<T>& toShuffle)
    {
        int n = toShuffle.size();
        int k = 0;
        while (n > 1)
        {
            n--;
            k = Twin2Engine::Core::Random::Range(0, n);
            T value = toShuffle[k];
            toShuffle[k] = toShuffle[n];
            toShuffle[n] = value;
        }
    }
}



#endif // !_VECTOR_SHUFFLER_H_
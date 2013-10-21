#include <cstdlib>

#include "random.h"

RandomDoubles::RandomDoubles(int seed, int count)
    : m_count(count),
      m_index(0),
      m_values(new double[count])
{
    srand(seed);
    for (int i = 0; i < count; i++) {
        m_values[i] = rand() / (double)RAND_MAX;
    }

    m_index = (int)(count * (rand() / (double)RAND_MAX));
}

double RandomDoubles::next()
{
    if (m_index >= m_count) {
        m_index = 0;
    }

    return m_values[m_index++];
}

RandomDoubles::~RandomDoubles()
{
    delete[] m_values;
}

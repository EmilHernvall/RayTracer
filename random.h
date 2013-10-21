#ifndef __RANDOM_H_
#define __RANDOM_H_

class RandomDoubles {
public:
    RandomDoubles(int seed, int count);
    ~RandomDoubles();

    double next();
private:
    int m_count;
    int m_index;
    double* m_values;
};

#endif // __RANDOM_H_

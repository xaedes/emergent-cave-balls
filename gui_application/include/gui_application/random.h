#pragma once

#include <random>

#include "gui_application/version.h"

namespace gui_application {

    class Random
    {
    public:
        Random(unsigned int value = 1337)
            : m_rndUniformDouble (0, 1)
            , m_rndUniformFloat  (0, 1)
            , m_rndNormalDouble  (0, 1)
            , m_rndNormalFloat   (0, 1)
        {
            seed(value);
        }
        virtual ~Random() {}
        
        inline void seed(unsigned int value)
        {
            m_seed = value;
            m_gen = std::mt19937(m_seed);
        }

        inline unsigned int seed() const { return m_seed; }
        inline std::mt19937& gen() { return m_gen; }

        int    uniform_int( int    min,  int    max );
        float  uniform( float  min,  float  max );
        double uniform( double min,  double max );
        float  normal ( float  mean, float  std );
        double normal ( double mean, double std );
        float  normal_minmax ( float  mean, float  std, float  min,  float  max );
        double normal_minmax ( double mean, double std, double min,  double max );
        float  normal_nstd   ( float  mean, float  std, float  n_std );
        double normal_nstd   ( double mean, double std, double n_std );

    protected:
        unsigned int m_seed;
        std::mt19937 m_gen;
        std::uniform_real_distribution<double> m_rndUniformDouble;
        std::uniform_real_distribution<float>  m_rndUniformFloat;
        std::normal_distribution<double>       m_rndNormalDouble;
        std::normal_distribution<float>        m_rndNormalFloat;
    };

} // namespace gui_application


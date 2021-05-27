#include "gui_application/random.h"

namespace gui_application {

    int    Random::uniform_int( int    min,  int    max )
    {
        return min + (max - min) * m_rndUniformDouble(gen());
    }
    float  Random::uniform( float  min,  float  max )
    {
        return min + (max - min) * m_rndUniformFloat(gen());
    }
    double Random::uniform( double min,  double max )
    {
        return min + (max - min) * m_rndUniformDouble(gen());
    }
    float  Random::normal ( float  mean, float  std )
    {
        return mean + std * m_rndNormalFloat(gen());
    }
    double Random::normal ( double mean, double std )
    {
        return mean + std * m_rndNormalFloat(gen());
    }
    float  Random::normal_minmax ( float  mean, float  std, float  min,  float  max )
    {
        float val = normal(mean, std);
        return (val < min) ? min : (
            (val > max) ? max : val
        );
    }
    double Random::normal_minmax ( double mean, double std, double min,  double max )
    {
        double val = normal(mean, std);
        return (val < min) ? min : (
            (val > max) ? max : val
        );
    }
    float  Random::normal_nstd   ( float  mean, float std, float  n_std )
    {
        return normal_minmax(mean, std, mean - n_std * std, mean + n_std * std);
    }
    double Random::normal_nstd   ( double mean, double std, double n_std )
    {
        return normal_minmax(mean, std, mean - n_std * std, mean + n_std * std);
    }
    
} // namespace gui_application
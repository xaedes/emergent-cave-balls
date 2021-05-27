#pragma once

#include <cmath>
#include "geometry/pi.h"

// #define PI 3.141592653589793238462643383279502884197169399375105820974944

namespace geometry {

    // Translate to equivalent angle in [0,2pi[ range
    template <typename T>
    T wrapTo2Pi(T rad)
    {
        return std::fmod(rad, static_cast<T>(2 * PI)) + ((rad < 0) ? static_cast<T>(2 * PI) : 0);
    }



    // Translate to equivalent angle in ]-pi,pi] range
    template <typename T>
    T wrapToPi(T rad)
    {
        return wrapTo2Pi<T>(rad + static_cast<T>(PI)) - static_cast<T>(PI);
    }



    // Translate rad_now to equivalent angle so the jump from rad_before won't be greater than |PI|
    template <typename T>
    T wrapToPiSeq(T rad_before, T rad_now)
    {
        rad_before = wrapToPi(rad_before);
        rad_now = wrapToPi(rad_now);
        T diff = rad_now - rad_before;
        if (diff > +static_cast<T>(PI)) rad_now -= static_cast<T>(2 * PI);
        if (diff < -static_cast<T>(PI)) rad_now += static_cast<T>(2 * PI);
        return rad_now;
    }

    template <typename T>
    T angleDiff(T rad_before, T rad_now)
    {
        T rad_now_ = wrapToPiSeq<T>(rad_before, rad_now);
        return wrapToPi<T>(rad_now_ - rad_before);
    }

} // namespace geometry

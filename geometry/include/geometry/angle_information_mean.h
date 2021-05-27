#pragma once

#include "geometry/wrap_pi.h"

namespace geometry {

    template<typename float_t = double, typename uint_t = unsigned int>
    struct AngleInformationMean
    {
        using float_type = float_t;
        using uint_type = uint_t;

        float_type evidence_sum = 0;
        uint_type evidence_count = 0;

        // Translate to equivalent angle in [0,2pi[ range
        float_type wrapTo2Pi(float_type rad)
        {
            return std::fmod(rad, static_cast<float_type>(2 * PI)) + ((rad < 0) ? static_cast<float_type>(2 * PI) : 0);
        }

        // Translate to equivalent angle in ]-pi,pi] range
        float_type wrapToPi(float_type rad)
        {
            return wrapTo2Pi(rad + static_cast<float_type>(PI)) - static_cast<float_type>(PI);
        }

        // Translate rad_now to equivalent angle so the jump from rad_before won't be greater than |PI|
        float_type wrapToPiSeq(float_type rad_before, float_type rad_now)
        {
            rad_before = wrapToPi(rad_before);
            rad_now = wrapToPi(rad_now);
            float_type diff = rad_now - rad_before;
            if (diff > +static_cast<float_type>(PI)) rad_now -= static_cast<float_type>(2 * PI);
            if (diff < -static_cast<float_type>(PI)) rad_now += static_cast<float_type>(2 * PI);
            return rad_now;
        }

        float_type angleDiff(float_type rad_before, float_type rad_now)
        {
            float_type rad_now_ = wrapToPiSeq(rad_before, rad_now);
            return wrapToPi(rad_now_ - rad_before);
        }

        void evidence(float_type angle)
        {
            if (evidence_count == 0)
            {
                evidence_sum = angle;
                evidence_count = 1;
            }
            else
            {
                float_type mean_ = mean();
                float_type innovation = angleDiff(mean_, angle);
                evidence_sum += (mean_ + innovation);
                ++evidence_count;
            }
        }
        float_type mean()
        {
            return (evidence_sum / evidence_count);
        }
        float_type mean(float_type default)
        {
            return evidence_count ? (evidence_sum / evidence_count) : default;
        }
    };

} // namespace geometry

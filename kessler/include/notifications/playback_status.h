#pragma once

#ifndef KESSLER_PLAYBACK_STATUS_H
#define KESSLER_PLAYBACK_STATUS_H

#include <boost/endian/conversion.hpp>

class PlaybackStatus {
public:
    PlaybackStatus(boost::uint8_t playback_status,
                   float elapsed_time,
                   float photo_count,
                   boost::uint8_t motor_count,
                   boost::uint8_t motor_address,
                   float position
                   );
    boost::uint8_t playback_status;
    float elapsed_time;
    float photo_count;
    boost::uint8_t motor_count;
    boost::uint8_t motor_address;
    float position;
};


#endif //KESSLER_PLAYBACK_STATUS_H

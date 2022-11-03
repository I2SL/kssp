#include "playback_status.h"

PlaybackStatus::PlaybackStatus(boost::uint8_t playback_status, float elapsed_time, boost::uint8_t motor_count,
                               boost::uint8_t motor_address, float position):
                               playback_status(playback_status),
                               elapsed_time(elapsed_time),
                               motor_count(motor_count),
                               motor_address(motor_address),
                               position(position)
                               {}
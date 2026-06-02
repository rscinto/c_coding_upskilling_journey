// sensor_common.h
#ifndef SENSOR_COMMON_H
#define SENSOR_COMMON_H


typedef enum
{
    // Sensor is intentionally disabled.
    // No sampling, no init attempts, no error recovery.
    SENSOR_STATE_OFF,

    // Sensor is currently being initialized/configured.
    // Example: check device ID, load calibration, write config registers.
    // If successful -> READY.
    // If failed -> ERROR or REINIT_WAIT.
    SENSOR_STATE_INIT,

    // Sensor is initialized and available.
    // Normal sampling/update logic is allowed to run.
    SENSOR_STATE_READY,

    // Sensor has failed enough times that its data should not be trusted.
    // App/UI may show ERR, stale data, or sensor unavailable.
    SENSOR_STATE_ERROR,

    // Sensor is waiting before trying initialization again.
    // This prevents hammering the I2C bus or spamming init attempts.
    // After delay expires -> INIT.
    SENSOR_STATE_REINIT_WAIT

} Sensor_State_t;

#endif

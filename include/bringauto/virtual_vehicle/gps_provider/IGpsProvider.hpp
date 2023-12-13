#pragma once


namespace bringauto::virtual_vehicle::gps_provider {
/**
 * @brief Structure for gps position
 */
struct GpsPosition {
	float latitude;
	float longitude;
	float altitude;
};

/**
 * @brief Generic gps location provider interface, all gps tracker interfaces have to inherit from this class
 */
class IGpsProvider {
public:

	/**
	 * @brief Get position from GPS device in Decimal degrees (DD) format
	 * @return float latitude
	 */
	virtual GpsPosition getPosition() = 0;

	/**
	 * @brief Get current speed from GPS device
	 * @return float speed
	 */
	virtual float getSpeed() = 0;
};
}

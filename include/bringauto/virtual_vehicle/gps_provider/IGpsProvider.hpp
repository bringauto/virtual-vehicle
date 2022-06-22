#pragma once


namespace bringauto::virtual_vehicle::gps_provider{
/**
 * @brief Generic gps location provider interface, all gps tracker interfaces have to inherit from this class
 */
class IGpsProvider {
public:
	/**
	 * @brief Get latitude from GPS device in Decimal degrees (DD) format
	 * @return float latitude
	 */
	virtual float getLatitude() = 0;

	/**
	 * @brief Get longitude from GPS device in Decimal degrees (DD) format
	 * @return float longitude
	 */
	virtual float getLongitude() = 0;

	/**
	 * @brief Get altitude from GPS device
	 * @return float altitude
	 */
	virtual float getAltitude() = 0;

	/**
	 * @brief Get current speed from GPS device
	 * @return float speed
	 */
	virtual float getSpeed() = 0;
};
}

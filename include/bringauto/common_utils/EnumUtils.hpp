#pragma once

#include <bringauto/communication/Status.hpp>
#include <bringauto/communication/Command.hpp>
#include <bringauto/settings/Settings.hpp>



namespace bringauto::common_utils {
/**
 * @brief Helper class for work with enums
 */
class EnumUtils {
public:
	EnumUtils() = delete;

	/**
	 * @brief Method maps corresponding string into fleet provider enum, transformation is case insensitive
	 * @param toEnum string to be mapped to enum, see @settings::FleetProvider
	 * @return corresponding enum, FleetProvider::INVALID if no mapping possible (wrong input)
	 */
	static settings::FleetProvider stringToFleetProvider(std::string toEnum);

	/**
	 * @brief Method maps corresponding string into vehicle provider enum, transformation is case insensitive
	 * @param toEnum string to be mapped to enum, see @settings::VehicleProvider
	 * @return corresponding enum, VehicleProvider::INVALID if no mapping possible (wrong input)
	 */
	static settings::VehicleProvider stringToVehicleProvider(std::string toEnum);

	/**
	 * @brief Method maps corresponding string into gps provider enum, transformation is case insensitive
	 * @param toEnum string to be mapped to enum, see @settings::GpsProvider
	 * @return corresponding enum, GpsProvider::INVALID if no mapping possible (wrong input)
	 */
	static settings::GpsProvider stringToGpsProvider(std::string toEnum);

	/**
	 * @brief Translate given enum to string representation
	 * @tparam T enum type
	 * @param value enum value
	 * @return string representing enum value
	 */
	template <typename T>
	static std::string enumToString(T value);
};
}
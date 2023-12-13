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
	 * @brief Method maps corresponding value into enum, transformation is case insensitive
	 * @param toEnum string to be mapped to enum
	 * @return corresponding enum
	 */
	template <typename T, typename K>
	static T valueToEnum(K value);

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
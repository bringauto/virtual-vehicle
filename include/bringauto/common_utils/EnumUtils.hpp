#pragma once

#include <bringauto/communication/Status.hpp>
#include <bringauto/communication/Command.hpp>



namespace bringauto::common_utils {
/**
 * @brief Helper class for work with enums
 */
class EnumUtils {
public:
	EnumUtils() = delete;

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
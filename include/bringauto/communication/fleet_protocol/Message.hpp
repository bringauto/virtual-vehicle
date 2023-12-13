#pragma once


#include <internal_client.h>
#include <string>



namespace bringauto::communication::fleet_protocol {
/**
 * @brief Class that handles message allocation and deallocation
 */
class Message {
public:
	/**
	 * @brief Correctly allocates and create message
	 */
	explicit Message(const std::string &data);

	/**
	 * @brief Returns message buffer
	 */
	[[nodiscard]] buffer getBuffer() const;

	/**
	 * @brief Correctly deallocates message
	 */
	~Message();

private:
	/// Internal client buffer for message
	buffer message_ {};
};
}

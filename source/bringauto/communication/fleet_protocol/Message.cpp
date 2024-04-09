#include <bringauto/communication/fleet_protocol//Message.hpp>

#include <cstring>



namespace bringauto::communication::fleet_protocol {
Message::Message(const std::string &data) {
	allocate(&message_, data.size());
	std::memcpy(message_.data, data.c_str(), message_.size_in_bytes);
}

Message::~Message() {
	deallocate(&message_);
}

buffer Message::getBuffer() const {
	return message_;
}
}
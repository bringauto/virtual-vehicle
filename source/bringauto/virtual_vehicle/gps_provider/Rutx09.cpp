#include <bringauto/virtual_vehicle/gps_provider/Rutx09.hpp>

#include <stdexcept>
///enable modbus, allow remote access, allow persistent connection
///enable gps
///router subnet HAVE to be different from wan subnet!!!

namespace bringauto::virtual_vehicle::gps_provider {
RUTX09::RUTX09(const std::string &ipAddress, uint16_t port, int slaveId): modbusConnection_(ipAddress, port) {
	modbusConnection_.modbus_set_slave_id(slaveId);
}

float RUTX09::getLatitude() {
	readValue(latitudeRegister_);
	return unpackFloat();
}

float RUTX09::getLongitude() {
	readValue(longitudeRegister_);
	return unpackFloat();
}

float RUTX09::getAltitude() {
	return 0.0;
}

float RUTX09::getSpeed() {
	readValue(speedRegister_);
	return unpackFloat();
}

void RUTX09::readValue(const RUTX09::RutxValue &rutxValue) {
	reconnectModbus();
	modbusConnection_.modbus_read_holding_registers(rutxValue.address, rutxValue.length, buffer_);
}

void RUTX09::reconnectModbus() {
	if(!modbusConnection_.is_connected()) {
		if(!modbusConnection_.modbus_connect()){
			throw std::runtime_error("Unable to connect to modbus server!");
		}
	}
}

float RUTX09::unpackFloat() {
	uint32_t temp = unpackUnsignedInt();
	float value;
	std::memcpy(&value, &temp, sizeof(value));
	return value;
}

int RUTX09::unpackInt() {
	uint32_t temp = unpackUnsignedInt();
	return static_cast<int>(temp);
}

uint32_t RUTX09::unpackUnsignedInt() {
	uint32_t temp = ((buffer_[0] << 16) | (buffer_[1]));
	return temp;
}

GpsPosition RUTX09::getPosition() {
	GpsPosition position{};
	position.longitude = getLongitude();
	position.latitude = getLatitude();
	position.altitude = getAltitude();
	return position;
}

}

#pragma once

#include <bringauto/virtual_vehicle/gps_provider/IGpsProvider.hpp>
#include <modbus.h>

#include <string>
#include <memory>


namespace bringauto::virtual_vehicle::gps_provider {
/**
 * @brief Class for working with RUTX09 gps router from teltonika
 * This class communicates through modbus
 */
class RUTX09: public IGpsProvider {
private:
	enum class ValueType{
		UINT16_T,
		UINT32_T,
		INT,
		FLOAT,
		TEXT,
	};

	struct RutxValue{
		uint32_t address;
		uint32_t length;
		ValueType valueType;
	};
	///RUTX09 register information https://wiki.teltonika-networks.com/view/Monitoring_via_Modbus
	const static int maxRegisterSize_{16};
	const RutxValue latitudeRegister_{143,2, ValueType::FLOAT};
	const RutxValue longitudeRegister_{145,2, ValueType::FLOAT};
	// Documentation is bad, in documentation is speed INT but FLOAT seems to be more accurate, needs testing
	const RutxValue speedRegister_{179,2, ValueType::FLOAT};

public:
	RUTX09(const std::string &ipAddress, uint16_t port, int slaveId);

	float getLatitude() override;

	float getLongitude() override;

	float getAltitude() override;

	// Documentation is bad, in documentation is speed INT but FLOAT seems to be more accurate, needs testing
	float getSpeed() override;

private:
	modbus modbusConnection_; ///modbus lib https://github.com/fz-lyu/modbuspp
	uint16_t buffer_[maxRegisterSize_];

	void readValue(const RutxValue& rutxValue);
	void reconnectModbus();

	uint32_t unpackUnsignedInt();
	float unpackFloat();
	int unpackInt();
};
}
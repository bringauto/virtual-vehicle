#include <bringauto/virtual_vehicle/vehicle_provider/IVirtualVehicle.hpp>



namespace bringauto::virtual_vehicle::vehicle_provider {

void IVirtualVehicle::drive() {
	while(!globalContext_->ioContext.stopped()) {
		nextEvent();
	}
}
}
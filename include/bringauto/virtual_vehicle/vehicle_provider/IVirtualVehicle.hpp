#pragma once

#include <bringauto/osm/OsmiumHandler.hpp>
#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include <memory>


namespace bringauto::virtual_vehicle::vehicle_provider {
/**
 * @brief Generic vehicle location provider interface, all location provider interfaces have to inherit from this class
 */
class IVirtualVehicle {
public:
	IVirtualVehicle(const std::shared_ptr<osm::Route>& route, const std::shared_ptr<communication::ICommunication>& com,
					const std::shared_ptr<GlobalContext>& globalContext): route_(route), com_(com),
																   globalContext_(globalContext) {};

	/**
	 * Prepare vehicle and route for drive simulation
	 */
	virtual void initialize() = 0;

	/**
	 * Simulate vehicle driving
	 */
	void drive();

protected:
	std::shared_ptr<osm::Route> route_;
	std::shared_ptr<communication::ICommunication> com_;
	std::shared_ptr<GlobalContext> globalContext_;

	virtual void nextEvent() = 0;
};
}

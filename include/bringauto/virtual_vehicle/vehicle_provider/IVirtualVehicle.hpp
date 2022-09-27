#pragma once

#include <bringauto/osm/OsmiumHandler.hpp>
#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>
#include <bringauto/virtual_vehicle/Map.hpp>

#include <memory>
#include <utility>


namespace bringauto::virtual_vehicle::vehicle_provider {
/**
 * @brief Generic vehicle location provider interface, all location provider interfaces have to inherit from this class
 */
class IVirtualVehicle {
public:
	IVirtualVehicle(std::shared_ptr<communication::ICommunication> com, std::shared_ptr<GlobalContext> globalContext):
                    com_(std::move(com)), globalContext_(std::move(globalContext)) {};

	/**
	 * Prepare vehicle and route for drive simulation
	 */
	virtual void initialize() = 0;

	/**
	 * Simulate vehicle driving
	 */
	void drive();

protected:
    bringauto::virtual_vehicle::Map map_;
	std::shared_ptr<osm::Route> actualRoute_;

	std::shared_ptr<communication::ICommunication> com_;
	std::shared_ptr<GlobalContext> globalContext_;

	virtual void nextEvent() = 0;
};
}

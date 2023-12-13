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

	virtual ~IVirtualVehicle() = default;

protected:
	/// Driving map
	bringauto::virtual_vehicle::Map map_;
	/// Current route of the vehicle
	std::shared_ptr<osm::Route> actualRoute_;

	/// Communication interface
	std::shared_ptr<communication::ICommunication> com_;
	/// Global context
	std::shared_ptr<GlobalContext> globalContext_;

	/**
	 * @brief Simulate vehicle driving - got o next event
	 */
	virtual void nextEvent() = 0;
};
}

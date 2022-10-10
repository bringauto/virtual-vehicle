#include <bringauto/virtual_vehicle/Map.hpp>

#include <osmium/io/any_input.hpp>
#include <osmium/visitor.hpp>



void bringauto::virtual_vehicle::Map::loadMapFromFile(const std::string &filePath) {
	auto objectTypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation;
	osmium::io::Reader reader { filePath, objectTypes };
	bringauto::osm::OsmiumHandler handler;
	osmium::apply(reader, handler);
	reader.close();
	routes_ = handler.getRoutes();
	points_ = handler.getPoints();
}

std::shared_ptr<bringauto::osm::Route> bringauto::virtual_vehicle::Map::getRoute(const std::string &routeName) {
	auto routeIt = std::find_if(routes_.begin(), routes_.end(),
								[&routeName](const auto &route) { return route->getRouteName() == routeName; });
	if(routeIt == routes_.end()) {
		throw std::runtime_error("Route " + routeName + " was not found");
	}
	return *routeIt;
}

void bringauto::virtual_vehicle::Map::speedOverride(unsigned int speed) {
	for(auto const &route: routes_) {
		route->speedOverride(speed);
	}
}

void bringauto::virtual_vehicle::Map::prepareRoutes() {
	for(const auto &route: routes_) {
		route->prepareRoute();
	}
}


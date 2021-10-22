
#include <bringauto/osm/Way.hpp>

const std::vector<std::shared_ptr<bringauto::osm::Point>> &bringauto::osm::Way::getStops() const {
	return stops_;
}

const std::vector<std::shared_ptr<bringauto::osm::Point>> &bringauto::osm::Way::getPoints() const {
	return points_;
}

void bringauto::osm::Way::appendStop(const std::shared_ptr<Point> &point) {
	auto stopIt = std::find_if(stops_.begin(), stops_.end(),
							   [&point](const std::shared_ptr<Point> &stop) {
								   return point->getName() == stop->getName();
							   });
	//do not add duplicate stop
	if (stopIt == stops_.end()) {
		stops_.push_back(point);
	}
}

void bringauto::osm::Way::appendPoint(const std::shared_ptr<bringauto::osm::Point> &point) {
	points_.push_back(point);
}
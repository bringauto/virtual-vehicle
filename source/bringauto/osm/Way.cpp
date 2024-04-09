
#include <bringauto/osm/Way.hpp>



namespace bringauto::osm {
const std::vector<std::shared_ptr<Point>> &Way::getStops() const {
	return stops_;
}

const std::vector<std::shared_ptr<Point>> &Way::getPoints() const {
	return points_;
}

void Way::appendStop(const std::shared_ptr<Point> &point) {
	auto stopIt = std::find_if(stops_.begin(), stops_.end(),
							   [&point](const std::shared_ptr<Point> &stop) {
								   return point->getName() == stop->getName();
							   });
	///do not add duplicate stop
	if(stopIt == stops_.end()) {
		stops_.push_back(point);
	}
}

void Way::appendPoint(const std::shared_ptr<Point> &point) {
	points_.push_back(point);
}
}
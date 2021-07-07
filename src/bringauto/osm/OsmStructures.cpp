#include <bringauto/osm/OsmStructures.hpp>
#include <bringauto/logging/Logger.hpp>
#include <osmium/geom/haversine.hpp>

void bringauto::osm::Route::appendPoint(const std::shared_ptr<bringauto::osm::Point> &point) {
    points_.push_back(point);
}

void bringauto::osm::Route::setRouteName(const std::optional<std::string> &routeName) {
    routeName_ = routeName;
}

void bringauto::osm::OsmiumHandler::node(const osmium::Node &node) {
    std::shared_ptr<Point> point;
    bool stop = false;
    if (node.tags().has_key("stop")) {
        stop = std::string{node.tags().get_value_by_key("stop")} == "true";
    }
    if (node.tags().has_key("speed")) {
        double speed = std::stod(node.tags().get_value_by_key("speed"));
        point = std::make_shared<Point>(Point{node.id(), node.location().lat(), node.location().lon(), stop, speed});
    } else {
        point = std::make_shared<Point>(Point{node.id(), node.location().lat(), node.location().lon(), stop});
    }
    points_.push_back(point);
}

void bringauto::osm::OsmiumHandler::relation(const osmium::Relation &relation) {
    osmiumObjectId relationId = relation.id();
    if (relation.members().size() != 1) {
        throw std::runtime_error("Relation " + std::to_string(relationId) + " has multiple ways");
    }
    osmiumObjectId routeId = relation.members().begin()->ref();

    if (!relation.tags().has_key("name")) {
        throw std::runtime_error("Relation " + std::to_string(relationId) + " has no name");
    }

    std::string routeName = relation.tags().get_value_by_key("name");

    auto routeIt = std::find_if(routes_.begin(), routes_.end(),
                                [&routeId](const auto &route) { return route->getId() == routeId; });
    if (routeIt == routes_.end()) {
        throw std::runtime_error("Way " + std::to_string(routeId) + " was not found in relations");
    }
    routeIt->get()->setRouteName(routeName);
}

void bringauto::osm::OsmiumHandler::way(const osmium::Way &way) {
    auto route = std::make_shared<Route>(Route{way.id()});

    for (const auto &node : way.nodes()) {
        auto nodeId = node.ref();
        auto pointIt = std::find_if(points_.begin(), points_.end(),
                                    [&nodeId](const std::shared_ptr<Point> &point) {
                                        return point->getId() == nodeId;
                                    });
        if (pointIt == points_.end()) {
            throw std::runtime_error("Point " + std::to_string(nodeId) + " in way was not found points");
        }
        route->appendPoint(std::make_shared<Point>(Point{**pointIt}));
    }
    routes_.push_back(route);
}

[[nodiscard]] const std::vector<std::shared_ptr<bringauto::osm::Point>> &
bringauto::osm::OsmiumHandler::getPoints() const {
    return points_;
}

[[nodiscard]] const std::vector<std::shared_ptr<bringauto::osm::Route>> &
bringauto::osm::OsmiumHandler::getRoutes() const {
    return routes_;
}


std::string bringauto::osm::Route::getRouteName() {
    return routeName_.has_value() ? routeName_.value() : "";
}

void bringauto::osm::Route::propagateSpeed() {
    double speed;
    if (points_.front()->getSpeedInMetersPerSecond() > 0) {
        speed = points_.front()->getSpeedInMetersPerSecond();
    } else {
        speed = 5;
        bringauto::logging::Logger::logWarning(
                "First point of route " + getRouteName() + " does not contain speed, defaulting to 5m/s");
    }

    for (const auto &point : points_) {
        if (point->getSpeedInMetersPerSecond() > 0) {
            speed = point->getSpeedInMetersPerSecond();
        } else {
            point->setSpeed(speed);
        }
    }
}

void bringauto::osm::Route::setNextPosition() {
    positionIt++;
    if (positionIt != points_.end()) {
        return;
    }
    bringauto::logging::Logger::logInfo("End of route has been reached, continuing another lap");
    if (!routeIsCircular_) {
        bringauto::logging::Logger::logInfo("Route is not circular, reversing.");
        std::reverse(points_.begin(), points_.end());
        positionIt = points_.begin();
        positionIt++;
    }else{
        positionIt = points_.begin();
    }

}

void bringauto::osm::Route::prepareRoute() {
    positionIt = points_.begin();
    double distanceBetweenStartAndEnd = osmium::geom::haversine::distance(
            osmium::geom::Coordinates{points_.front()->getLatitude(), points_.front()->getLongitude()},
            osmium::geom::Coordinates{points_.back()->getLatitude(), points_.back()->getLongitude()});

    routeIsCircular_ = (distanceBetweenStartAndEnd < roundRouteLimitInMeters);
    propagateSpeed();
}

std::shared_ptr<bringauto::osm::Point> bringauto::osm::Route::getPosition() {
    return *positionIt;
}





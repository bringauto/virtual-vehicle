
#include <bringauto/osm/Route.hpp>

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
    if(points_.empty()){
        throw std::runtime_error("Route " + getRouteName() + " has no points.");
    }
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

bool bringauto::osm::Route::areStopsPresent(const std::vector<std::string>& stopNames) {
    for(const auto& stopName: stopNames){
        auto pointIt = std::find_if(stops_.begin(), stops_.end(), [&stopName](const auto &point) { return stopName == point->getName(); });
        if(pointIt == stops_.end()){
            logging::Logger::logError("Unknown stop: " + stopName);
            return false;
        }
    }
    return true;
}

void bringauto::osm::Route::appendWay(const std::shared_ptr<Way>& way) {
    auto points = way->getPoints();
    auto stops = way->getStops();
    if(!points_.empty()){
        double distanceBetweenRoutes = osmium::geom::haversine::distance(
                osmium::geom::Coordinates{points_.back()->getLatitude(), points_.back()->getLongitude()},
                osmium::geom::Coordinates{points.front()->getLatitude(), points.front()->getLongitude()});
        if(distanceBetweenRoutes > routesDistanceThresholdInMeters_){
            bringauto::logging::Logger::logWarning("Distance between part of routes is higher than threshold " + std::to_string(roundRouteLimitInMeters));
        }
    }
    points_.insert(points_.end(), points.begin(), points.end());
    stops_.insert(stops_.end(), stops.begin(), stops.end());
}

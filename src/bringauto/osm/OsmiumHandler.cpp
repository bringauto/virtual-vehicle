#include <bringauto/osm/OsmiumHandler.hpp>
#include <bringauto/logging/Logger.hpp>

#include <osmium/geom/haversine.hpp>


void bringauto::osm::Route::setRouteName(const std::optional<std::string> &routeName) {
    routeName_ = routeName;
}

void bringauto::osm::OsmiumHandler::node(const osmium::Node &node) {
    std::shared_ptr<Point> point;
    bool stop = false;
    std::string stopName{};

    if (node.tags().has_key("stop")) {
        stop = std::string{node.tags().get_value_by_key("stop")} == "true";
    }

    if(stop){
        if(!node.tags().has_key("name") ){
            bringauto::logging::Logger::logWarning("Warning! Found unnamed stop that will be ignored!");
            stop = false;
        }else{
            stopName = std::string{node.tags().get_value_by_key("name")};
        }
    }

    if (node.tags().has_key("speed")) {
        double speed = std::stod(node.tags().get_value_by_key("speed"));
        point = std::make_shared<Point>(Point{node.id(), node.location().lat(), node.location().lon(), stop, stopName, speed});
    } else {
        point = std::make_shared<Point>(Point{node.id(), node.location().lat(), node.location().lon(), stop, stopName});
    }
    points_.push_back(point);
}

void bringauto::osm::OsmiumHandler::relation(const osmium::Relation &relation) {
    osmiumObjectId relationId = relation.id();
    auto route = std::make_shared<Route>(Route{relation.id()});

    if (relation.members().empty()) {
        throw std::runtime_error("Relation " + std::to_string(relationId) + " does not have a way");
    }

    if (!relation.tags().has_key("name")) {
        throw std::runtime_error("Relation " + std::to_string(relationId) + " has no name");
    }
    std::string routeName = relation.tags().get_value_by_key("name");
    route->setRouteName(routeName);

    for(auto const& member: relation.members()){
        auto wayIt = std::find_if(ways_.begin(), ways_.end(),
                                    [&member](const auto &way) { return member.ref() == way->getId(); });
        if (wayIt == ways_.end()) {
            throw std::runtime_error("Way " + std::to_string(member.ref()) + " was not found in relations");
        }
        route->appendWay(*wayIt);
    }

    routes_.push_back(route);
}

void bringauto::osm::OsmiumHandler::way(const osmium::Way &way) {
    auto currentWay = std::make_shared<Way>(Way{way.id()});

    for (const auto &node : way.nodes()) {
        auto nodeId = node.ref();
        auto pointIt = std::find_if(points_.begin(), points_.end(),
                                    [&nodeId](const std::shared_ptr<Point> &point) {
                                        return point->getId() == nodeId;
                                    });
        if (pointIt == points_.end()) {
            throw std::runtime_error("Point " + std::to_string(nodeId) + " in way was not found points");
        }
        auto pointToAdd = std::make_shared<Point>(Point{**pointIt});
        currentWay->appendPoint(pointToAdd);
        if(pointIt->get()->isStop()){
            currentWay->appendStop(pointToAdd);
        }
    }
    ways_.push_back(currentWay);
}

[[nodiscard]] const std::vector<std::shared_ptr<bringauto::osm::Point>> &
bringauto::osm::OsmiumHandler::getPoints() const {
    return points_;
}

[[nodiscard]] const std::vector<std::shared_ptr<bringauto::osm::Route>> &
bringauto::osm::OsmiumHandler::getRoutes() const {
    return routes_;
}







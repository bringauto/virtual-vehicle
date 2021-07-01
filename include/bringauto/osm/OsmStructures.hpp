#pragma once

#include <osmium/handler.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/osm/relation.hpp>
#include <vector>
#include <optional>
#include <memory>

namespace bringauto::osm {
    using osmiumObjectId = int64_t;

    /**
     * Parent object of Node, Way, Relation and Area, see [OSM entities](https://osmcode.org/osmium-concepts/), only id is used from OSM object
     */
    class OsmObject {
    public:
        explicit OsmObject(osmiumObjectId elementId)
                : id_(elementId) {};

        [[nodiscard]] osmiumObjectId getId() const {
            return id_;
        }

    protected:
        /**
         * ID of object, in .osm file id='<object id>'
         */
        osmiumObjectId id_{-1};
    };

    /**
     * Class for representing OSM node, contains all information needed for route, point position, speed and stop
     */
    class Point : public OsmObject {
    public:
        Point(osmiumObjectId nodeId, double nodeLatitude, double nodeLongitude, bool stop)
                : OsmObject(nodeId), latitude_(nodeLatitude), longitude_(nodeLongitude), stop_(stop) {}

        Point(osmiumObjectId nodeId, double nodeLatitude, double nodeLongitude, bool stop, double speed)
                : OsmObject(nodeId), latitude_(nodeLatitude), longitude_(nodeLongitude), stop_(stop),
                  speedInMetersPerSecond_(speed) {}

        Point(const Point &point) : OsmObject(point.getId()), latitude_(point.getLatitude()),
                                    longitude_(point.getLongitude()), stop_(point.isStop()), speedInMetersPerSecond_(
                        point.getSpeedInMetersPerSecond()) {}

        [[nodiscard]] double getLongitude() const {
            return longitude_;
        }

        [[nodiscard]] double getLatitude() const {
            return latitude_;
        }

        [[nodiscard]] const double &getSpeedInMetersPerSecond() const {
            //todo error check
            return speedInMetersPerSecond_;
        }

        [[nodiscard]] bool isStop() const {
            return stop_;
        }

        void setSpeed(const double &speed) {
            speedInMetersPerSecond_ = speed;
        }

    private:
        double longitude_;
        double latitude_;
        double speedInMetersPerSecond_{-1};
        bool stop_{};
    };


    /**
     * Class for representing both osm Way and Relation
     */
    class Route : public OsmObject {
    public:
        explicit Route(osmiumObjectId nodeId, const std::string &routeName)
                : OsmObject(nodeId), routeName_(routeName) {
        }

        explicit Route(osmiumObjectId nodeId)
                : OsmObject(nodeId) {
        }

        /**
         * Adding point to the end of route
         * @param point point to be appended
         */
        void appendPoint(const std::shared_ptr<Point> &point);

        /**
         * Only some points have speed tag, this method will propagate speed into rest of points
         */
        void propagateSpeed();

        /**
         * Function that will prepare route, it will set actual position to starting (first) point and determine if route is routeIsCircular_ or not
         */
        void prepareRoute();

        void setRouteName(const std::optional<std::string> &routeName);

        std::string getRouteName();

        std::shared_ptr<Point> getPosition();

        /**
         * Move on point vector, if end is reached route will be connected to start if circular or reversed
         */
        void setNextPosition();

    private:
        std::vector<std::shared_ptr<Point>> points_{};
        std::optional<std::string> routeName_;
        std::vector<std::shared_ptr<Point>>::iterator positionIt;
        bool routeIsCircular_{false};
        /**
         * Limit distance from stop and end point to determine if route is circular or not
         */
        const double roundRouteLimitInMeters{10.0};

    };

    /**
     * Handler that libosmium uses when reading file, node(), way() and relation() methods are called by osmium when corresponding objects are parsed,
     * class will parse those objects and store them
     */
    class OsmiumHandler : public osmium::handler::Handler {
    public:
        /**
         * Osmium handler method for getting Way object from file, method will create new Route and fill it with Points that are included in Way,
         * route name will not be set, route name is set in relation object
         *
         */
        void way(const osmium::Way &way);

        /**
         * Osmium handler method for getting Node object from file, method will create Point instance of node with
         * its position, stop and speed if present and appends it into points vector
         *
         */
        void node(const osmium::Node &node);

        /**
         * Osmium handler method for getting Relation object from file, method will read route name from relation and add it to corresponding route
         *
         */
        void relation(const osmium::Relation &relation);

        [[nodiscard]] const std::vector<std::shared_ptr<Point>> &getPoints() const;

        [[nodiscard]] const std::vector<std::shared_ptr<Route>> &getRoutes() const;

    private:
        std::vector<std::shared_ptr<Point>> points_;
        std::vector<std::shared_ptr<Route>> routes_;
    };
}
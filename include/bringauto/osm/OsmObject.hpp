#pragma once

#include <osmium/handler.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/osm/relation.hpp>

namespace bringauto::osm {
    using osmiumObjectId = int64_t;

/**
     * Parent object of Node, Way, Relation and Area, see [OSM entities](https://osmcode.org/osmium-concepts/), only id is used from OSM object
     */
    class OsmObject {
    public:
        explicit OsmObject(osmiumObjectId elementId)
                : id_(elementId) {};

        /**
         * Get osmium object id that corresponds with id/ref in .osm file
         * @return
         */
        [[nodiscard]] osmiumObjectId getId() const;

    protected:
        /**
         * ID of object, in .osm file id='<object id>'
         */
        osmiumObjectId id_{-1};
    };
}


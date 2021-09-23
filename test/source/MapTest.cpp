#include <bringauto/virtual_vehicle/Map.hpp>

#include <osmium/visitor.hpp>
#include <gtest/gtest.h>
#include <bringauto/logging/Logger.hpp>
#include <bringauto/logging/ConsoleSink.hpp>


TEST(MapCreation, MapTest){
    bringauto::logging::Logger::addSink<bringauto::logging::ConsoleSink>();
    bringauto::logging::Logger::LoggerSettings params{"virtual-vehicle-mapTest", bringauto::logging::Logger::Verbosity::Info};
    bringauto::logging::Logger::init(params);
    std::string mapPath = "maps/virtual_vehicle_map.osm";
    std::string emptyMapPath = "maps/empty.osm";
    std::string wrongPath = "maps/fake.osm";

    bringauto::virtual_vehicle::Map map1, map2, map3;
    EXPECT_NO_THROW(map1.loadMapFromFile(mapPath));
    EXPECT_ANY_THROW(map2.loadMapFromFile(emptyMapPath));
    EXPECT_ANY_THROW(map3.loadMapFromFile(wrongPath));

    EXPECT_ANY_THROW(map1.getRoute("fake route"));
    EXPECT_NO_THROW(map1.getRoute("brno_luzanky"));
}
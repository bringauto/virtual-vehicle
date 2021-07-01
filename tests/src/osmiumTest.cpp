#include "../include/osmiumTest.h"

TEST(WrongFilePath, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(OsmiumHandler,OsmiumStructures){
    auto objectTypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation;
    std::string filePath = "./luzanky.osm";

    osmium::io::Reader reader{filePath, objectTypes};
    virtual_vehicle::osm::osm_structures::OsmiumHandler handler;
    osmium::apply(reader, handler);
    reader.close();

    std::cout << "debug";
}


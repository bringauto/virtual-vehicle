#include <bringauto/osm/OsmiumHandler.hpp>
#include <bringauto/osm/Point.hpp>
#include <bringauto/osm/Way.hpp>
#include <bringauto/osm/Route.hpp>
#include <OsmiumTest.hpp>

#include <gtest/gtest.h>
#include <osmium/io/any_input.hpp>
#include <osmium/visitor.hpp>

#include <filesystem>



TEST_F(OsmiumTest, WrongFilePath) {
	// Expect two strings not to be equal.
	auto objectTypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation;
	std::string filePath = "maps/fake.osm";
	EXPECT_ANY_THROW(osmium::io::Reader reader(filePath, objectTypes));
}

TEST_F(OsmiumTest, EmptyFile) {
	std::string fileName = "maps/empty.osm";
	auto objectTypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation;
	osmium::io::Reader reader { fileName, objectTypes };
	bringauto::osm::OsmiumHandler handler;
	EXPECT_ANY_THROW(osmium::apply(reader, handler));
	reader.close();
}


TEST_F(OsmiumTest, RouteParsing) {
	auto objectTypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation;
	std::string filePath = "maps/virtual_vehicle_map.osm";

	osmium::io::Reader reader { filePath, objectTypes };
	bringauto::osm::OsmiumHandler handler;
	EXPECT_NO_THROW(osmium::apply(reader, handler));
	reader.close();
	auto routes = handler.getRoutes();
	EXPECT_EQ(routes.size(), 10);

	auto points = handler.getPoints();
	EXPECT_FALSE(points.empty());
}

TEST_F(OsmiumTest, RouteName) {
	auto objectTypes = osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation;
	std::string filePath = "maps/virtual_vehicle_map.osm";

	osmium::io::Reader reader { filePath, objectTypes };
	bringauto::osm::OsmiumHandler handler;
	osmium::apply(reader, handler);
	reader.close();

	auto routes = handler.getRoutes();

	for(const auto &route: routes) {
		EXPECT_FALSE(route->getRouteName().empty());
	}
	std::string routeName = "Test name";
	auto firstRoute = routes.front();
	EXPECT_FALSE(firstRoute->getRouteName() == routeName);
	firstRoute->setRouteName(routeName);
	EXPECT_TRUE(firstRoute->getRouteName() == routeName);
}

TEST_F(OsmiumTest, Points) {
	double latitude1 = 10.0;
	double longitude1 = 10.0;
	double speed1 = 10.0;
	auto point1 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(1, latitude1, longitude1, true, "", speed1));
	auto point2 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(2, 20.01, 20.02, false, ""));

	EXPECT_TRUE(point1->isStop());
	EXPECT_FALSE(point2->isStop());
	EXPECT_DOUBLE_EQ(point1->getSpeedInMetersPerSecond(), speed1);
	EXPECT_TRUE(abs(point2->getSpeedInMetersPerSecond() - speed1) > 0.1);
	EXPECT_DOUBLE_EQ(point1->getLongitude(), longitude1);
	EXPECT_DOUBLE_EQ(point1->getLatitude(), latitude1);
	EXPECT_EQ(point1->getId(), 1);
	point2->setSpeed(point1->getSpeedInMetersPerSecond());
	EXPECT_DOUBLE_EQ(point1->getSpeedInMetersPerSecond(), point2->getSpeedInMetersPerSecond());
}

TEST_F(OsmiumTest, RoutesOneWay) {
	auto point1 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(1, 10.0, 10.0, true, "", 10.0));
	auto point2 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(2, 20.01, 20.02, false, ""));
	auto point3 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(3, 30.01, 30.02, true, ""));
	auto point4 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(4, 40.01, 40.02, false, "", 20.0));
	auto point5 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(4, 40.01, 40.02, true, "", 40.0));
	auto route = std::make_shared<bringauto::osm::Route>(bringauto::osm::Route(0));
	auto way = std::make_shared<bringauto::osm::Way>(bringauto::osm::Way(0));

	way->appendPoint(point1);
	way->appendPoint(point2);
	way->appendPoint(point3);
	way->appendPoint(point4);
	way->appendPoint(point5);

	route->appendWay(way);

	route->propagateSpeed();
	EXPECT_DOUBLE_EQ(point1->getSpeedInMetersPerSecond(), 10.0);
	EXPECT_DOUBLE_EQ(point2->getSpeedInMetersPerSecond(), 10.0);
	EXPECT_DOUBLE_EQ(point3->getSpeedInMetersPerSecond(), 10.0);
	EXPECT_DOUBLE_EQ(point4->getSpeedInMetersPerSecond(), 20.0);
	EXPECT_DOUBLE_EQ(point5->getSpeedInMetersPerSecond(), 40.0);

	route->prepareRoute();
	EXPECT_TRUE(route->getPosition() == point1);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point2);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point3);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point4);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point5);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point4);
}

TEST_F(OsmiumTest, RoutesCircular) {
	auto point1 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(1, 10.0, 10.0, true, ""));
	auto point2 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(2, 20.01, 20.02, false, ""));
	auto point3 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(3, 30.01, 30.02, true, ""));
	auto point4 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(4, 40.01, 40.02, false, "", 20.0));
	auto point5 = std::make_shared<bringauto::osm::Point>(
			bringauto::osm::Point(4, 10.0, 10.00, true, "", 40.0));
	auto route = std::make_shared<bringauto::osm::Route>(bringauto::osm::Route(0));

	auto way = std::make_shared<bringauto::osm::Way>(bringauto::osm::Way(0));

	way->appendPoint(point1);
	way->appendPoint(point2);
	way->appendPoint(point3);
	way->appendPoint(point4);
	way->appendPoint(point5);

	route->appendWay(way);
	route->propagateSpeed();
	EXPECT_DOUBLE_EQ(point1->getSpeedInMetersPerSecond(), 5.0);
	EXPECT_DOUBLE_EQ(point2->getSpeedInMetersPerSecond(), 5.0);
	EXPECT_DOUBLE_EQ(point3->getSpeedInMetersPerSecond(), 5.0);
	EXPECT_DOUBLE_EQ(point4->getSpeedInMetersPerSecond(), 20.0);
	EXPECT_DOUBLE_EQ(point5->getSpeedInMetersPerSecond(), 40.0);

	route->prepareRoute();
	EXPECT_TRUE(route->getPosition() == point1);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point2);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point3);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point4);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point5);
	route->setNextPosition();
	EXPECT_TRUE(route->getPosition() == point1);
}
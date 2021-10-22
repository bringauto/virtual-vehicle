#pragma once

#include <bringauto/osm/OsmObject.hpp>

namespace bringauto::osm {
	/**
	 * Class for representing OSM node, contains all information needed for route, point position, speed and stop
	 */
	class Point : public OsmObject {
	public:
		Point(osmiumObjectId nodeId, double nodeLatitude, double nodeLongitude, bool stop, const std::string &stopName)
				: OsmObject(nodeId), latitude_(nodeLatitude), longitude_(nodeLongitude), stop_(stop), name_{stopName} {}

		Point(osmiumObjectId nodeId, double nodeLatitude, double nodeLongitude, bool stop, const std::string &stopName,
			  double speed)
				: OsmObject(nodeId), latitude_(nodeLatitude), longitude_(nodeLongitude), stop_(stop), name_{stopName},
				  speedInMetersPerSecond_(speed) {}

		Point(const Point &point) : OsmObject(point.getId()), latitude_(point.getLatitude()),
									longitude_(point.getLongitude()), stop_(point.isStop()), speedInMetersPerSecond_(
						point.getSpeedInMetersPerSecond()), name_(point.getName()) {}

		/**
		 * Get point longitude
		 * @return longitude
		 */
		[[nodiscard]] double getLongitude() const;

		/**
		 * Get point latitude
		 * @return
		 */
		[[nodiscard]] double getLatitude() const;

		/**
		 * Get planned speed on point in m/s
		 * @return planned speed in m/s
		 */
		[[nodiscard]] const double &getSpeedInMetersPerSecond() const;

		/**
		 * See if point is also a stop
		 * @return true if point is stop
		 */
		[[nodiscard]] bool isStop() const;

		/**
		 * Set point planned speed in m/s
		 * @param speed speed in m/s
		 */
		void setSpeed(const double &speed);

		/**
		 * Get point name, used for detecting planned stops
		 * @return point name
		 */
		[[nodiscard]] const std::string &getName() const;

	private:
		double longitude_;
		double latitude_;
		double speedInMetersPerSecond_{-1};
		bool stop_{};
		std::string name_{};
	};
}


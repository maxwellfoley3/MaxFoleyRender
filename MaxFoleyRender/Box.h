//
//  Box.h
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 2/13/18.
//  Copyright © 2018 Maxwell Foley. All rights reserved.
//

#ifndef Box_h
#define Box_h

#include "common.h"
#include "Ray.h"

namespace MFR {
	class Box
	{
		public:
			Point origin;
			float width;
			float height;
			float depth;
		
			Box(){}
		
			Box(Point _origin, float _width, float _height, float _depth)
			{
				origin = _origin; width = _width; height = _height; depth = _depth;
			}
		
			Box expand(Box b)
			{
				float xMin = std::min(b.origin.x(),origin.x());
				xMin = std::min(xMin,b.origin.x()+b.width);
				xMin = std::min(xMin,origin.x()+width);
				
				float yMin = std::min(b.origin.y(),origin.y());
				yMin = std::min(yMin,b.origin.y()+b.height);
				yMin = std::min(yMin,origin.y()+height);
				
				float zMin = std::min(b.origin.z(),origin.z());
				zMin = std::min(zMin,b.origin.z()+b.depth);
				zMin = std::min(zMin,origin.z()+depth);
				
				
				float xMax = std::max(b.origin.x(),origin.x());
				xMax = std::max(xMax,b.origin.x()+b.width);
				xMax = std::max(xMax,origin.x()+width);
				
				float yMax = std::max(b.origin.y(),origin.y());
				yMax = std::max(yMax,b.origin.y()+b.height);
				yMax = std::max(yMax,origin.y()+height);
				
				float zMax = std::max(b.origin.z(),origin.z());
				zMax = std::max(zMax,b.origin.z()+b.depth);
				zMax = std::max(zMax,origin.z()+depth);
				
				origin = Point(xMin,yMin,zMin);
				width = xMax-xMin;
				height = yMax-yMin;
				depth = zMax-zMin;

				return *this;
				
			}
		
			//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
			bool intersect(Ray ray) {
                auto t1 = std::chrono::high_resolution_clock::now();
                
				Point minBound = origin;
				Point maxBound = Point(origin.x()+width,origin.y()+height,origin.z()+depth);
				
                auto t2 = std::chrono::high_resolution_clock::now();

				//if ray origin is in the box, return true
				if(ray.origin.x() > minBound.x() && ray.origin.x() < maxBound.x() &&
				ray.origin.y() > minBound.y() && ray.origin.y() < maxBound.y() &&
				ray.origin.z() > minBound.z() && ray.origin.z() < maxBound.z())
				{
					return true;
				}
				

				//contains the amount of traveling along the ray you would need to do to enter the bounds from the "near side" on x,y,and z planes
				Point nearDists = (minBound-ray.origin)/ray.direction;
				
				//contains the amount of traveling along the ray you would need to do to enter the bounds from the "far side" on x,y,and z planes
				Point farDists = (maxBound-ray.origin)/ray.direction;


				//is the secret just that we need to make sure all the entrance dists are smaller than the exit dists in order to actually have the correct understanding of entrance and exit?
				
                
				Point entranceDists = Point(std::min(nearDists.x(), farDists.x()),
				std::min(nearDists.y(), farDists.y()),
				std::min(nearDists.z(), farDists.z()));
				
                auto t3 = std::chrono::high_resolution_clock::now();

				Point exitDists = Point(std::max(nearDists.x(), farDists.x()),
				std::max(nearDists.y(), farDists.y()),
				std::max(nearDists.z(), farDists.z()));
                
                auto t4 = std::chrono::high_resolution_clock::now();



				//either ALL the exit dists must be bigger than the entrance dists (if it enters from the front) or vice versa (if it enters from the back). otherwise it doesnt actually hit the cube
				
				bool enterFromTheFront = (exitDists.y() > entranceDists.x()) && (exitDists.z() > entranceDists.x()) &&
				(exitDists.x() > entranceDists.y()) && (exitDists.z() > entranceDists.y()) &&
				(exitDists.x() > entranceDists.z()) && (exitDists.y() > entranceDists.z());
				
                auto t5 = std::chrono::high_resolution_clock::now();

				bool enterFromTheBack = (entranceDists.y() > exitDists.x()) && (entranceDists.z() > exitDists.x()) &&
				(entranceDists.x() > exitDists.y()) && (entranceDists.z() > exitDists.y()) &&
				(entranceDists.x() > exitDists.z()) && (entranceDists.y() > exitDists.z());
				
				if(!(enterFromTheFront || enterFromTheBack))
				{
					return false;
				}
                auto t6 = std::chrono::high_resolution_clock::now();

				//the one that actually enters the box is the greatest one
				float trueEntranceDist = std::max(std::max(entranceDists.x(),entranceDists.y()),entranceDists.z());
				
				//the one that actually enters the box is the least one
				float trueExitDist = std::min(std::min(exitDists.x(),exitDists.y()),exitDists.z());

                auto t7 = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t7 - t1 ).count();

                if(logLevel >= 2 && duration > 100) {
                    auto d1 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
                    auto d2 = std::chrono::duration_cast<std::chrono::microseconds>( t3 - t1 ).count();
                    auto d3 = std::chrono::duration_cast<std::chrono::microseconds>( t4 - t1 ).count();
                    auto d4 = std::chrono::duration_cast<std::chrono::microseconds>( t5 - t1 ).count();
                    auto d5 = std::chrono::duration_cast<std::chrono::microseconds>( t6 - t1 ).count();

                    printf ("time: %10d %10d %10d %10d %10d %10d \n", d1, d2, d3, d4, d5, duration);
                    std::stringstream ss;
                    ss << ray << duration << std::endl;
                    std::cout << ss.str();
                    printf ("box: %10d %10d %10d  %10d %10d %10d  \n", origin.x(), origin.y(), origin.z(), width, height, depth, duration);

                }

				//it only actually counts if these distances are positive
				if(trueEntranceDist >= 0 || trueExitDist >= 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		
			float size()
			{
				return width*height*depth;
			}
		
            int longest_axis() {
                // introduce element of randomness
                int r = (rand() % 3);
                if(r == 0) {
                    if(width > height && width > depth) {
                        return 0;
                    } else if (height > depth) {
                        return 1;
                    } else {
                        return 2;
                    }
                } else if(r==1) {
                    if(height > width && height > depth) {
                        return 1;
                    } else if (depth > width) {
                        return 2;
                    } else {
                        return 0;
                    }
                } else {
                    if(depth > width && depth > height) {
                        return 2;
                    } else if (width > height) {
                        return 0;
                    } else {
                        return 1;
                    }
                }
            }
        
			friend std::ostream& operator<<(std::ostream &strm, const MFR::Box &box) {
				return strm << "Box with origin " << box.origin << " width " << box.width <<
				" height " << box.height << " depth " << box.depth;
			}
	};
}
#endif /* Box_h */

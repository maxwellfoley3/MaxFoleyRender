//
//  Light.h
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 11/26/17.
//  Copyright © 2017 Maxwell Foley. All rights reserved.
//

#ifndef Light_h
#define Light_h

namespace MFR {

	//for now just point light
	enum LightType{PointLight};

	class Light {
		public:
			Point position;
			float strength;
			LightType type;
			bool shadowsEnabled;
			Color color;
	
			Light() {
			
			}
			Light(Point _position, float _strength, LightType _type)
			{
				position = _position; strength = _strength; type = _type;
			}
		
			Color biradiance(Point p)
			{
		//		return 1.0;
				//TODO: different types of lights
				float intensity = 1/pow((p-position).length(),2);
                return /*color*/ intensity * strength > 1.0 ? 1.0 : intensity * strength;
			}
		};
}
#endif /* Light_h */

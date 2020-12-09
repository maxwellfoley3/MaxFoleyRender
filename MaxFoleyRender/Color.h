//
//  Color.h
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 11/26/17.
//  Copyright © 2017 Maxwell Foley. All rights reserved.
//

#ifndef Color_h
#define Color_h

namespace MFR {
	class Color {
		public:
			float r;
			float g;
			float b;
		
			Color(void)
			{
				r = 0.0;
				g = 0.0;
				b = 0.0;
			}
		
			Color(float f)
			{
				r = f;
				g = f;
				b = f;
			}

			Color(float _r, float _g, float _b)
			{
				r = _r; g = _g; b = _b;
			}
		
			friend std::ostream& operator<<(std::ostream &strm, const MFR::Color &c) {
				return strm << "(" << c.r << "," << c.g << ","<< c.b << ")";
			}
		
			//addition
			Color & operator+=(const Color& rhs)
			{
				this->r += rhs.r;
				this->g += rhs.g;
				this->b += rhs.b;
				return *this;
			}
			friend MFR::Color operator+(MFR::Color lhs, const MFR::Color& rhs)
			{
				lhs += rhs;
				return lhs;
			}

			//scalar multiplication
			Color & operator*=(float f)
			{
				this->r = this->r*f;
				this->g = this->g*f;
				this->b = this->b*f;

				return *this;
			}
			friend Color operator*(Color lhs, const float f)
			{
				lhs.r*=f;
				lhs.g*=f;
				lhs.b*=f;
				return lhs;
			}
		
			//color multiplication
			Color & operator*=(Color rhs)
			{
				this->r = this->r*rhs.r;
				this->g = this->g*rhs.g;
				this->b = this->b*rhs.b;

				return *this;
			}
        
			friend Color operator*(Color lhs, Color rhs)
			{
				lhs.r*=rhs.r;
				lhs.g*=rhs.g;
				lhs.b*=rhs.b;
				return lhs;
			}
		
		
			//scalar division
			Color & operator/=(float f)
			{
				this->r = this->r/f;
				this->g = this->g/f;
				this->b = this->b/f;

				return *this;
			}
			friend Color operator/(Color lhs, const float f)
			{
				lhs.r/=f;
				lhs.g/=f;
				lhs.b/=f;
				return lhs;
			}

	};
}

#endif /* Color_h */


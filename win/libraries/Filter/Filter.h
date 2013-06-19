#ifndef Filter__h
#define Filter__h

/* 	Filter

	Small abstract Filter class, low-pass (LowPassFilter class) and high-pass filter classes (HighPassFilter).

	Copyright (C) 2010 Multiplo
	http://robotgroup.com.ar
	http://multiplo.org
	Developers: Alan Kharsansky; Julián U. da Silva Gillig
	Created: 2010.06.11 (yyyy.mm.dd)
	Last modified: 2010.09.13 (yyyy.mm.dd)

	IMPORTANT NOTICE: This library is based on code from the SNAPI library, which is under The Multiplo
	Pacifist License (MPL). This license is (or may be) not compatible with the GNU Lesser General Public
	License (LGPL), so this notice constitutes an special written exception from Multiplo, to allow deployment
	under the LGPL, in order to be compiled with the Arduino libraries and still hold compatibility	between
	both licenses.
	But this does not affects the SNAPI original license in any way, and DOES NOT AUTHORIZES ANY THIRD PARTY
	TO USE OR REDISTRIBUTE THE SNAPI LIBRARY UNDER THE LGPL. In other words: This exception ONLY AFFECTS the
	library when compiled with Arduino libraries, if and only if, this license (the MPL) results in some
	inconpatibility with the LGPL (so in this last case, then LGPL applies):

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

    Any way, we strongly believe in peace, so we would not like to see our work in any non-civil or military
    project. But, due to the possible incompatibilty with the LGPL licenses, this may be only a recommendation
    and an expression of our wishes. But, if some clever guy can determine that LGPL and MPL are compatible
    licenses, then MPL applies here. You can contact us at: info_t1@multiplo.com.ar
*/

class Filter
{
	protected:
		float filterValue;
		float kFilter;

	public:
        inline Filter(const float kFilter)  :   filterValue(0.0),
                                                kFilter(kFilter)
        {
        }

        inline virtual ~Filter()
        {
        }

        //Yes: these are inline virtuals, useful specially if the instances are accesed statically (not
        //through pointers), so the compiler does not always resolve the call dinamically:
        inline virtual float addValue(float value) = 0;

        inline virtual float getLastValue() const
        {
            return filterValue;
        }

        inline virtual void setFilter(const float value)
        {
            kFilter = value;
        }

        inline virtual float getFilter() const
        {
            return kFilter;
        }

        inline virtual void reset()
        {
            filterValue = 0.0;
        }
};


class LowPassFilter : public Filter
{
    public:
        inline LowPassFilter(const float kFilter) : Filter(kFilter)
        {
        }

        inline virtual ~LowPassFilter()
        {
        }

        inline float addValue(float value)
        {
            return filterValue = (value*kFilter) + (filterValue * (1.0 - kFilter));
        }
};


class HighPassFilter : public Filter
{
    public:
        inline HighPassFilter(const float kFilter) : Filter(kFilter)
        {
        }

        inline virtual ~HighPassFilter()
        {
        }

        inline float addValue(float value)
        {
            return filterValue = value - ((value*kFilter) + (filterValue * (1.0 - kFilter)));
        }
};

#endif

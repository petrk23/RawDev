/*
 * This file is part of RawDev.
 *
 * Copyright (C) 2020-2023 Petr Krajník
 *
 * RawDev is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RawDev is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RawDev.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <memory>
#include "Color.hpp"

class Image;
class Options;
class CamProfile;
class Rect;

class ScaleModule
{
	std::shared_ptr<CamProfile> m_CamProfile;
	double m_ColorTemp;
	int m_Tint;

public:
	static void run(Image& img, const Options &opt);

private:
	ScaleModule(// Constructor
		const std::shared_ptr<CamProfile> &profile, const Options &opt);
	void scale(Image &img, double black,
		double scaleR, double scaleG, double scaleB);
	void process(Image &img);

private: // Black point helpers
	double calcBlackPoint(const Image &img);
	void averageBlacks(const Image &img,
		Rect(&masked)[2], Color::RGB64 &black);
	Color::RGB64 estimateBlackPoint(const Image &img);

private: // White balance helpers
	void calcWhiteBalance(double &redWB, double &greenWB, double &blueWB);
	Color::RGB64 calcCameraNeutral();
	void applyTintCorrection(Color::RGB64 &refNeutral);

private: // Other
	void printScale(const char* name,
		const double value, const bool last = false);
};

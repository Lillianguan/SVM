#include "StdAfx.h"
#pragma once

#include "Plate.h"

Plate::Plate()
{
}

Plate::Plate(Mat img, Rect pos)
{
	plateImg = img;
	position = pos;
}



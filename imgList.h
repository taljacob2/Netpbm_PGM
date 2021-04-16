#define _CRT_SECURE_NO_WARNINGS
#ifndef __IMGLIST__
#define __IMGLIST__
#include "grayImage.h"
#include <stdio.h>
#include <stdlib.h>



unsigned int findAllSegments(grayImage* img, unsigned char threshold, imgPosCell*** segments);
#endif

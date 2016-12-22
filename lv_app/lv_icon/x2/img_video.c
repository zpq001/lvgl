#include "img_conf.h"
#include "lv_conf.h"

#if USE_IMG_VIDEO != 0 || LV_APP_USE_INTERNAL_ICONS == 2

#include <stdint.h> 
#include "misc/others/color.h"

const color_int_t img_video [] = { /*Width = 32, Height = 21*/ 
32,	/*Width*/
21,	/*Heigth*/
16,	/*Color depth = 16*/
1,	/*Flags: Transp = 1*/
2016, 2016, 2016, 2016, 33808, 31727, 31727, 31727, 31727, 31727, 31727, 31727, 31727, 31727, 31727, 31727, 31727, 31727, 31727, 33808, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 29582, 19017, 14823, 12710, 12710, 14791, 14791, 14791, 14791, 14791, 14791, 14791, 14791, 14791, 14791, 12710, 12710, 14791, 16936, 27469, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 21162, 8484, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 2145, 6371, 16904, 33808, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
25356, 6339, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 19049, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
19017, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12710, 2016, 2016, 2016, 2016, 2016, 2016, 21162, 21162, 
14823, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10597, 2016, 2016, 2016, 2016, 2016, 31727, 4258, 10565, 
14823, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12678, 2016, 2016, 2016, 2016, 23275, 6339, 0, 10597, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12678, 2016, 2016, 2016, 31727, 6371, 0, 0, 10597, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10597, 2016, 2016, 23275, 8452, 32, 0, 0, 12678, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8484, 2016, 2016, 8452, 0, 0, 0, 0, 12678, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4226, 16936, 16936, 2145, 0, 0, 0, 0, 12678, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4258, 21130, 21130, 6371, 0, 0, 0, 0, 12678, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10565, 2016, 2016, 19049, 4258, 0, 0, 0, 12678, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12710, 2016, 2016, 2016, 19049, 6339, 0, 0, 10597, 
14823, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12678, 2016, 2016, 2016, 2016, 19049, 2145, 0, 10565, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12678, 2016, 2016, 2016, 2016, 2016, 16936, 2113, 12678, 
16904, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10597, 2016, 2016, 2016, 2016, 2016, 2016, 12710, 16904, 
23243, 4226, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16936, 2016, 2016, 2016, 2016, 2016, 2016, 31695, 29614, 
2016, 14823, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10565, 29582, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 33808, 14823, 6371, 6371, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 6371, 6371, 14791, 27501, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
2016, 2016, 2016, 2016, 2016, 33808, 33808, 33808, 33808, 33808, 33808, 33808, 33808, 33808, 33808, 33808, 33808, 33808, 33808, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 2016, 
};

#endif
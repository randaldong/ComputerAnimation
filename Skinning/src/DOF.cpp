#include "DOF.h"
#include <iostream>

DOF::DOF() {
    DOFvalue = 0.0f; // default pose is set to 0.0f
    DOFmin = -4.0f;
    DOFmax = 4.0f;
}

DOF::~DOF()
{
}

void DOF::SetValue(float val) {
    if ((DOFmin <= val) && (val <= DOFmax))
        DOFvalue = val;
    else if (val < DOFmin)
        DOFvalue = DOFmin;
    else if (DOFmax < val)
        DOFvalue = DOFmax;
}

void DOF::SetMinMax(float min, float max) {
    DOFmin = min;
    DOFmax = max;
}

float DOF::GetValue() {
    return DOFvalue;
}
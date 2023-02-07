#pragma once

class DOF {
public:
    float DOFvalue;
    float DOFmin, DOFmax;

    DOF();
    ~DOF();

    void SetValue(float val);
    void SetMinMax(float min, float max);
    float GetValue();
};

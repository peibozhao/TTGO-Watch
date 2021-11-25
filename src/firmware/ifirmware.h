
#pragma once

class IFirmware {
public:
    virtual void Setup() = 0;

    virtual void Loop() = 0;
};

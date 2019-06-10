#pragma once

#include "json.hpp"
#include "DatabaseFunctions.h"


class InputReader {

public:
    InputReader();

    void readInit();
    void readApp();

    DatabaseFunctions dbfun;

private:


};



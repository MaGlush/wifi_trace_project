#pragma once

#include "includes.hpp"

// #ASU add autocontrast to header 

Image PostProcessing(Image src_image){
    Image res_image = autocontrast();
    return res_image;
}

// autocontrast realization from CG-1

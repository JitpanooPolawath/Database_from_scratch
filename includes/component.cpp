#include "component.h"


uint32_t swapEndian32(uint32_t val){
    return 
        ((0xFF000000 & val) >> 24) |
        ((0x00FF0000 & val) >> 8) |
        ((0x0000FF00 & val) << 8) |
        ((0x000000FF & val) << 24);
}
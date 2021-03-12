#ifndef __PFS_SIMULATOR_HPP__
#define __PFS_SIMULATOR_HPP__

#include <bits/stdc++.h>


class PFS_mask_handler
{
private:

public:
#if ULLLONG_MAX == 0xffffffffffffffffffffffffffffffff
    uint128_t ssa0_mask = ULLLONG_MAX;
    uint128_t ssa1_mask = 0;
#elif ULLONG_MAX == 0xffffffffffffffff
    uint64_t ssa0_mask = ULLONG_MAX;
    uint64_t ssa1_mask = 0;
#elif ULONG_MAX == 0xffffffff
    uint32_t ssa0_mask = ULONG_MAX;
    uint32_t ssa1_mask = 0;
#else
    uint16_t ssa0_mask = 0xFFFF;
    uint16_t ssa1_mask = 0;
#endif
};


#endif
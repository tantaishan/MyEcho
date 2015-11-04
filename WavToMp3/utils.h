#define mul(x, y)  ((x>>16)*(y>>16)+(((x>>16)*((y&0xffff)))>>16)+(((y>>16)*((x&0xffff)))>>16))/* Left justified fractional multiply. */
#define muls(x, y)  (((x>>16)*(y>>16)<<1)+(((x>>16)*((y&0xffff)))>>15)+(((y>>16)*((x&0xffff)))>>15))
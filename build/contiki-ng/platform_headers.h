#ifndef PLATFORM_HEADERS_H_
#define PLATFORM_HEADERS_H_

/*#if TARGET == "srf06-cc26xx"
#if BOARD == "sensortag/cc2650"
    #include "target/srf06-cc26xx/sensortag/cc2650/memory_cc2650.h"
    #include "target/srf06-cc26xx/sensortag/cc2650/loader.h"
    #include "connection/connection_ercoap.h"
#endif
#endif

#if TARGET == "zoul"
#if BOARD == "firefly"*/
    #include "target/zoul/firefly-reva/memory_firefly.h"
    #include "connection/connection_ercoap.h"
/*#endif
#endif*/

#endif /* PLATFORM_HEADERS_H_ */

#Libpull Agents

Libpull is a library that exposes all the features to create a complete update system. It already includes an update agent and a bootloader, but it can be used to build your own if necessary. In the next sections we will describe the agents included in the library.

## Update Agents

The update agent is the application using the libpull library to effectively perform the update. It is in charge of communicating with the network and coordinate the operations to successfully download, verify and apply the update image. It should be normally executed in parallel to the standard application. In this way, when an update is available, the device will require the minimum time to obtain it. 

The update agent defines all the configurations of the library, such as the endpoint used, the resources that must be accessed by the subscriber and the receiver, the type of connection that must be used, the polling timeout. Moreover, it must be able to recover from errors, and safely fail if the errors are not recoverable. For this reason, the update agent has been implemented as a while loop that exits only if an unrecoverable error has been encountered or the update process is successful.

![update_agent_workflow](https://github.com/libpull/libpull_graphics/raw/master/documentation/update_agent_flow.png)

## Bootloader

The first execution of the bootloader is called bootstrap. During the bootstrap the memory objects are erased, and if the recovery image is enabled, the running object is stored into a specific memory object, allowing a fast recovery in case of failures. 

To recognize the first run from the other runs, the bootloader needs to store its state in a persistent memory. This is done defining a new memory object, called bootloader_ctx, that is stored in the last page of the internal memory and contains a bit, used to indicate if the bootloader is running for the first time. The bootloader_ctx is generated during the building phase and flashed to the board at the correct offset. The *first_run* bit is initially set to one and can be only set to 0 once by the bootloader since that memory is write protected.

If the bootloader finds a newest version in a memory object compared to the version of the running one, it verifies the signature of that object and, if valid, copies it to the running object. The verification is performed before the copy, since if the signature is invalid it avoids a write cycle.

An important operation performed by the bootloader is to write protect all the sectors of the Flash before loading the image, preventing in this way the update agent and all the software running after the bootloader from modifying the content of the internal memory and prevents an attacker to store persistent data in the internal Flash.

![bootloader_exeuction_scheme](https://github.com/libpull/libpull_graphics/raw/master/documentation/bootloader_execution_scheme.png)

## Build your own agent

If the agents included in the library are not sufficient for your application, you can still use the functions provided by the library to create your own update agent or bootloader that follows your specific logic.

To do that you may want to follow the [API Documentation](../api/api.html)

set(FREERTOS_SOURCE_DIR ${DEPS_ROOT}/FreeRTOS/FreeRTOS/Source)
set(FREERTOS_PORT_DIR ${FREERTOS_SOURCE_DIR}/portable/GCC/ARM_CM7/r0p1)

set(FREERTOS_SOURCES
    ${FREERTOS_SOURCE_DIR}/tasks.c
    ${FREERTOS_SOURCE_DIR}/queue.c
    ${FREERTOS_SOURCE_DIR}/list.c
    ${FREERTOS_SOURCE_DIR}/timers.c
    # From https://www.FreeRTOS.org/a00111.html
    # + heap_4:
    #    + Can be used even when the application repeatedly deletes tasks,
    #      queues, semaphores, mutexes, etc..
    #    + Is much less likely than the heap_2 implementation to result in a heap
    #      space that is badly fragmented into multiple small blocks - even when
    #      the memory being allocated and freed is of random size.
    #    + Is not deterministic - but is much more efficient that most standard
    #      C library malloc implementations.
    # NOTE should only be used when configSUPPORT_DYNAMIC_ALLOCATION=1
    # TODO compare heap_5, use FreeRTOS-kernel and set port/heap using
    # recommended mechanisms like (e.g. set(FREERTOS_PORT "TEMPLATE" CACHE STRING "" FORCE))

    ${FREERTOS_SOURCE_DIR}/portable/MemMang/heap_4.c
    ${FREERTOS_PORT_DIR}/port.c
    # Application specific rtos implementations
    # ${PROJECT_ROOT}/Base/config/rtos.c
)

set(FREERTOS_INCLUDES
    ${FREERTOS_SOURCE_DIR}/include
    ${FREERTOS_PORT_DIR}
    # Sets our freeRTOS config
    ${CONFIG_ROOT}/third_party/peak/
)

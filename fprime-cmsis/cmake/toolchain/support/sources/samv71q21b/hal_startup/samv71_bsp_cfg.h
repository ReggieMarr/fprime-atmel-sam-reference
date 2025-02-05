#ifndef SAMV71_BSP_CFG_H_
#define SAMV71_BSP_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pioConfig.h"
#include "sam.h"

void initPio(const pioGPIOConfig_t* configs, uint32_t numConfigs);
void applyPinCfgToPortCfg(pioGPIOConfig_t const* cfg, uint32_t const pinMask, pio_registers_t* port);
#ifdef __cplusplus
}
#endif

#endif /* SAMV71Q_BSP_CFG_H_ */

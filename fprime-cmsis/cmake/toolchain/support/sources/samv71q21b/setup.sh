# Create a folder for packs and navigate into it
export CMSIS_PACK_ROOT="$(pwd)/samv71_blink/packs"
mkdir -p samv71_blink/packs && cd samv71_blink/packs

# Initialize CMSIS pack repository
cpackget init

# Add the required CMSIS packs
cpackget add ARM::CMSIS@5.8.0
cpackget add Microchip::SAMV71_DFP@4.10.230
cpackget add ARM::CMSIS-FreeRTOS@11.1.0
cpackget add ARM::CMSIS-Driver@2.10.0
cpackget add ARM::CMSIS-View@1.2.0
cpackget add ARM::CMSIS-Compiler@2.1.0
cpackget add ARM::Cortex_DFP@1.1.0

# Move back to the project root
cd ..

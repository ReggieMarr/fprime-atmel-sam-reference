####
# AtmelOs:
#
# The Arudino Os support layer uses the F prime baremetal virtualization technology and a custom
# rebuild of the Atmel framework in order to ensure that all the code can compile into a sensible
# binary. This file sets up the Os layer to support all of that.
####
restrict_platforms(AtmelFw)
add_custom_target("${FPRIME_CURRENT_MODULE}")

# Set up Atmel implementations
add_fprime_supplied_os_module(Console Atmel)
add_fprime_supplied_os_module(RawTime Atmel)

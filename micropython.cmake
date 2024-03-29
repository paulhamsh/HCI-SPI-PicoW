# Create an INTERFACE library for our C module.
add_library(usermod_hci_spi_pico_w INTERFACE)

# Add our source files to the lib
target_sources(usermod_hci_spi_pico_w INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/hci_spi_pico_w.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_hci_spi_pico_w INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_hci_spi_pico_w)

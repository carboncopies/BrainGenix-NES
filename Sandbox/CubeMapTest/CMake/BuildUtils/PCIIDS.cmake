##########################
# Builds Library For ERS #
##########################

ERSBuildLogger(${Green} "Configuring PCIIDS Library")
set(INFOWARE_PCI_IDS_PATH ${LIB_DIR}/pciids/pci.ids)
include_directories(${LIB_DIR}/infoware/include)
add_subdirectory(${LIB_DIR}/infoware)
disable_target_warnings(infoware)
ERSBuildLogger(${BoldGreen} "Finished Configuring PCIIDS Library")
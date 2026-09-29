set(DEPENDENT_MP_BIN2HEXble_multimeter_default_A0IDZN0q "c:/Program Files/Microchip/xc32/v5.10/bin/xc32-bin2hex.exe")
set(DEPENDENT_DEPENDENT_TARGET_ELFble_multimeter_default_A0IDZN0q ${CMAKE_CURRENT_LIST_DIR}/../../../../out/ble_multimeter/default.elf)
set(DEPENDENT_TARGET_DIRble_multimeter_default_A0IDZN0q ${CMAKE_CURRENT_LIST_DIR}/../../../../out/ble_multimeter)
set(DEPENDENT_BYPRODUCTSble_multimeter_default_A0IDZN0q ${DEPENDENT_TARGET_DIRble_multimeter_default_A0IDZN0q}/${sourceFileNameble_multimeter_default_A0IDZN0q}.c)
add_custom_command(
    OUTPUT ${DEPENDENT_TARGET_DIRble_multimeter_default_A0IDZN0q}/${sourceFileNameble_multimeter_default_A0IDZN0q}.c
    COMMAND ${DEPENDENT_MP_BIN2HEXble_multimeter_default_A0IDZN0q} --image ${DEPENDENT_DEPENDENT_TARGET_ELFble_multimeter_default_A0IDZN0q} --image-generated-c ${sourceFileNameble_multimeter_default_A0IDZN0q}.c --image-generated-h ${sourceFileNameble_multimeter_default_A0IDZN0q}.h --image-copy-mode ${modeble_multimeter_default_A0IDZN0q} --image-offset ${addressble_multimeter_default_A0IDZN0q} 
    WORKING_DIRECTORY ${DEPENDENT_TARGET_DIRble_multimeter_default_A0IDZN0q}
    DEPENDS ${DEPENDENT_DEPENDENT_TARGET_ELFble_multimeter_default_A0IDZN0q})
add_custom_target(
    dependent_produced_source_artifactble_multimeter_default_A0IDZN0q 
    DEPENDS ${DEPENDENT_TARGET_DIRble_multimeter_default_A0IDZN0q}/${sourceFileNameble_multimeter_default_A0IDZN0q}.c
    )

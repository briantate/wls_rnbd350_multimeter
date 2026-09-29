include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(ble_multimeter_default_library_list )

# Handle files with suffix s, for group default-XC32
if(ble_multimeter_default_default_XC32_FILE_TYPE_assemble)
add_library(ble_multimeter_default_default_XC32_assemble OBJECT ${ble_multimeter_default_default_XC32_FILE_TYPE_assemble})
    ble_multimeter_default_default_XC32_assemble_rule(ble_multimeter_default_default_XC32_assemble)
    list(APPEND ble_multimeter_default_library_list "$<TARGET_OBJECTS:ble_multimeter_default_default_XC32_assemble>")

endif()

# Handle files with suffix S, for group default-XC32
if(ble_multimeter_default_default_XC32_FILE_TYPE_assembleWithPreprocess)
add_library(ble_multimeter_default_default_XC32_assembleWithPreprocess OBJECT ${ble_multimeter_default_default_XC32_FILE_TYPE_assembleWithPreprocess})
    ble_multimeter_default_default_XC32_assembleWithPreprocess_rule(ble_multimeter_default_default_XC32_assembleWithPreprocess)
    list(APPEND ble_multimeter_default_library_list "$<TARGET_OBJECTS:ble_multimeter_default_default_XC32_assembleWithPreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC32
if(ble_multimeter_default_default_XC32_FILE_TYPE_compile)
add_library(ble_multimeter_default_default_XC32_compile OBJECT ${ble_multimeter_default_default_XC32_FILE_TYPE_compile})
    ble_multimeter_default_default_XC32_compile_rule(ble_multimeter_default_default_XC32_compile)
    list(APPEND ble_multimeter_default_library_list "$<TARGET_OBJECTS:ble_multimeter_default_default_XC32_compile>")

endif()

# Handle files with suffix cpp, for group default-XC32
if(ble_multimeter_default_default_XC32_FILE_TYPE_compile_cpp)
add_library(ble_multimeter_default_default_XC32_compile_cpp OBJECT ${ble_multimeter_default_default_XC32_FILE_TYPE_compile_cpp})
    ble_multimeter_default_default_XC32_compile_cpp_rule(ble_multimeter_default_default_XC32_compile_cpp)
    list(APPEND ble_multimeter_default_library_list "$<TARGET_OBJECTS:ble_multimeter_default_default_XC32_compile_cpp>")

endif()

# Handle files with suffix [cC], for group default-XC32
if(ble_multimeter_default_default_XC32_FILE_TYPE_dependentObject)
add_library(ble_multimeter_default_default_XC32_dependentObject OBJECT ${ble_multimeter_default_default_XC32_FILE_TYPE_dependentObject})
    ble_multimeter_default_default_XC32_dependentObject_rule(ble_multimeter_default_default_XC32_dependentObject)
    list(APPEND ble_multimeter_default_library_list "$<TARGET_OBJECTS:ble_multimeter_default_default_XC32_dependentObject>")

endif()


# Main target for this project
add_executable(ble_multimeter_default_image_A0IDZN0q ${ble_multimeter_default_library_list})

set_target_properties(ble_multimeter_default_image_A0IDZN0q PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${ble_multimeter_default_output_dir}")
target_link_libraries(ble_multimeter_default_image_A0IDZN0q PRIVATE ${ble_multimeter_default_default_XC32_FILE_TYPE_link})
# Add the link options from the rule file.
ble_multimeter_default_link_rule( ble_multimeter_default_image_A0IDZN0q)

# Add bin2hex target for converting built file to a .hex file.
string(REGEX REPLACE [.]elf$ .hex ble_multimeter_default_image_name_hex ${ble_multimeter_default_image_name})
add_custom_target(ble_multimeter_default_Bin2Hex ALL
    COMMAND ${MP_BIN2HEX} \"${ble_multimeter_default_output_dir}/${ble_multimeter_default_image_name}\"
    BYPRODUCTS ${ble_multimeter_default_output_dir}/${ble_multimeter_default_image_name_hex}
    COMMENT "Convert built file to .hex")
add_dependencies(ble_multimeter_default_Bin2Hex ble_multimeter_default_image_A0IDZN0q)





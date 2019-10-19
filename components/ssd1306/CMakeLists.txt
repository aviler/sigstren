set( PROJECT tryme )
# set headers

message( "some" )
message( ${CMAKE_CURRENT_SOURCE_DIR} )
message( "THING" )

set(header_path ${CMAKE_CURRENT_SOURCE_DIR})
set(header ${header_path}/src
            ${header_path}/src/ssd1306_hal
            ${header_path}/src/ssd1306_hal/esp
            ${header_path}/src/intf
            ${header_path}/src/intf/i2c
            ${header_path}/src/intf/spi
            ${header_path}/src/intf/vga/esp32
            ${header_path}/src/intf/vga
            ${header_path}/src/lcd
            ${header_path}/src/nano_engine)

# set source files
set(src_path "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(c_src ${src_path}/ssd1306_fonts.c
	${src_path}/ssd1306_generic.c
	${src_path}/ssd1306_1bit.c
	${src_path}/ssd1306_8bit.c
	${src_path}/ssd1306_16bit.c
	${src_path}/ssd1306_menu.c
	${src_path}/ssd1306_hal/avr/platform.c
	${src_path}/ssd1306_hal/linux/platform.c
	${src_path}/ssd1306_hal/mingw/platform.c
	${src_path}/ssd1306_hal/stm32/platform.c
	${src_path}/ssd1306_hal/template/platform.c
	${src_path}/intf/i2c/ssd1306_i2c.c
	${src_path}/intf/i2c/ssd1306_i2c_embedded.c
	${src_path}/intf/i2c/ssd1306_i2c_twi.c
	${src_path}/intf/spi/ssd1306_spi.c
	${src_path}/intf/spi/ssd1306_spi_avr.c
	${src_path}/intf/spi/ssd1306_spi_usi.c
	${src_path}/intf/ssd1306_interface.c
	${src_path}/intf/uart/ssd1306_uart_builtin.c
	${src_path}/lcd/lcd_common.c
	${src_path}/lcd/lcd_pcd8544.c
	${src_path}/lcd/lcd_il9163.c
	${src_path}/lcd/lcd_ili9341.c
	${src_path}/lcd/oled_sh1106.c
	${src_path}/lcd/oled_ssd1306.c
	${src_path}/lcd/oled_ssd1325.c
	${src_path}/lcd/oled_ssd1327.c
	${src_path}/lcd/oled_ssd1331.c
	${src_path}/lcd/oled_ssd1351.c
	${src_path}/lcd/oled_template.c
	${src_path}/lcd/vga_monitor.c
	${src_path}/intf/vga/vga.c
	${src_path}/intf/vga/atmega328p/vga128x64.c
	${src_path}/intf/vga/atmega328p/vga96x40.c
	${src_path}/ssd1306_uart.c)

set(cpp_src ${src_path}/nano_engine/canvas.cpp
	${src_path}/nano_engine/core.cpp
	${src_path}/nano_gfx.cpp
	${src_path}/sprite_pool.cpp
	${src_path}/ssd1306_console.cpp
	${src_path}/ssd1306_hal/arduino/platform.cpp
	${src_path}/intf/vga/esp32/vga128x64.cpp
	${src_path}/intf/vga/esp32/CompositeOutput.cpp)

#set_source_files_properties(cpp_src PROPERTIES LANGUAGE CXX)

set(src ${c_src} ${cpp_src})

#add_library(ssd1306 ${src})

idf_component_register(SRCS ${src}
                    INCLUDE_DIRS ${header})
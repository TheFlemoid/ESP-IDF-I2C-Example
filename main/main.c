#include <stdio.h>
#include "HD44780.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"

#define I2C_MASTER_SCL_IO    22    // GPIO 22 for I2C SCL
#define I2C_MASTER_SDA_IO    21    // GPIO 21 for I2C SDA

#define ADXL345_SENSOR_ADDR  0x53  // I2C address for ADXL345 accelerometer on GY85 9-DOF module 
#define ADXL345_POW_CTL_REG  0x2D  // AXDL345 Power Control Register (sensor comes up in sleep mode, set to monitor)
#define ADXL345_DATAX0       0x32  // X-Axis Data 0
#define ADXL345_DATAX1       0x33  // X-Axis Data 1
#define ADXL345_DATAY0       0x34  // Y-Axis Data 0
#define ADXL345_DATAY1       0x35  // Y-Axis Data 1
#define ADXL345_DATAZ0       0x36  // Z-Axis Data 0
#define ADXL345_DATAZ1       0x37  // Z-Axis Data 1

// Global variable definition
i2c_master_bus_config_t i2cConfig = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};

i2c_device_config_t adxl345Config = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = ADXL345_SENSOR_ADDR,
    .scl_speed_hz = 100000,
};

i2c_master_bus_handle_t i2cBusHandle;
i2c_master_dev_handle_t adxlSensorHandle;

static uint32_t ONE_HUNDRED_MILLI_DELAY = (100 / portTICK_PERIOD_MS);
static uint32_t TWO_HUNDRED_FIFTY_MILLI_DELAY = (250 / portTICK_PERIOD_MS);

// Function predefinition
void setup_i2c();
void setup_accel_sensor();
void read_x_axis();
void read_y_axis();
void read_z_axis();

/**
 * Main function
 */
void app_main() {
    // Setup the character display
    HD44780_FOUR_BIT_BUS bus = { 2, 16, 25, 26, 27, 32, 17, 19 };
    HD44780_initFourBitBus(&bus);
    HD44780_clear();

    setup_i2c();
    setup_accel_sensor();

    vTaskDelay(ONE_HUNDRED_MILLI_DELAY);

    while (1) {
        read_x_axis();
        read_y_axis();
        read_z_axis();
        vTaskDelay(TWO_HUNDRED_FIFTY_MILLI_DELAY);
    }
}

/**
 * Initializes the I2C bus using the global I2C config
 * NOTE: GPIO 21 and 22 are the standard pins for I2C SDA 
 *       and SCL respectively.
 */
void setup_i2c() {
    // Step 1: Define the I2C bus configuration, and run i2c_new_master_bus to get an I2C Bus Handle
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2cConfig, &i2cBusHandle));

    // Step 2 (Optional): Probe the expected I2C slave devices to verify that the I2C bus handle/wiring is working
    ESP_ERROR_CHECK(i2c_master_probe(i2cBusHandle, ADXL345_SENSOR_ADDR, -1));

    // Step 3: Setup I2C device configs for each slave device, and run i2c_master_bus_add_device to get I2C slave
    //         device handles for each slave device.  This is what you'll primarily use to talk to the device.
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2cBusHandle, &adxl345Config, &adxlSensorHandle));
}

/**
 * Sets up the ADXL345 accelerometer sensor
 * NOTE: The sensor powers on in "sleep mode" so this function is simply
 *       setting the deivce into "measure mode" via the POWER_CTL register
 */
void setup_accel_sensor() {
    uint8_t monitor_cmd[2] = {ADXL345_POW_CTL_REG, 0x08};
    ESP_ERROR_CHECK(i2c_master_transmit(adxlSensorHandle, monitor_cmd, 2, -1));
}

/**
 * Reads the two x-axis accelerometer registers, converts the contents to a signed
 * integer, and writes the contents to the HD44780 display.
 */
void read_x_axis() {
    // Double write both X registers, indicating we want to read both X registers.
    uint8_t x_axis_reg[2] = {ADXL345_DATAX0, ADXL345_DATAX1};
    ESP_ERROR_CHECK(i2c_master_transmit(adxlSensorHandle, x_axis_reg, 2, -1));

    // Read both X registers
    uint8_t reg_data[2];
    ESP_ERROR_CHECK(i2c_master_receive(adxlSensorHandle, reg_data, 2, -1));

    // Result is a signed 16 bit number, but is stored as two uint_8t
    // We need to push them into one 'unsigned' 16 bit integer, and then
    // do signed/unsigned conversion to correctly show +/- 0.
    unsigned int uns_result = (reg_data[1] << 8) | reg_data[0];
    int result = (int16_t) uns_result;

    // Blank out the last two digits in case it spilled over
    HD44780_setCursorPos(6, 0);
    HD44780_print("  ");
    
    HD44780_homeCursor();
    char xAxis[10];
    sprintf(xAxis, "x:%.02f", result / 256.0);
    HD44780_print(xAxis);
}

/**
 * Reads the two y-axis accelerometer registers, converts the contents to a signed
 * integer, and writes the contents to the HD44780 display.
 */
void read_y_axis() {
    uint8_t y_axis_reg[2] = {ADXL345_DATAY0, ADXL345_DATAY1};
    ESP_ERROR_CHECK(i2c_master_transmit(adxlSensorHandle, y_axis_reg, 2, -1));

    uint8_t reg_data[2];
    ESP_ERROR_CHECK(i2c_master_receive(adxlSensorHandle, reg_data, 2, -1));

    // Result is a signed 16 bit number, but is stored as two uint_8t
    // We need to push them into one 'unsigned' 16 bit integer, and then
    // do signed/unsigned conversion to correctly show +/- 0.
    unsigned int uns_result = (reg_data[1] << 8) | reg_data[0];
    int result = (int16_t) uns_result;

    // Blank out the last two digits in case it spilled over
    HD44780_setCursorPos(14, 0);
    HD44780_print("  ");

    HD44780_setCursorPos(8, 0);
    char yAxis[10];
    sprintf(yAxis, "y:%.02f", result / 256.0);
    HD44780_print(yAxis);
}

/**
 * Reads the two z-axis accelerometer registers, converts the contents to a signed
 * integer, and writes the contents to the HD44780 display.
 */
void read_z_axis() {
    uint8_t z_axis_reg[2] = {ADXL345_DATAZ0, ADXL345_DATAZ1};
    ESP_ERROR_CHECK(i2c_master_transmit(adxlSensorHandle, z_axis_reg, 2, -1));

    uint8_t reg_data[2];
    ESP_ERROR_CHECK(i2c_master_receive(adxlSensorHandle, reg_data, 2, -1));

    // Result is a signed 16 bit number, but is stored as two uint_8t
    // We need to push them into one 'unsigned' 16 bit integer, and then
    // do signed/unsigned conversion to correctly show +/- 0.
    unsigned int uns_result = (reg_data[1] << 8) | reg_data[0];
    int result = (int16_t) uns_result;

    // Blank out the last two digits in case it spilled over
    HD44780_setCursorPos(6, 1);
    HD44780_print("  ");

    HD44780_setCursorPos(0, 1);
    char zAxis[10];
    sprintf(zAxis, "z:%.02f", result / 256.0);
    HD44780_print(zAxis);
}

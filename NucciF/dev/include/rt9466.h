/*!****************************************************************************
 * @file		rt9466.h
 * @author		d_el
 * @version		V1.0
 * @date		Oct 3, 2025
 * @copyright	License (MIT). Copyright (c) 2025 Storozhenko Roman
 * @brief
 */

#ifndef rt9466_H
#define rt9466_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include <stdint.h>
#include <stddef.h>

/*!****************************************************************************
 * Function declaration
 */
class Rt9466{
private:
	using i2cRead_t = bool (*)(uint8_t devAddr, uint8_t* dst, size_t len, uint16_t timeout);
	using i2cWrite_t = bool (*)(uint8_t devAddr, const uint8_t* src, size_t len, bool needstop, uint16_t timeout);

public:
	using ch_status_t = struct {
		uint8_t adc_in_conversion :1;
		uint8_t reserved :1;
		uint8_t vbus_ovp :1;
		uint8_t boost_stat :1;
		uint8_t vbat_trickle :1;
		enum {
			pre_charge,
			fast_charge
		}vbat_lvl :1;
		enum {
			ready,
			in_progress,
			charge_done,
			fault
		}chg_stat :2;
	};

	using adc_ch = enum: uint8_t {
		Reserved = 0,
		VBUS_div5,
		VBUS_div2,
		VSYS,
		VBAT,
		Reserved1,
		TS_BAT,
		Reserved2,
		IBUS,
		IBAT,
		Reserved3,
		REGN,
		TEMP_JC
	};


public:
	Rt9466(i2cRead_t read=nullptr, i2cWrite_t write=nullptr);
	void setI2c(i2cRead_t read, i2cWrite_t write);
	bool init(void);
	bool get_status(ch_status_t *chgstat);
	bool adc_start(adc_ch);
	bool adc_read(uint16_t* val);
	bool batteryRegulationVoltageSet(uint16_t voltage);	// X_XX, 3.90V - 4.71V
	bool chargingRegulationCurrentSet(uint8_t current);	// X_X, 0.1A - 5.0A

private:
	bool read_reg(uint8_t reg, uint8_t* val);
	bool write_reg(uint8_t reg, uint8_t val);

private:
	i2cRead_t m_read;
	i2cWrite_t m_write;
};

#ifdef __cplusplus
}
#endif

#endif //rt9466_H
/******************************** END OF FILE ********************************/

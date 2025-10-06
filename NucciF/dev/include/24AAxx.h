/*!****************************************************************************
 * @file		24AAxx.h
 * @author		d_el
 * @version		V1.0
 * @date		21.12.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver for eeprom memory 24AA04, 24AA08
 */
#ifndef e24AAxx_H
#define e24AAxx_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
* Include
*/
#include <stdint.h>
#include <stddef.h>

/*!****************************************************************************
* Prototypes for the functions
*/
class Eep24AA{
private:
	using i2cRead_t = bool (*)(uint8_t devAddr, uint8_t* dst, size_t len, uint16_t timeout);
	using i2cWrite_t = bool (*)(uint8_t devAddr, const uint8_t* src, size_t len, bool needstop, uint16_t timeout);

public:
	Eep24AA(i2cRead_t read=nullptr, i2cWrite_t write=nullptr);
	void setI2c(i2cRead_t read, i2cWrite_t write);
	void init(void);
	bool write(uint16_t dst, const void *src, uint16_t len);
	bool read(void *dst, uint16_t src, uint16_t len);

private:
	i2cRead_t m_read;
	i2cWrite_t m_write;
};

#ifdef __cplusplus
}
#endif

#endif //e24AAxx_H
/******************************** END OF FILE ********************************/

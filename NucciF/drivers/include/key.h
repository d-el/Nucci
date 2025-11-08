/*!****************************************************************************
 * @file		key.h
 * @author		d_el
 * @version		V1.0
 * @date		07.06.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver for keyboard
 */
#ifndef key_H
#define key_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Define
 */
#define KEY_NUM				( 7 )
#define KEY_SAMPLES			( 2 )

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	kFunc =		(1 << 0),
	kNext =		(1 << 1),
	kZero =		(1 << 2),
	kUp =		(1 << 3),
	kDown =		(1 << 4),
} kKey_type;

typedef struct {
	uint32_t dInState;
	uint32_t dInPrevState;
	uint32_t keyState;
	uint32_t longPrevState;
	uint32_t longState;
	uint32_t repeatKeyMask;
	uint32_t longKeyMask;
	uint32_t lockKeyMask;
	uint8_t toFirstRepeat;
	uint8_t repeat2repeat;
	uint32_t toLong;
	uint8_t dInFilterCnt[KEY_NUM];
	uint8_t toFirstReiterationCnt[KEY_NUM];
	uint8_t toReiterationCnt[KEY_NUM];
	uint8_t longCnt[KEY_NUM];
} key_type;

/*!****************************************************************************
 * Exported variables
 */
extern key_type key;

/*!****************************************************************************
 * Function declaration
 */
uint32_t keyProc(void);
uint32_t keyState(kKey_type keyMask);
uint32_t keyStateLong(kKey_type keyMask);
uint32_t keyDin(kKey_type keyMask);
void ksSet(uint16_t toFirstReiteration, uint16_t toReiteration, uint32_t reiterationKeyMask, uint16_t toLongCnt, uint32_t longKeyMask);
void keyAddReiteration(uint32_t reiterationKeyMask);
void keyClearReiteration(uint32_t reiterationKeyMask);
void keyWaitUnpress(uint32_t mask);
#ifdef __cplusplus
}
#endif

#endif //key_H
/******************************** END OF FILE ********************************/

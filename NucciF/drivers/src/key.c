/*!****************************************************************************
 * @file		key.c
 * @author		d_el
 * @version		V1.0
 * @date		07.06.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver for keyboard
 */

/*!****************************************************************************
 * Include
 */
#include "gpio.h"
#include "key.h"

/*!****************************************************************************
 * Memory
 */
key_type key;

/*!****************************************************************************
 * @brief
 */
void dInUpdate(key_type *pkey, uint32_t num, uint32_t val){
	if(val != 0){
		if(pkey->dInFilterCnt[num] < KEY_SAMPLES){
			pkey->dInFilterCnt[num]++;
		}else{
			pkey->dInState |= 1U << num;
		}
	}else{
		if(pkey->dInFilterCnt[num] > 0){
			pkey->dInFilterCnt[num]--;
		}else{
			pkey->dInState &= ~(1U << num);
		}
	}
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
uint32_t keyProc(void){
	key_type *pkey = &key;
	uint32_t mask;
	uint32_t iterator = 0;

	//Physical buttons
	dInUpdate(pkey, 0, !gppin_get(GP_bView));
	dInUpdate(pkey, 1, !gppin_get(GP_bNext));
	dInUpdate(pkey, 2, !gppin_get(GP_bZero));
	dInUpdate(pkey, 3, !gppin_get(GP_bUp));
	dInUpdate(pkey, 4, !gppin_get(GP_bDown));

	//Detect signal front
	for(mask = 1 << 0; mask < (1 << KEY_NUM); mask <<= 1){
		if((pkey->dInState & mask) == 0){
			pkey->lockKeyMask &= ~mask;
		}
		if(pkey->lockKeyMask & mask){
			key.keyState &= ~mask;
			key.longState &= ~mask;
			continue;
		}

		// Key state
		if(((pkey->dInPrevState & mask) == 0) && ((pkey->dInState & mask) != 0)){
			pkey->keyState |= mask;
		}else{
			pkey->keyState &= ~mask;
		}

		// Key repeat
		if((pkey->repeatKeyMask & mask) != 0){
			if((pkey->dInState & mask) != 0){
				if(pkey->toFirstReiterationCnt[iterator] < pkey->toFirstRepeat){
					pkey->toFirstReiterationCnt[iterator]++;
				}else{
					if(pkey->toReiterationCnt[iterator] < pkey->repeat2repeat){
						pkey->toReiterationCnt[iterator]++;
					}else{
						pkey->keyState |= mask;
						pkey->toReiterationCnt[iterator] = 0;
					}
				}
			}else{
				pkey->toFirstReiterationCnt[iterator] = 0;
				pkey->toReiterationCnt[iterator] = 0;
			}
		}

		// Key long
		if((pkey->longKeyMask & mask) != 0){
			if((pkey->dInState & mask) != 0){
				if(pkey->longCnt[iterator] < pkey->toLong){
					pkey->longCnt[iterator]++;
				}else{
					if((pkey->longPrevState & mask) == 0){
						pkey->longState |= mask;
						pkey->longPrevState |= mask;
					}else{
						pkey->longState &= ~mask;
					}
				}
			}else{
				pkey->longState &= ~mask;
				pkey->longPrevState &= ~mask;
				pkey->longCnt[iterator] = 0;
			}

		}
		iterator++;
	}
	pkey->dInPrevState = pkey->dInState;

	return pkey->keyState | pkey->longState;
}

/*!****************************************************************************
 * @param
 */
uint32_t keyState(kKey_type keyMask){
	if((key.keyState & keyMask) != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @param
 */
uint32_t keyStateLong(kKey_type keyMask){
	if((key.longState & keyMask) != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @param
 */
uint32_t keyDin(kKey_type keyMask){
	if((key.dInState & keyMask) != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @param
 */
void ksSet(uint16_t toFirstRepeat, uint16_t repeat2repeat, uint32_t repeatKeyMask, uint16_t toLong, uint32_t longKeyMask){
	key.toFirstRepeat = toFirstRepeat;
	key.repeat2repeat = repeat2repeat;
	key.repeatKeyMask = repeatKeyMask;
	key.toLong = toLong;
	key.longKeyMask = longKeyMask;
}

/*!****************************************************************************
 * @param
 */
void keyAddReiteration(uint32_t repeatKeyMask){
	key.repeatKeyMask = repeatKeyMask;
}

/*!****************************************************************************
 * @param
 */
void keyClearReiteration(uint32_t repeatKeyMask){
	key.repeatKeyMask &= ~repeatKeyMask;
}

/*!****************************************************************************
 * @param
 */
void keyWaitUnpress(uint32_t mask){
	key.lockKeyMask = mask;
}

/******************************** END OF FILE ********************************/

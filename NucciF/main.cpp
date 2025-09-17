/*!******************************************************************************
 *                                                                              *
 *                                                                              *
 * @file        main.cpp                                                        *
 * @author      Storozhenko Roman - D_EL                                        *
 * @date        10.03.2025                                                      *
 * @copyright   The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman     *
 * @mainpage    Main program body                                               *
 ********************************************************************************/

/*!****************************************************************************
 * Include
 */
#include <drivers.h>
#include <systemTSK.h>

/*!****************************************************************************
 *
 */
int main(void){
	hardInit();
	OSinit();
	while(1);
	return 0;
}

/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file		htmlPage.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		20.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include <string.h>
#include <systemTSK.h>
#include <version.h>
#include <prmSystem.h>
#include "htmlPage.h"

/*!****************************************************************************
 * Local prototypes for the functions
 */
urlData_type handle_statemeastask(void);

/*!****************************************************************************
 * MEMORY
 */
typedef struct __attribute__ ((packed)){
	uint16_t major;
	uint16_t minor;
	uint16_t patch;

	int16_t tubeVoltage;	///< [X V]
	uint32_t countTime;		///< [s]
	uint32_t radVal_uRph;	///< [uR/h]
	uint32_t rad_uR;		///< [uR]
	int16_t input_voltage;	///< [X_XX V]
	int16_t temperature;	///< [X_X °С]
	uint16_t status;
} state_t;

state_t bin_statemeastask;

#define resource_reg(resource) \
	extern const char *const _binary_##resource##_start; \
	extern const char *const _binary_##resource##_end; \
	extern const size_t _binary_##resource##_size;

#define resource_add(type, resource) \
		{ &_binary_##resource##_start, \
		(size_t)&_binary_##resource##_size, \
		type }

resource_reg(net_resource_index_html)
resource_reg(net_resource_overall_js)
resource_reg(net_resource_styles_css)
resource_reg(net_resource_favicon_png)
resource_reg(net_resource_404_html)

const httpResource_type httpResource[] = {
	{
		.url = "/ ",
		.data = resource_add(urlDataType_html, net_resource_index_html),
		.handler = NULL
	},
	{
		"/overall.js",
		.data = resource_add(urlDataType_js, net_resource_overall_js),
		.handler = NULL
	},
	{
		.url = "/overall.js",
		.data = resource_add(urlDataType_js, net_resource_overall_js),
		.handler = NULL
	},
	{
		.url = "/styles.css",
		.data = resource_add(urlDataType_css, net_resource_styles_css),
		.handler = NULL
	},
	{
		.url = "/statemeastask.bin",
		.data = { nullptr, sizeof(bin_statemeastask), urlDataType_bin },
		.handler = handle_statemeastask
	},
	{
		.url = "/favicon.png",
		.data = resource_add(urlDataType_ico, net_resource_favicon_png),
		.handler = NULL
	},
	{
		.url = "/404",
		.data = resource_add(urlDataType_html, net_resource_404_html),
		.handler = NULL
	},
};

const uint8_t getUrlNumber = sizeof(httpResource) / sizeof(httpResource[0]);

/*!****************************************************************************
 *
 */
urlData_type handle_statemeastask(void){
	static urlData_type urlData;
	bin_statemeastask.tubeVoltage = 391;
	bin_statemeastask.countTime = Prm::countTime.val;
	bin_statemeastask.radVal_uRph = Prm::radVal_uRph.val;
	bin_statemeastask.rad_uR = Prm::rad_uR.val;

	bin_statemeastask.major = getVersionMajor();
	bin_statemeastask.minor = getVersionMinor();
	bin_statemeastask.patch = getVersionPatch();

	urlData.payload = &bin_statemeastask;
	urlData.size = sizeof(bin_statemeastask);
	urlData.type = urlDataType_bin;
	return urlData;
}

/******************************** END OF FILE ********************************/

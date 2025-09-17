/*!****************************************************************************
 * @file		radmonClient.cpp
 * @author		d_el
 * @version		V1.0
 * @date		May 22, 2025
 * @copyright	License (MIT). Copyright (c) 2024 Storozhenko Roman
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "plog.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include <lwip/dns.h>
#include "htmlPage.h"
#include "httpServerTSK.h"
#include <prmSystem.h>

/*!****************************************************************************
 * MEMORY
 */
static const char* radmonHost = "radmon.org";
static const char* userName = "d_el";
static const char* userPass = "del_31415"/*"Ud46WSjKdwKnRab"*/;
#define LOG_LOCAL_LEVEL P_LOG_WARN
static const char *logTag = "radMon";
char buffer[256];

/*!****************************************************************************
 * @brief
 */
bool server_resolve(const char *host, ip_addr_t *address){
	/* initialize SNTP server address */
	P_LOGI(logTag, "dns_gethostbyname, url: %s", host);
	err_t err = dns_gethostbyname(host, address, nullptr, NULL);

	if(err == ERR_INPROGRESS){
		/* DNS request sent, wait for sntp_dns_found being called */
		P_LOGD(logTag, "request: Waiting for server address to be resolved");
		return false;
	}

	if(err == ERR_OK){
		P_LOGD(logTag, "server_address: %s", ipaddr_ntoa(address));
		return true;
	}else{
		/* address conversion failed, try another server */
		P_LOGD(logTag, "request: Invalid server address, trying next server");
		return false;
	}
	return true;
}

bool sentToServer(ip_addr_t* site_ip, char* src, size_t len){
	/* Create a new TCP connection handle */
	struct netconn *conn = netconn_new(NETCONN_TCP);
	if(conn == NULL){
		P_LOGW(logTag, "Error create netconn");
		vTaskDelete(NULL);
	}

	/* Bind to port 80 (HTTP) with default IP address */
	err_t err = netconn_connect(conn, site_ip, 80);
	if(err != ERR_OK){
		P_LOGW(logTag, "Error netconn connect");
		return false;
	}

	P_LOGD(logTag, "req: %s", buffer);
	err = netconn_write(conn, src, len, NETCONN_NOCOPY);
	if(err == ERR_OK){
		P_LOGD(logTag, "Write success");
	}else{
		P_LOGW(logTag, "Write error");
	}
	vTaskDelay(1000); // Wait a little and give the server time to respond.

	struct netbuf *inbuf;
	err = netconn_recv(conn, &inbuf);
	if(err != ERR_OK){
		P_LOGW(logTag, "Error in netconn_recv, %" PRIi8, err);
	}else{ //res == ERR_OK
		char* buf;
		u16_t buflen;
		netbuf_data(inbuf, (void**)&buf, &buflen);
		P_LOGD(logTag, "Netbuf_data: %p (%" PRIu16 ")", buf, buflen);
		P_LOGD(logTag, "ans: %s", buf);
	}
	netbuf_delete(inbuf);
	netconn_disconnect(conn);
	netconn_close(conn);
	netconn_delete(conn);

	return true;
}

size_t httpAddHeader(char* bff, size_t bfflen, const char* function){
	return snprintf(bff, bfflen, "GET /radmon.php?function=%s&user=%s&password=%s", function, userName, userPass);
}

size_t httpAddTail(char* bff, size_t bfflen){
	return snprintf(bff, bfflen," HTTP/1.1\r\n"
								"Host: %s\r\n"
								"User-Agent: STM32 based\r\n"
								"Connection: close\r\n\r\n",
								radmonHost);
}

size_t submit(ip_addr_t* site_ip, char* bff, size_t bfflen, uint32_t cpm){
	//https://radmon.org/radmon.php?function=submit&user=Simomax&password=datasendingpassword&value=100&unit=CPM
	size_t len = 0;
	len += httpAddHeader(&bff[len], bfflen - len, "submit");
	len += snprintf(&bff[len], bfflen - len, "&value=%lu&unit=CPM", cpm);
	len += httpAddTail(&bff[len], bfflen - len);
	sentToServer(site_ip, bff, len);
	return len;
}

size_t setlatitudelongitude(ip_addr_t* site_ip, char* bff, size_t bfflen){
	//https://radmon.org/radmon.php?function=setlatitudelongitude&user=Simomax&password=datasendingpassword&latitude=53.80951&longitude=-3.014777
	size_t len = 0;
	len += httpAddHeader(&bff[len], bfflen - len, "setlatitudelongitude");
	len += snprintf(&bff[len], bfflen - len, "&latitude=49.958105");
	len += snprintf(&bff[len], bfflen - len, "&longitude=36.344180");
	len += httpAddTail(&bff[len], bfflen - len);
	sentToServer(site_ip, bff, len);
	return len;
}

size_t updatedescription(ip_addr_t* site_ip, char* bff, size_t bfflen){
	//https://radmon.org/radmon.php?function=updatedescription&user=Simomax&password=datasendingpassword&description=My%20Station%20Is%20Fantastic!
	size_t len = 0;
	len += httpAddHeader(&bff[len], bfflen - len, "updatedescription");
	len += snprintf(&bff[len], bfflen - len, "&description=STM32%%SBT-10%%tube%%based");
	len += httpAddTail(&bff[len], bfflen - len);
	sentToServer(site_ip, bff, len);
	return len;
}

size_t setconversionfactor(ip_addr_t* site_ip, char* bff, size_t bfflen){
	//https://radmon.org/radmon.php?function=setconversionfactor&user=Simomax&password=datasendingpassword&value=0.00833
	size_t len = 0;
	len += httpAddHeader(&bff[len], bfflen - len, "setconversionfactor");
	len += snprintf(&bff[len], bfflen - len, "&value=0.001655629");
	len += httpAddTail(&bff[len], bfflen - len);
	sentToServer(site_ip, bff, len);
	return len;
}



/*!****************************************************************************
 * @brief
 */
void radmonClientTSK(void* pPrm){
	(void)pPrm;

	while(1){
		P_LOGD(logTag, "Wait network");
		while(!netif_is_link_up(netif_default) && !netif_ip4_addr(netif_default)->addr){
			vTaskDelay(pdMS_TO_TICKS(100));
		}

		ip_addr_t site_ip;
		bool dnsResult;
		do{
			dnsResult = server_resolve(radmonHost, &site_ip);
			vTaskDelay(pdMS_TO_TICKS(1'000));
		}while(!dnsResult);

		updatedescription(&site_ip, buffer, sizeof(buffer));
		setconversionfactor(&site_ip, buffer, sizeof(buffer));
		setlatitudelongitude(&site_ip, buffer, sizeof(buffer));

		while(1){
			if(Prm::countTime.val >= 60){
				submit(&site_ip, buffer, sizeof(buffer), Prm::pulseCountpm.val);
			}
			vTaskDelay(30000);
		}
	}
}

/******************************** END OF FILE ********************************/

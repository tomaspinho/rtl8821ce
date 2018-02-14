#include "halmac_2_platform.h"
#include "halmac_type.h"
#if HALMAC_PLATFORM_WINDOWS == 1
#if HALMAC_8822B_SUPPORT
#include "halmac_88xx/halmac_api_win8822b.h"
#include "halmac_88xx/halmac_win8822b_cfg.h"
#endif
#if HALMAC_8821C_SUPPORT
#include "halmac_88xx/halmac_api_win8821c.h"
#include "halmac_88xx/halmac_win8821c_cfg.h"
#endif

#else
#include "halmac_88xx/halmac_api_88xx.h"
#include "halmac_88xx/halmac_88xx_cfg.h"
#endif

#if HALMAC_8822B_SUPPORT
#include "halmac_88xx/halmac_8822b/halmac_8822b_cfg.h"
#endif
#if HALMAC_8821C_SUPPORT
#include "halmac_88xx/halmac_8821c/halmac_8821c_cfg.h"
#endif

HALMAC_RET_STATUS
halmac_check_platform_api(
	IN VOID *pDriver_adapter,
	IN HALMAC_INTERFACE halmac_interface,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api
);

HALMAC_RET_STATUS
halmac_get_chip_info(
	IN VOID	*pDriver_adapter,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api,
	IN HALMAC_INTERFACE	halmac_interface,
	IN PHALMAC_ADAPTER pHalmac_adapter
);

u8
platform_reg_read_8_sdio(
	IN VOID	*pDriver_adapter,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api,
	IN u32 offset
);

HALMAC_RET_STATUS
plarform_reg_write_8_sdio(
	IN VOID	*pDriver_adapter,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api,
	IN u32 offset,
	IN u8 data
);

HALMAC_RET_STATUS
halmac_convert_to_sdio_bus_offset(
	INOUT u32 *halmac_offset
);

/**
 * halmac_init_adapter() - init halmac_adapter
 * @pDriver_adapter : the adapter of caller
 * @pHalmac_platform_api : the platform APIs which is used in halmac APIs
 * @halmac_interface : bus interface
 * @ppHalmac_adapter : the adapter of halmac
 * @ppHalmac_api : the function pointer of APIs, caller shall call APIs by function pointer
 * Author : KaiYuan Chang / Ivan Lin
 * Return : HALMAC_RET_STATUS
 * More details of status code can be found in prototype document
 */
HALMAC_RET_STATUS
halmac_init_adapter(
	IN VOID	*pDriver_adapter,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api,
	IN HALMAC_INTERFACE	halmac_interface,
	OUT	PHALMAC_ADAPTER *ppHalmac_adapter,
	OUT	PHALMAC_API *ppHalmac_api
)
{
	PHALMAC_ADAPTER pHalmac_adapter = (PHALMAC_ADAPTER)NULL;
	HALMAC_RET_STATUS status = HALMAC_RET_SUCCESS;
	u8 *pBuf = NULL;

#if HALMAC_PLATFORM_WINDOWS == 1
	u8 chip_id = 0;
#endif
	union {
		u32	i;
		u8	x[4];
	} ENDIAN_CHECK = { 0x01000000 };

	status = halmac_check_platform_api(pDriver_adapter, halmac_interface, pHalmac_platform_api);
	if (HALMAC_RET_SUCCESS != status)
		return status;
	pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ALWAYS, HALMAC_SVN_VER "\n");
	pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ALWAYS, "HALMAC_MAJOR_VER = %x\n", HALMAC_MAJOR_VER);
	pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ALWAYS, "HALMAC_PROTOTYPE_VER = %x\n", HALMAC_PROTOTYPE_VER);
	pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ALWAYS, "HALMAC_MINOR_VER = %x\n", HALMAC_MINOR_VER);
	pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ALWAYS, "HALMAC_PATCH_VER = %x\n", HALMAC_PATCH_VER);

	pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_TRACE, "halmac_init_adapter_88xx ==========>\n");

	/* Check endian setting - Little endian : 1, Big endian : 0*/
	if (HALMAC_SYSTEM_ENDIAN == ENDIAN_CHECK.x[0]) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "Endian setting Err!!\n");
		return HALMAC_RET_ENDIAN_ERR;
	}

	pBuf = (u8 *)pHalmac_platform_api->RTL_MALLOC(pDriver_adapter, sizeof(HALMAC_ADAPTER));

	if (pBuf == NULL) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "Malloc HAL Adapter Err!!\n");
		return HALMAC_RET_MALLOC_FAIL;
	}
	pHalmac_platform_api->RTL_MEMSET(pDriver_adapter, pBuf, 0x00, sizeof(HALMAC_ADAPTER));
	pHalmac_adapter = (PHALMAC_ADAPTER)pBuf;

	/* return halmac adapter address to caller */
	*ppHalmac_adapter = pHalmac_adapter;

	/* Record caller info */
	pHalmac_adapter->pHalmac_platform_api = pHalmac_platform_api;
	pHalmac_adapter->pDriver_adapter = pDriver_adapter;
	halmac_interface = (HALMAC_INTERFACE_AXI == halmac_interface) ? HALMAC_INTERFACE_PCIE : halmac_interface;
	pHalmac_adapter->halmac_interface = halmac_interface;

	PLATFORM_MUTEX_INIT(pDriver_adapter, &(pHalmac_adapter->EfuseMutex));
	PLATFORM_MUTEX_INIT(pDriver_adapter, &(pHalmac_adapter->h2c_seq_mutex));

	/*Get Chip*/
	if (HALMAC_RET_SUCCESS != halmac_get_chip_info(pDriver_adapter, pHalmac_platform_api, halmac_interface, pHalmac_adapter)) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "HALMAC_RET_CHIP_NOT_SUPPORT\n");
		return HALMAC_RET_CHIP_NOT_SUPPORT;
	}

	/* Assign function pointer to halmac API */
#if HALMAC_PLATFORM_WINDOWS == 0
	halmac_init_adapter_para_88xx(pHalmac_adapter);
	status = halmac_mount_api_88xx(pHalmac_adapter);
#else

#if HALMAC_8822B_SUPPORT
	if (HALMAC_CHIP_ID_8822B == pHalmac_adapter->chip_id) {
		halmac_init_adapter_para_win8822b(pHalmac_adapter);
		status = halmac_mount_api_win8822b(pHalmac_adapter);
	}
#endif
#if HALMAC_8821C_SUPPORT
	if (HALMAC_CHIP_ID_8821C == pHalmac_adapter->chip_id) {
		halmac_init_adapter_para_win8821c(pHalmac_adapter);
		status = halmac_mount_api_win8821c(pHalmac_adapter);
	}
#endif

#endif

	/* Return halmac API function pointer */
	*ppHalmac_api = (PHALMAC_API)pHalmac_adapter->pHalmac_api;

	PLATFORM_MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_TRACE, "halmac_init_adapter_88xx <==========\n");

	return status;
}

/**
 * halmac_halt_api() - stop halmac_api action
 * @pHalmac_adapter : the adapter of halmac
 * Author : Ivan Lin
 * Return : HALMAC_RET_STATUS
 * More details of status code can be found in prototype document
 */
HALMAC_RET_STATUS
halmac_halt_api(
	IN PHALMAC_ADAPTER pHalmac_adapter
)
{
	VOID *pDriver_adapter = NULL;
	PHALMAC_PLATFORM_API pHalmac_platform_api = (PHALMAC_PLATFORM_API)NULL;

	if (HALMAC_RET_SUCCESS != halmac_adapter_validate(pHalmac_adapter))
		return HALMAC_RET_ADAPTER_INVALID;

	pDriver_adapter = pHalmac_adapter->pDriver_adapter;
	pHalmac_platform_api = pHalmac_adapter->pHalmac_platform_api;

	PLATFORM_MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_TRACE, "halmac_halt_api ==========>\n");
	pHalmac_adapter->halmac_state.api_state = HALMAC_API_STATE_HALT;
	PLATFORM_MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_TRACE, "halmac_halt_api ==========>\n");
	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_deinit_adapter() - deinit halmac adapter
 * @pHalmac_adapter : the adapter of halmac
 * Author : KaiYuan Chang / Ivan Lin
 * Return : HALMAC_RET_STATUS
 * More details of status code can be found in prototype document
 */
HALMAC_RET_STATUS
halmac_deinit_adapter(
	IN PHALMAC_ADAPTER pHalmac_adapter
)
{
	VOID *pDriver_adapter = NULL;

	if (HALMAC_RET_SUCCESS != halmac_adapter_validate(pHalmac_adapter))
		return HALMAC_RET_ADAPTER_INVALID;

	pDriver_adapter = pHalmac_adapter->pDriver_adapter;

	PLATFORM_MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_TRACE, "[TRACE]halmac_deinit_adapter_88xx ==========>\n");

	PLATFORM_MUTEX_DEINIT(pDriver_adapter, &(pHalmac_adapter->EfuseMutex));
	PLATFORM_MUTEX_DEINIT(pDriver_adapter, &(pHalmac_adapter->h2c_seq_mutex));

	if (NULL != pHalmac_adapter->pHalEfuse_map) {
		PLATFORM_RTL_FREE(pDriver_adapter, pHalmac_adapter->pHalEfuse_map, pHalmac_adapter->hw_config_info.efuse_size);
		pHalmac_adapter->pHalEfuse_map = (u8 *)NULL;
	}

	if (NULL != pHalmac_adapter->halmac_state.psd_set.pData) {
		PLATFORM_RTL_FREE(pDriver_adapter, pHalmac_adapter->halmac_state.psd_set.pData, pHalmac_adapter->halmac_state.psd_set.data_size);
		pHalmac_adapter->halmac_state.psd_set.pData = (u8 *)NULL;
	}

	if (NULL != pHalmac_adapter->pHalmac_api) {
		PLATFORM_RTL_FREE(pDriver_adapter, pHalmac_adapter->pHalmac_api, sizeof(HALMAC_API));
		pHalmac_adapter->pHalmac_api = NULL;
	}

	pHalmac_adapter->pHalAdapter_backup = NULL;
	PLATFORM_RTL_FREE(pDriver_adapter, pHalmac_adapter, sizeof(HALMAC_ADAPTER));

	return HALMAC_RET_SUCCESS;
}

HALMAC_RET_STATUS
halmac_check_platform_api(
	IN VOID *pDriver_adapter,
	IN HALMAC_INTERFACE	halmac_interface,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api
)
{
	VOID *pAdapter_Local = NULL;

	pAdapter_Local = pDriver_adapter;

	if (NULL == pHalmac_platform_api)
		return HALMAC_RET_PLATFORM_API_NULL;

	if (NULL == pHalmac_platform_api->MSG_PRINT)
		return HALMAC_RET_PLATFORM_API_NULL;

	if (HALMAC_INTERFACE_SDIO == halmac_interface) {
		if (NULL == pHalmac_platform_api->SDIO_CMD52_READ) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD52_READ)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->SDIO_CMD53_READ_8) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD53_READ_8)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->SDIO_CMD53_READ_16) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD53_READ_16)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->SDIO_CMD53_READ_32) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD53_READ_32)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->SDIO_CMD53_READ_N) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD53_READ_N)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->SDIO_CMD52_WRITE) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD52_WRITE)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->SDIO_CMD53_WRITE_8) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD53_WRITE_8)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->SDIO_CMD53_WRITE_16) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD53_WRITE_16)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->SDIO_CMD53_WRITE_32) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->SDIO_CMD53_WRITE_32)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
	}

	if ((HALMAC_INTERFACE_USB == halmac_interface) || (HALMAC_INTERFACE_PCIE == halmac_interface)) {
		if (NULL == pHalmac_platform_api->REG_READ_8) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->REG_READ_8)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->REG_READ_16) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->REG_READ_16)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->REG_READ_32) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->REG_READ_32)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->REG_WRITE_8) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->REG_WRITE_8)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->REG_WRITE_16) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->REG_WRITE_16)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
		if (NULL == pHalmac_platform_api->REG_WRITE_32) {
			pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->REG_WRITE_32)\n");
			return HALMAC_RET_PLATFORM_API_NULL;
		}
	}

	if (NULL == pHalmac_platform_api->RTL_FREE) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->RTL_FREE)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}

	if (NULL == pHalmac_platform_api->RTL_MALLOC) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->RTL_MALLOC)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (NULL == pHalmac_platform_api->RTL_MEMCPY) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->RTL_MEMCPY)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (NULL == pHalmac_platform_api->RTL_MEMSET) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->RTL_MEMSET)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (NULL == pHalmac_platform_api->RTL_DELAY_US) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->RTL_DELAY_US)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}

	if (NULL == pHalmac_platform_api->MUTEX_INIT) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->MUTEX_INIT)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (NULL == pHalmac_platform_api->MUTEX_DEINIT) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->MUTEX_DEINIT)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (NULL == pHalmac_platform_api->MUTEX_LOCK) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->MUTEX_LOCK)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (NULL == pHalmac_platform_api->MUTEX_UNLOCK) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->MUTEX_UNLOCK)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}
	if (NULL == pHalmac_platform_api->EVENT_INDICATION) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "(NULL==pHalmac_platform_api->EVENT_INDICATION)\n");
		return HALMAC_RET_PLATFORM_API_NULL;
	}

	pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_TRACE, "halmac_check_platform_api ==========>\n");

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_get_version() - get HALMAC version
 * @version : return version of major, prototype and minor information
 * Author : KaiYuan Chang / Ivan Lin
 * Return : HALMAC_RET_STATUS
 * More details of status code can be found in prototype document
 */
HALMAC_RET_STATUS
halmac_get_version(
	OUT	HALMAC_VER *version
)
{
	version->major_ver = (u8)HALMAC_MAJOR_VER;
	version->prototype_ver = (u8)HALMAC_PROTOTYPE_VER;
	version->minor_ver = (u8)HALMAC_MINOR_VER;

	return HALMAC_RET_SUCCESS;
}

HALMAC_RET_STATUS
halmac_get_chip_info(
	IN VOID	*pDriver_adapter,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api,
	IN HALMAC_INTERFACE	halmac_interface,
	IN PHALMAC_ADAPTER pHalmac_adapter
)
{
	PHALMAC_API pHalmac_api = (PHALMAC_API)NULL;
	u8 chip_id, chip_version;
	u32 polling_count;

	pHalmac_api = (PHALMAC_API)pHalmac_adapter->pHalmac_api;

	/* Get Chip_id and Chip_version */
	if (HALMAC_INTERFACE_SDIO == pHalmac_adapter->halmac_interface) {
		plarform_reg_write_8_sdio(pDriver_adapter, pHalmac_platform_api, REG_SDIO_HSUS_CTRL, platform_reg_read_8_sdio(pDriver_adapter, pHalmac_platform_api, REG_SDIO_HSUS_CTRL) & ~(BIT(0)));

		polling_count = 10000;
		while (!(platform_reg_read_8_sdio(pDriver_adapter, pHalmac_platform_api, REG_SDIO_HSUS_CTRL) & 0x02)) {
			polling_count--;
			if (polling_count == 0)
				return HALMAC_RET_SDIO_LEAVE_SUSPEND_FAIL;
		}

		chip_id = platform_reg_read_8_sdio(pDriver_adapter, pHalmac_platform_api, REG_SYS_CFG2);
		chip_version =  platform_reg_read_8_sdio(pDriver_adapter, pHalmac_platform_api, REG_SYS_CFG1 + 1) >> 4;
	} else {
		chip_id = pHalmac_platform_api->REG_READ_8(pDriver_adapter, REG_SYS_CFG2);
		chip_version = pHalmac_platform_api->REG_READ_8(pDriver_adapter, REG_SYS_CFG1 + 1) >> 4;
	}

	PLATFORM_MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_TRACE, "[TRACE]Chip id : 0x%X\n", chip_id);
	PLATFORM_MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_TRACE, "[TRACE]Chip version : 0x%X\n", chip_version);

	pHalmac_adapter->chip_version = (HALMAC_CHIP_VER)chip_version;

	if (HALMAC_CHIP_ID_HW_DEF_8822B == chip_id)
		pHalmac_adapter->chip_id = HALMAC_CHIP_ID_8822B;
	else if (HALMAC_CHIP_ID_HW_DEF_8821C == chip_id)
		pHalmac_adapter->chip_id = HALMAC_CHIP_ID_8821C;
	else if (HALMAC_CHIP_ID_HW_DEF_8814B == chip_id)
		pHalmac_adapter->chip_id = HALMAC_CHIP_ID_8814B;
	else if (HALMAC_CHIP_ID_HW_DEF_8197F == chip_id)
		pHalmac_adapter->chip_id = HALMAC_CHIP_ID_8197F;
	else {
		pHalmac_adapter->chip_id = HALMAC_CHIP_ID_UNDEFINE;
		return HALMAC_RET_CHIP_NOT_SUPPORT;
	}

	return HALMAC_RET_SUCCESS;
}

u8
platform_reg_read_8_sdio(
	IN VOID	*pDriver_adapter,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api,
	IN u32 offset
)
{
	u8 value8;
	u32 halmac_offset = offset;
	HALMAC_RET_STATUS status = HALMAC_RET_SUCCESS;

	if (0 == (halmac_offset & 0xFFFF0000))
		halmac_offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset(&halmac_offset);
	if (HALMAC_RET_SUCCESS != status) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "platform_reg_read_8_sdio error = %x\n", status);
		return status;
	}

	value8 = pHalmac_platform_api->SDIO_CMD52_READ(pDriver_adapter, halmac_offset);

	return value8;
}

HALMAC_RET_STATUS
plarform_reg_write_8_sdio(
	IN VOID	*pDriver_adapter,
	IN PHALMAC_PLATFORM_API pHalmac_platform_api,
	IN u32 offset,
	IN u8 data
)
{
	HALMAC_RET_STATUS status = HALMAC_RET_SUCCESS;
	u32 halmac_offset = offset;

	if (0 == (halmac_offset & 0xFFFF0000))
		halmac_offset |= WLAN_IOREG_OFFSET;

	status = halmac_convert_to_sdio_bus_offset(&halmac_offset);

	if (HALMAC_RET_SUCCESS != status) {
		pHalmac_platform_api->MSG_PRINT(pDriver_adapter, HALMAC_MSG_INIT, HALMAC_DBG_ERR, "halmac_reg_write_8_sdio_88xx error = %x\n", status);
		return status;
	}
	pHalmac_platform_api->SDIO_CMD52_WRITE(pDriver_adapter, halmac_offset, data);

	return HALMAC_RET_SUCCESS;
}


HALMAC_RET_STATUS
halmac_convert_to_sdio_bus_offset(
	INOUT u32 *halmac_offset
)
{

	switch ((*halmac_offset) & 0xFFFF0000) {
	case WLAN_IOREG_OFFSET:
		*halmac_offset = (HALMAC_SDIO_CMD_ADDR_MAC_REG << 13) | (*halmac_offset & HALMAC_WLAN_MAC_REG_MSK);
		break;
	case SDIO_LOCAL_OFFSET:
		*halmac_offset = (HALMAC_SDIO_CMD_ADDR_SDIO_REG << 13) | (*halmac_offset & HALMAC_SDIO_LOCAL_MSK);
		break;
	default:
		*halmac_offset = 0xFFFFFFFF;
		return HALMAC_RET_CONVERT_SDIO_OFFSET_FAIL;
	}

	return HALMAC_RET_SUCCESS;
}

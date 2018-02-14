/******************************************************************************
 *
 * Copyright(c) 2016 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#define _RTL8821C_MAC_C_

#include <drv_types.h>		/* PADAPTER, basic_types.h and etc. */
#include <hal_data.h>		/* HAL_DATA_TYPE */
#include "../hal_halmac.h"	/* Register Definition and etc. */

inline u8 rtl8821c_rcr_config(PADAPTER p, u32 rcr)
{
	int err;

	err = rtw_write32(p, REG_RCR_8821C, rcr);
	if (_FAIL == err)
		return _FALSE;

	GET_HAL_DATA(p)->ReceiveConfig = rcr;
	return _TRUE;
}

inline u8 rtl8821c_rcr_get(PADAPTER p, u32 *rcr)
{
	u32 v32;

	v32 = rtw_read32(p, REG_RCR_8821C);
	if (rcr)
		*rcr = v32;
	GET_HAL_DATA(p)->ReceiveConfig = v32;
	return _TRUE;
}

inline u8 rtl8821c_rcr_check(PADAPTER p, u32 check_bit)
{
	PHAL_DATA_TYPE hal;
	u32 rcr;

	hal = GET_HAL_DATA(p);
	rcr = hal->ReceiveConfig;
	if ((rcr & check_bit) == check_bit)
		return _TRUE;

	return _FALSE;
}

inline u8 rtl8821c_rcr_add(PADAPTER p, u32 add)
{
	PHAL_DATA_TYPE hal;
	u32 rcr;
	u8 ret = _TRUE;

	hal = GET_HAL_DATA(p);

	rcr = hal->ReceiveConfig;
	rcr |= add;
	if (rcr != hal->ReceiveConfig)
		ret = rtl8821c_rcr_config(p, rcr);

	return ret;
}

inline u8 rtl8821c_rcr_clear(PADAPTER p, u32 clear)
{
	PHAL_DATA_TYPE hal;
	u32 rcr;
	u8 ret = _TRUE;

	hal = GET_HAL_DATA(p);

	rcr = hal->ReceiveConfig;
	rcr &= ~clear;
	if (rcr != hal->ReceiveConfig)
		ret = rtl8821c_rcr_config(p, rcr);

	return ret;
}

#ifdef CONFIG_XMIT_ACK
inline u8 rtl8821c_set_mgnt_xmit_ack(_adapter *adapter)
{
	int err;

	/*ack for xmit mgmt frames.*/
	err = rtw_write32(adapter, REG_FWHW_TXQ_CTRL_8821C, rtw_read32(adapter, REG_FWHW_TXQ_CTRL_8821C) | BIT(12));
	if (err == _FAIL)
		return _FAIL;

	return _SUCCESS;
}
#endif

inline u8 rtl8821c_rx_ba_ssn_appended(PADAPTER p)
{
	return rtl8821c_rcr_check(p, BIT_APP_BASSN_8821C);
}

inline u8 rtl8821c_rx_fcs_append_switch(PADAPTER p, u8 enable)
{
	u32 rcr_bit;
	u8 ret = _TRUE;

	rcr_bit = BIT_APP_FCS_8821C;
	if (_TRUE == enable)
		ret = rtl8821c_rcr_add(p, rcr_bit);
	else
		ret = rtl8821c_rcr_clear(p, rcr_bit);

	return ret;
}

inline u8 rtl8821c_rx_fcs_appended(PADAPTER p)
{
	return rtl8821c_rcr_check(p, BIT_APP_FCS_8821C);
}

u8 rtl8821c_rx_tsf_addr_filter_config(_adapter *adapter, u8 config)
{
	u8 v8;
	int err;

	v8 = GET_HAL_DATA(adapter)->rx_tsf_addr_filter_config;

	if (v8 != config) {

		err = rtw_write8(adapter, REG_NAN_RX_TSF_FILTER_8821C, config);
		if (_FAIL == err)
			return _FALSE;
	}

	GET_HAL_DATA(adapter)->rx_tsf_addr_filter_config = config;
	return _TRUE;
}

/*
 * Return:
 *	_SUCCESS	Download Firmware OK.
 *	_FAIL		Download Firmware FAIL!
 */
s32 rtl8821c_fw_dl(PADAPTER adapter, u8 wowlan)
{
	struct dvobj_priv *d = adapter_to_dvobj(adapter);
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(adapter);
	int err;
	u8 fw_bin = _TRUE;

#ifdef CONFIG_FILE_FWIMG
	fw_bin = _TRUE;
	if (_TRUE == wowlan) {
		rtw_get_phy_file_path(adapter, MAC_FILE_FW_WW_IMG);
		err = rtw_halmac_dlfw_from_file(d, rtw_phy_para_file_path);
	} else {
		rtw_get_phy_file_path(adapter, MAC_FILE_FW_NIC);
		err = rtw_halmac_dlfw_from_file(d, rtw_phy_para_file_path);
	}
#else
	fw_bin = _FALSE;
	if (_TRUE == wowlan)
		err = rtw_halmac_dlfw(d, array_mp_8821c_fw_wowlan, array_length_mp_8821c_fw_wowlan);
	else
		err = rtw_halmac_dlfw(d, array_mp_8821c_fw_nic, array_length_mp_8821c_fw_nic);
#endif

	if (!err) {
		adapter->bFWReady = _TRUE;
		hal_data->fw_ractrl = _TRUE;
		RTW_INFO("%s Download Firmware from %s success\n", __func__, (fw_bin) ? "file" : "array");
		RTW_INFO("%s FW Version:%d SubVersion:%d\n", (wowlan) ? "WOW" : "NIC", hal_data->firmware_version, hal_data->firmware_sub_version);

		return _SUCCESS;
	} else {
		RTW_ERR("%s Download Firmware from %s failed\n", __func__, (fw_bin) ? "file" : "array");
		return _FAIL;
	}
}
/*
 * Return:
 *	_SUCCESS	Download Firmware MEM OK.
 *	_FAIL		Download Firmware MEM FAIL!
 */
s32 rtl8821c_fw_mem_dl(PADAPTER adapter, enum fw_mem mem)
{
	struct dvobj_priv *d = adapter_to_dvobj(adapter);
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(adapter);
	int err = 0;
	u8 fw_bin = _TRUE;

#ifdef CONFIG_FILE_FWIMG
	fw_bin = _TRUE;
	rtw_get_phy_file_path(adapter, MAC_FILE_FW_NIC);
	err = rtw_halmac_dlfw_mem_from_file(d, rtw_phy_para_file_path, mem);
#else
	fw_bin = _FALSE;
	err = rtw_halmac_dlfw_mem(d, array_mp_8821c_fw_nic, array_length_mp_8821c_fw_nic, mem);
#endif

	if (err) {
		RTW_ERR("%s Download Firmware MEM from %s failed\n", __func__, (fw_bin) ? "file" : "array");
		return _FAIL;
	}

	RTW_INFO("%s Download Firmware MEM from %s success\n", __func__, (fw_bin) ? "file" : "array");
	return _SUCCESS;

}


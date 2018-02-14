/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
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
/* ************************************************************
 * Description:
 *
 * This file is for 8821C dynamic mechanism
 *
 *
 * ************************************************************ */
#define _RTL8812C_DM_C_

/* ************************************************************
 * include files
 * ************************************************************
 */

#include <drv_types.h>
#include <rtl8821c_hal.h>

/* ************************************************************
 * Global var
 * ************************************************************ */
static void dm_CheckProtection(PADAPTER adapter)
{
}
	
static void dm_CheckStatistics(PADAPTER adapter)
{
}
	
#ifdef CONFIG_SUPPORT_HW_WPS_PBC
static void dm_CheckPbcGPIO(PADAPTER adapter)
{
	u8 tmp1byte;
	u8 bPbcPressed = _FALSE;

	if (!adapter->registrypriv.hw_wps_pbc)
		return;
	
#ifdef CONFIG_USB_HCI
	tmp1byte = rtw_read8(adapter, GPIO_IO_SEL);
	tmp1byte |= (HAL_8192C_HW_GPIO_WPS_BIT);
	rtw_write8(adapter, GPIO_IO_SEL, tmp1byte); /* enable GPIO[2] as output mode */

	tmp1byte &= ~(HAL_8192C_HW_GPIO_WPS_BIT);
	rtw_write8(adapter, GPIO_IN, tmp1byte); /* reset the floating voltage level */

	tmp1byte = rtw_read8(adapter, GPIO_IO_SEL);
	tmp1byte &= ~(HAL_8192C_HW_GPIO_WPS_BIT);
	rtw_write8(adapter, GPIO_IO_SEL, tmp1byte); /* enable GPIO[2] as input mode */

	tmp1byte = rtw_read8(adapter, GPIO_IN);
	if (tmp1byte == 0xff)
		return;
	
	if (tmp1byte & HAL_8192C_HW_GPIO_WPS_BIT)
		bPbcPressed = _TRUE;
#else
	tmp1byte = rtw_read8(adapter, GPIO_IN);
	
	if ((tmp1byte == 0xff) || adapter->init_adpt_in_progress)
		return;
	
	if ((tmp1byte & HAL_8192C_HW_GPIO_WPS_BIT) == 0)
		bPbcPressed = _TRUE;
#endif
	
	if (_TRUE == bPbcPressed) {
	/*
	 * Here we only set bPbcPressed to true
	 * After trigger PBC, the variable will be set to false
	 */
		RTW_INFO("CheckPbcGPIO - PBC is pressed\n");
			rtw_request_wps_pbc_event(adapter);
	}
}
#endif /* CONFIG_SUPPORT_HW_WPS_PBC */
	
	
#ifdef CONFIG_PCI_HCI
/*
 * Description:
 *	Perform interrupt migration dynamically to reduce CPU utilization.
 *
 * Assumption:
 *	1. Do not enable migration under WIFI test.
 */
void dm_InterruptMigration(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	BOOLEAN bCurrentIntMt, bCurrentACIntDisable;
	BOOLEAN IntMtToSet = _FALSE;
	BOOLEAN ACIntToSet = _FALSE;


	/* Retrieve current interrupt migration and Tx four ACs IMR settings first. */
	bCurrentIntMt = hal->bInterruptMigration;
	bCurrentACIntDisable = hal->bDisableTxInt;

	/*
	 * <Roger_Notes> Currently we use busy traffic for reference instead of RxIntOK counts to prevent non-linear Rx statistics
	 * when interrupt migration is set before. 2010.03.05.
	 */
	if (!adapter->registrypriv.wifi_spec
		&& (check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE)
		&& pmlmepriv->LinkDetectInfo.bHigherBusyTraffic) {
		IntMtToSet = _TRUE;

		/* To check whether we should disable Tx interrupt or not. */
		if (pmlmepriv->LinkDetectInfo.bHigherBusyRxTraffic)
			ACIntToSet = _TRUE;
	}

	/* Update current settings. */
	if (bCurrentIntMt != IntMtToSet) {
		RTW_INFO("%s: Update interrrupt migration(%d)\n", __FUNCTION__, IntMtToSet);
		if (IntMtToSet) {
			/*
			 * <Roger_Notes> Set interrrupt migration timer and corresponging Tx/Rx counter.
			 * timer 25ns*0xfa0=100us for 0xf packets.
			 * 2010.03.05.
			 */
			rtw_write32(adapter, REG_INT_MIG, 0xff000fa0); /* 0x306:Rx, 0x307:Tx */
			hal->bInterruptMigration = IntMtToSet;
		} else {
			/* Reset all interrupt migration settings. */
			rtw_write32(adapter, REG_INT_MIG, 0);
			hal->bInterruptMigration = IntMtToSet;
		}
	}
}
#endif /* CONFIG_PCI_HCI */
	
/*
 * ============================================================
 * functions
 * ============================================================
 */
static void init_phydm_cominfo(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal;
	struct PHY_DM_STRUCT *pDM_Odm;
	u32 SupportAbility = 0;
	u8 cut_ver = ODM_CUT_A, fab_ver = ODM_TSMC;

	
	hal = GET_HAL_DATA(adapter);
	pDM_Odm = &hal->odmpriv;

	Init_ODM_ComInfo(adapter);

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_PACKAGE_TYPE, hal->PackageType);

	if (IS_CHIP_VENDOR_TSMC(hal->version_id))
		fab_ver = ODM_TSMC;
	else if (IS_CHIP_VENDOR_UMC(hal->version_id))
		fab_ver = ODM_UMC;
	else if (IS_CHIP_VENDOR_SMIC(hal->version_id))
		fab_ver = ODM_UMC + 1;
	else
		RTW_INFO("%s: unknown fab_ver=%d !!\n",
			 __FUNCTION__, GET_CVID_MANUFACTUER(hal->version_id));

	if (IS_A_CUT(hal->version_id))
		cut_ver = ODM_CUT_A;
	else if (IS_B_CUT(hal->version_id))
		cut_ver = ODM_CUT_B;
	else if (IS_C_CUT(hal->version_id))
		cut_ver = ODM_CUT_C;
	else if (IS_D_CUT(hal->version_id))
		cut_ver = ODM_CUT_D;
	else if (IS_E_CUT(hal->version_id))
		cut_ver = ODM_CUT_E;
	else if (IS_F_CUT(hal->version_id))
		cut_ver = ODM_CUT_F;
	else if (IS_I_CUT(hal->version_id))
		cut_ver = ODM_CUT_I;
	else if (IS_J_CUT(hal->version_id))
		cut_ver = ODM_CUT_J;
	else if (IS_K_CUT(hal->version_id))
		cut_ver = ODM_CUT_K;
	else
		RTW_INFO("%s: unknown cut_ver=%d !!\n",
			 __FUNCTION__, GET_CVID_CUT_VERSION(hal->version_id));

	RTW_INFO("%s: fab_ver=%d cut_ver=%d\n", __FUNCTION__, fab_ver, cut_ver);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_FAB_VER, fab_ver);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_CUT_VER, cut_ver);

	SupportAbility = ODM_RF_CALIBRATION | ODM_RF_TX_PWR_TRACK;

#ifdef CONFIG_DISABLE_ODM
	SupportAbility = 0;
#endif 
	odm_cmn_info_update(pDM_Odm, ODM_CMNINFO_ABILITY, SupportAbility);
}
	
void rtl8821c_phy_init_dm_priv(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT *podmpriv = &hal->odmpriv;

	init_phydm_cominfo(adapter);
	odm_init_all_timers(podmpriv);

	/*PHYDM API - thermal trim*/
	phydm_get_thermal_trim_offset(podmpriv);
	/*PHYDM API - power trim*/
	phydm_get_power_trim_offset(podmpriv);
}
	
void rtl8821c_phy_deinit_dm_priv(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT *podmpriv = &hal->odmpriv;


	odm_cancel_all_timers(podmpriv);
}

void rtl8821c_phy_init_haldm(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT *pDM_Odm = &hal->odmpriv;

	hal->DM_Type = dm_type_by_driver;
	odm_dm_init(pDM_Odm);
}

void rtl8821c_phy_haldm_watchdog(PADAPTER Adapter)
{
	BOOLEAN bFwCurrentInPSMode = _FALSE;
	BOOLEAN bFwPSAwake = _TRUE;
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	struct PHY_DM_STRUCT		*pDM_Odm = &(pHalData->odmpriv);


	if (!rtw_is_hw_init_completed(Adapter))
		goto skip_dm;

#ifdef CONFIG_LPS
	bFwCurrentInPSMode = adapter_to_pwrctl(Adapter)->bFwCurrentInPSMode;
	rtw_hal_get_hwreg(Adapter, HW_VAR_FWLPS_RF_ON, (u8 *)(&bFwPSAwake));
#endif

#ifdef CONFIG_P2P_PS
	/* Fw is under p2p powersaving mode, driver should stop dynamic mechanism.
	 modifed by thomas. 2011.06.11.*/
	if (Adapter->wdinfo.p2p_ps_mode)
		bFwPSAwake = _FALSE;
#endif /*CONFIG_P2P_PS*/

	if ((rtw_is_hw_init_completed(Adapter))
		&& ((!bFwCurrentInPSMode) && bFwPSAwake)) {
		/*	Calculate Tx/Rx statistics. */
		dm_CheckStatistics(Adapter);

		/* Dynamically switch RTS/CTS protection.*/
		/*dm_CheckProtection(Adapter);*/
	}

#ifdef CONFIG_DISABLE_ODM
	goto skip_dm;
#endif

	if (rtw_is_hw_init_completed(Adapter)) {
		u8	bLinked = _FALSE;
		u8	bsta_state = _FALSE;
		u8	bBtDisabled = _TRUE;

#ifdef CONFIG_LPS_PG
		u8	is_in_lpspg = _FALSE;
		struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(Adapter);
#endif
		if (rtw_mi_check_status(Adapter, MI_STA_LINKED) || rtw_mi_check_status(Adapter, MI_AP_ASSOC)) {
			bLinked = _TRUE;
			if (rtw_mi_check_status(Adapter, MI_STA_LINKED))
				bsta_state = _TRUE;
		}

		odm_cmn_info_update(&pHalData->odmpriv, ODM_CMNINFO_LINK, bLinked);
		odm_cmn_info_update(&pHalData->odmpriv, ODM_CMNINFO_STATION_STATE, bsta_state);

#ifdef CONFIG_BT_COEXIST
		bBtDisabled = rtw_btcoex_IsBtDisabled(Adapter);
		odm_cmn_info_update(&pHalData->odmpriv, ODM_CMNINFO_BT_ENABLED, ((bBtDisabled == _TRUE) ? _FALSE : _TRUE));
#else
		odm_cmn_info_update(&pHalData->odmpriv, ODM_CMNINFO_BT_ENABLED, _FALSE);
#endif /* CONFIG_BT_COEXIST*/


#ifdef CONFIG_LPS_PG
		if ((pwrpriv->lps_level == LPS_PG) && (pwrpriv->pwr_mode != PS_MODE_ACTIVE) && (pwrpriv->rpwm <= PS_STATE_S2))
			is_in_lpspg = _TRUE;
		odm_cmn_info_update(&pHalData->odmpriv, ODM_CMNINFO_LPSPG, is_in_lpspg);
#endif
		odm_dm_watchdog(&pHalData->odmpriv);

	}

skip_dm:

#ifdef CONFIG_SUPPORT_HW_WPS_PBC
	/* Check GPIO to determine current Pbc status.*/
	dm_CheckPbcGPIO(Adapter);
#endif
	return;
}

void rtl8821c_phy_haldm_in_lps(PADAPTER adapter)
{
#if 0 /* phydm not ready */
	u32 PWDB_rssi = 0;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT *pDM_Odm = &hal->odmpriv;
	struct _dynamic_initial_gain_threshold_ *pDM_DigTable = &pDM_Odm->dm_dig_table;
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct sta_info *psta = NULL;


	RTW_INFO("%s: rssi_min=%d\n", __FUNCTION__, pDM_Odm->rssi_min);

	/* update IGI */
	odm_write_dig(pDM_Odm, pDM_Odm->rssi_min);

	/* set rssi to fw */
	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta && (psta->rssi_stat.undecorated_smoothed_pwdb > 0)) {
		PWDB_rssi = (psta->mac_id | (psta->rssi_stat.undecorated_smoothed_pwdb << 16));
		rtl8821c_set_FwRssiSetting_cmd(adapter, (u8 *)&PWDB_rssi);
	}
#endif
}

void rtl8821c_phy_haldm_watchdog_in_lps(PADAPTER adapter)
{
#if 0 /* phydm not ready */
	u8 bLinked = _FALSE;
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct PHY_DM_STRUCT *pDM_Odm = &hal->odmpriv;
	struct _dynamic_initial_gain_threshold_ *pDM_DigTable = &pDM_Odm->dm_dig_table;
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct sta_info *psta = NULL;
#ifdef CONFIG_CONCURRENT_MODE
	PADAPTER pbuddy_adapter = adapter->pbuddy_adapter;
#endif /* CONFIG_CONCURRENT_MODE */


	if (!rtw_is_hw_init_completed(adapter))
		goto skip_lps_dm;

	if (rtw_linked_check(adapter))
		bLinked = _TRUE;

#ifdef CONFIG_CONCURRENT_MODE
	if (pbuddy_adapter && rtw_linked_check(pbuddy_adapter))
		bLinked = _TRUE;
#endif /* CONFIG_CONCURRENT_MODE */

	odm_cmn_info_update(&hal->odmpriv, ODM_CMNINFO_LINK, bLinked);

	if (bLinked == _FALSE)
		goto skip_lps_dm;

	if (!(pDM_Odm->support_ability & ODM_BB_RSSI_MONITOR))
		goto skip_lps_dm;

	/* Do DIG by RSSI In LPS-32K */

	/* 1. Find MIN-RSSI */
	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta == NULL)
		goto skip_lps_dm;

	hal->entry_min_undecorated_smoothed_pwdb = psta->rssi_stat.undecorated_smoothed_pwdb;

	RTW_INFO("cur_ig_value=%d, entry_min_undecorated_smoothed_pwdb=%d\n",
		pDM_DigTable->cur_ig_value, hal->entry_min_undecorated_smoothed_pwdb);

	if (hal->entry_min_undecorated_smoothed_pwdb <= 0)
		goto skip_lps_dm;

	hal->min_undecorated_pwdb_for_dm = hal->entry_min_undecorated_smoothed_pwdb;

	pDM_Odm->rssi_min = hal->min_undecorated_pwdb_for_dm;

#ifdef CONFIG_LPS
	if ((pDM_DigTable->cur_ig_value > pDM_Odm->rssi_min + 5)
		|| (pDM_DigTable->cur_ig_value < pDM_Odm->rssi_min - 5))
		rtw_dm_in_lps_wk_cmd(adapter);
#endif

skip_lps_dm:
#endif
	return;
}

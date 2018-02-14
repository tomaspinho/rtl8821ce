/******************************************************************************
 *
 * Copyright(c) 2014 Realtek Corporation. All rights reserved.
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

#include <drv_types.h>
#include <hal_data.h>

/* A mapping from HalData to ODM. */
enum odm_board_type_e boardType(u8 InterfaceSel)
{
	enum odm_board_type_e        board	= ODM_BOARD_DEFAULT;

#ifdef CONFIG_PCI_HCI
	INTERFACE_SELECT_PCIE   pcie	= (INTERFACE_SELECT_PCIE)InterfaceSel;
	switch (pcie) {
	case INTF_SEL0_SOLO_MINICARD:
		board |= ODM_BOARD_MINICARD;
		break;
	case INTF_SEL1_BT_COMBO_MINICARD:
		board |= ODM_BOARD_BT;
		board |= ODM_BOARD_MINICARD;
		break;
	default:
		board = ODM_BOARD_DEFAULT;
		break;
	}

#elif defined(CONFIG_USB_HCI)
	INTERFACE_SELECT_USB    usb	= (INTERFACE_SELECT_USB)InterfaceSel;
	switch (usb) {
	case INTF_SEL1_USB_High_Power:
		board |= ODM_BOARD_EXT_LNA;
		board |= ODM_BOARD_EXT_PA;
		break;
	case INTF_SEL2_MINICARD:
		board |= ODM_BOARD_MINICARD;
		break;
	case INTF_SEL4_USB_Combo:
		board |= ODM_BOARD_BT;
		break;
	case INTF_SEL5_USB_Combo_MF:
		board |= ODM_BOARD_BT;
		break;
	case INTF_SEL0_USB:
	case INTF_SEL3_USB_Solo:
	default:
		board = ODM_BOARD_DEFAULT;
		break;
	}

#endif
	/* RTW_INFO("===> boardType(): (pHalData->InterfaceSel, pDM_Odm->BoardType) = (%d, %d)\n", InterfaceSel, board); */

	return board;
}

void Init_ODM_ComInfo(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT		*pDM_Odm = &(pHalData->odmpriv);
	struct mlme_ext_priv	*pmlmeext = &adapter->mlmeextpriv;
	struct mlme_priv	*pmlmepriv = &adapter->mlmepriv;
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(adapter);
	int i;

	_rtw_memset(pDM_Odm, 0, sizeof(*pDM_Odm));

	pDM_Odm->adapter = adapter;

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_PLATFORM, ODM_CE);

	rtw_odm_init_ic_type(adapter);

	if (rtw_get_intf_type(adapter) == RTW_GSPI)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_INTERFACE, ODM_ITRF_SDIO);
	else
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_INTERFACE, rtw_get_intf_type(adapter));

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_MP_TEST_CHIP, IS_NORMAL_CHIP(pHalData->version_id));

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_PATCH_ID, pHalData->CustomerID);

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_BWIFI_TEST, adapter->registrypriv.wifi_spec);


	if (pHalData->rf_type == RF_1T1R)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_1T1R);
	else if (pHalData->rf_type == RF_1T2R)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_1T2R);
	else if (pHalData->rf_type == RF_2T2R)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_2T2R);
	else if (pHalData->rf_type == RF_2T2R_GREEN)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_2T2R_GREEN);
	else if (pHalData->rf_type == RF_2T3R)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_2T3R);
	else if (pHalData->rf_type == RF_2T4R)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_2T4R);
	else if (pHalData->rf_type == RF_3T3R)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_3T3R);
	else if (pHalData->rf_type == RF_3T4R)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_3T4R);
	else if (pHalData->rf_type == RF_4T4R)
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_4T4R);
	else
		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_XTXR);


	{
		/* 1 ======= BoardType: ODM_CMNINFO_BOARD_TYPE ======= */
		u8 odm_board_type = ODM_BOARD_DEFAULT;

		if (pHalData->ExternalLNA_2G != 0) {
			odm_board_type |= ODM_BOARD_EXT_LNA;
			odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_EXT_LNA, 1);
		}
		if (pHalData->external_lna_5g != 0) {
			odm_board_type |= ODM_BOARD_EXT_LNA_5G;
			odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_5G_EXT_LNA, 1);
		}
		if (pHalData->ExternalPA_2G != 0) {
			odm_board_type |= ODM_BOARD_EXT_PA;
			odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_EXT_PA, 1);
		}
		if (pHalData->external_pa_5g != 0) {
			odm_board_type |= ODM_BOARD_EXT_PA_5G;
			odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_5G_EXT_PA, 1);
		}
		if (pHalData->EEPROMBluetoothCoexist)
			odm_board_type |= ODM_BOARD_BT;

		odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_BOARD_TYPE, odm_board_type);
		/* 1 ============== End of BoardType ============== */
	}

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_DOMAIN_CODE_2G, pHalData->Regulation2_4G);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_DOMAIN_CODE_5G, pHalData->Regulation5G);

#ifdef CONFIG_DFS_MASTER
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_DFS_REGION_DOMAIN, adapter->registrypriv.dfs_region_domain);
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_DFS_MASTER_ENABLE, &(adapter_to_rfctl(adapter)->dfs_master_enabled));
#endif

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_GPA, pHalData->TypeGPA);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_APA, pHalData->TypeAPA);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_GLNA, pHalData->TypeGLNA);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_ALNA, pHalData->TypeALNA);

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RFE_TYPE, pHalData->rfe_type);

	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_EXT_TRSW, 0);

	/*Add by YuChen for kfree init*/
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_REGRFKFREEENABLE, adapter->registrypriv.RegPwrTrimEnable);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RFKFREEENABLE, pHalData->RfKFreeEnable);

	/*Antenna diversity relative parameters*/
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_ANT_DIV, &(pHalData->AntDivCfg));
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_RF_ANTENNA_TYPE, pHalData->TRxAntDivType);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_BE_FIX_TX_ANT, pHalData->b_fix_tx_ant);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_WITH_EXT_ANTENNA_SWITCH, pHalData->with_extenal_ant_switch);

	/* (8822B) efuse 0x3D7 & 0x3D8 for TX PA bias */
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_EFUSE0X3D7, pHalData->efuse0x3d7);
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_EFUSE0X3D8, pHalData->efuse0x3d8);

	/*Add by YuChen for adaptivity init*/
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_ADAPTIVITY, &(adapter->registrypriv.adaptivity_en));
	phydm_adaptivity_info_init(pDM_Odm, PHYDM_ADAPINFO_CARRIER_SENSE_ENABLE, (adapter->registrypriv.adaptivity_mode != 0) ? TRUE : FALSE);
	phydm_adaptivity_info_init(pDM_Odm, PHYDM_ADAPINFO_DCBACKOFF, adapter->registrypriv.adaptivity_dc_backoff);
	phydm_adaptivity_info_init(pDM_Odm, PHYDM_ADAPINFO_DYNAMICLINKADAPTIVITY, (adapter->registrypriv.adaptivity_dml != 0) ? TRUE : FALSE);
	phydm_adaptivity_info_init(pDM_Odm, PHYDM_ADAPINFO_TH_L2H_INI, adapter->registrypriv.adaptivity_th_l2h_ini);
	phydm_adaptivity_info_init(pDM_Odm, PHYDM_ADAPINFO_TH_EDCCA_HL_DIFF, adapter->registrypriv.adaptivity_th_edcca_hl_diff);

#ifdef CONFIG_IQK_PA_OFF
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_IQKPAOFF, 1);
#endif
	odm_cmn_info_init(pDM_Odm, ODM_CMNINFO_IQKFWOFFLOAD, pHalData->RegIQKFWOffload);

	/* Pointer reference */
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_TX_UNI, &(dvobj->traffic_stat.tx_bytes));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_RX_UNI, &(dvobj->traffic_stat.rx_bytes));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_WM_MODE, &(pmlmeext->cur_wireless_mode));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_BAND, &(pHalData->current_band_type));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_FORCED_RATE, &(pHalData->ForcedDataRate));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_FORCED_IGI_LB, &(pHalData->u1ForcedIgiLb));

	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_SEC_CHNL_OFFSET, &(pHalData->nCur40MhzPrimeSC));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_SEC_MODE, &(adapter->securitypriv.dot11PrivacyAlgrthm));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_BW, &(pHalData->current_channel_bw));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_CHNL, &(pHalData->current_channel));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_NET_CLOSED, &(adapter->net_closed));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_FORCED_IGI_LB, &(pHalData->u1ForcedIgiLb));

	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_SCAN, &(pmlmepriv->bScanInProcess));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_POWER_SAVING, &(pwrctl->bpower_saving));
	/*Add by Yuchen for phydm beamforming*/
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_TX_TP, &(dvobj->traffic_stat.cur_tx_tp));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_RX_TP, &(dvobj->traffic_stat.cur_rx_tp));
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_ANT_TEST, &(pHalData->antenna_test));
#ifdef CONFIG_USB_HCI
	odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_HUBUSBMODE, &(dvobj->usb_speed));
#endif
	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		odm_cmn_info_ptr_array_hook(pDM_Odm, ODM_CMNINFO_STA_STATUS, i, NULL);

	phydm_init_debug_setting(pDM_Odm);

	/* TODO */
	/* odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_BT_OPERATION, _FALSE); */
	/* odm_cmn_info_hook(pDM_Odm, ODM_CMNINFO_BT_DISABLE_EDCA, _FALSE); */
}


static u32 edca_setting_UL[HT_IOT_PEER_MAX] =
/*UNKNOWN, REALTEK_90, REALTEK_92SE, BROADCOM,*/
/*RALINK, ATHEROS, CISCO, MERU, MARVELL, 92U_AP, SELF_AP(DownLink/Tx) */
{ 0x5e4322, 0xa44f, 0x5e4322, 0x5ea32b, 0x5ea422, 0x5ea322, 0x3ea430, 0x5ea42b, 0x5ea44f, 0x5e4322, 0x5e4322};

static u32 edca_setting_DL[HT_IOT_PEER_MAX] =
/*UNKNOWN, REALTEK_90, REALTEK_92SE, BROADCOM,*/
/*RALINK, ATHEROS, CISCO, MERU, MARVELL, 92U_AP, SELF_AP(UpLink/Rx)*/
{ 0xa44f, 0x5ea44f,	 0x5e4322, 0x5ea42b, 0xa44f, 0xa630, 0x5ea630, 0x5ea42b, 0xa44f, 0xa42b, 0xa42b};

static u32 edca_setting_dl_g_mode[HT_IOT_PEER_MAX] =
/*UNKNOWN, REALTEK_90, REALTEK_92SE, BROADCOM,*/
/*RALINK, ATHEROS, CISCO, MERU, MARVELL, 92U_AP, SELF_AP */
{ 0x4322, 0xa44f, 0x5e4322, 0xa42b, 0x5e4322, 0x4322,	 0xa42b, 0x5ea42b, 0xa44f, 0x5e4322, 0x5ea42b};

void rtw_hal_turbo_edca(_adapter *adapter)
{
	HAL_DATA_TYPE		*hal_data = GET_HAL_DATA(adapter);
	struct dvobj_priv		*dvobj = adapter_to_dvobj(adapter);
	struct recv_priv		*precvpriv = &(adapter->recvpriv);
	struct registry_priv		*pregpriv = &adapter->registrypriv;
	struct mlme_ext_priv	*pmlmeext = &(adapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	/* Parameter suggested by Scott  */
#if 0
	u32	EDCA_BE_UL = edca_setting_UL[p_mgnt_info->iot_peer];
	u32	EDCA_BE_DL = edca_setting_DL[p_mgnt_info->iot_peer];
#endif
	u32	EDCA_BE_UL = 0x5ea42b;
	u32	EDCA_BE_DL = 0x00a42b;
	u8	ic_type = rtw_get_chip_type(adapter);

	u8	iot_peer = 0;
	u8	wireless_mode = 0xFF;                 /* invalid value */
	u8	traffic_index;
	u32	edca_param;
	u64	cur_tx_bytes = 0;
	u64	cur_rx_bytes = 0;
	u8	bbtchange = _TRUE;
	u8	is_bias_on_rx = _FALSE;
	u8	is_linked = _FALSE;
	u8	interface_type;

	if (hal_data->dis_turboedca)
		return;

	if (rtw_mi_check_status(adapter, MI_ASSOC))
		is_linked = _TRUE;

	if (is_linked != _TRUE) {
		precvpriv->is_any_non_be_pkts = _FALSE;
		return;
	}

	if ((pregpriv->wifi_spec == 1)) { /* || (pmlmeinfo->HT_enable == 0)) */
		precvpriv->is_any_non_be_pkts = _FALSE;
		return;
	}

	interface_type = rtw_get_intf_type(adapter);
	wireless_mode = pmlmeext->cur_wireless_mode;

	iot_peer = pmlmeinfo->assoc_AP_vendor;

	if (iot_peer >=  HT_IOT_PEER_MAX) {
		precvpriv->is_any_non_be_pkts = _FALSE;
		return;
	}

	if (ic_type == RTL8188E) {
		if ((iot_peer == HT_IOT_PEER_RALINK) || (iot_peer == HT_IOT_PEER_ATHEROS))
			is_bias_on_rx = _TRUE;
	}

	/* Check if the status needs to be changed. */
	if ((bbtchange) || (!precvpriv->is_any_non_be_pkts)) {
		cur_tx_bytes = dvobj->traffic_stat.cur_tx_bytes;
		cur_rx_bytes = dvobj->traffic_stat.cur_rx_bytes;

		/* traffic, TX or RX */
		if (is_bias_on_rx) {
			if (cur_tx_bytes > (cur_rx_bytes << 2)) {
				/* Uplink TP is present. */
				traffic_index = UP_LINK;
			} else {
				/* Balance TP is present. */
				traffic_index = DOWN_LINK;
			}
		} else {
			if (cur_rx_bytes > (cur_tx_bytes << 2)) {
				/* Downlink TP is present. */
				traffic_index = DOWN_LINK;
			} else {
				/* Balance TP is present. */
				traffic_index = UP_LINK;
			}
		}
#if 0
		if ((p_dm_odm->dm_edca_table.prv_traffic_idx != traffic_index)
			|| (!p_dm_odm->dm_edca_table.is_current_turbo_edca))
#endif
		{
			if (interface_type == RTW_PCIE) {
				EDCA_BE_UL = 0x6ea42b;
				EDCA_BE_DL = 0x6ea42b;
			}

			/* 92D txop can't be set to 0x3e for cisco1250 */
			if ((iot_peer == HT_IOT_PEER_CISCO) && (wireless_mode == ODM_WM_N24G)) {
				EDCA_BE_DL = edca_setting_DL[iot_peer];
				EDCA_BE_UL = edca_setting_UL[iot_peer];
			}
			/* merge from 92s_92c_merge temp*/
			else if ((iot_peer == HT_IOT_PEER_CISCO) && ((wireless_mode == ODM_WM_G) || (wireless_mode == (ODM_WM_B | ODM_WM_G)) || (wireless_mode == ODM_WM_A) || (wireless_mode == ODM_WM_B)))
				EDCA_BE_DL = edca_setting_dl_g_mode[iot_peer];
			else if ((iot_peer == HT_IOT_PEER_AIRGO) && ((wireless_mode == ODM_WM_G) || (wireless_mode == ODM_WM_A)))
				EDCA_BE_DL = 0xa630;
			else if (iot_peer == HT_IOT_PEER_MARVELL) {
				EDCA_BE_DL = edca_setting_DL[iot_peer];
				EDCA_BE_UL = edca_setting_UL[iot_peer];
			} else if (iot_peer == HT_IOT_PEER_ATHEROS) {
				/* Set DL EDCA for Atheros peer to 0x3ea42b.*/
				/* Suggested by SD3 Wilson for ASUS TP issue.*/
				EDCA_BE_DL = edca_setting_DL[iot_peer];
			}

			if ((ic_type == RTL8812) || (ic_type == RTL8821) || (ic_type == RTL8192E)) { /* add 8812AU/8812AE */
				EDCA_BE_UL = 0x5ea42b;
				EDCA_BE_DL = 0x5ea42b;

				RTW_DBG("8812A: EDCA_BE_UL=0x%x EDCA_BE_DL =0x%x\n", EDCA_BE_UL, EDCA_BE_DL);
			}

			if (interface_type == RTW_PCIE &&
				(ic_type == RTL8822B)) {
				EDCA_BE_UL = 0x6ea42b;
				EDCA_BE_DL = 0x6ea42b;
			}

			if (traffic_index == DOWN_LINK)
				edca_param = EDCA_BE_DL;
			else
				edca_param = EDCA_BE_UL;

			rtw_hal_set_hwreg(adapter, HW_VAR_AC_PARAM_BE, (u8 *)(&edca_param));

			RTW_DBG("Turbo EDCA =0x%x\n", edca_param);

			hal_data->prv_traffic_idx = traffic_index;
		}

		hal_data->is_turbo_edca = _TRUE;
	} else {
		/*  */
		/* Turn Off EDCA turbo here. */
		/* Restore original EDCA according to the declaration of AP. */
		/*  */
		if (hal_data->is_turbo_edca) {
			edca_param = hal_data->ac_param_be;
			rtw_hal_set_hwreg(adapter, HW_VAR_AC_PARAM_BE, (u8 *)(&edca_param));
			hal_data->is_turbo_edca = _FALSE;
		}
	}

}



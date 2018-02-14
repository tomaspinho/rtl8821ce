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

#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8821C_SUPPORT == 1)

/* ======================================================================== */
/* These following functions can be used for PHY DM only*/

u32	reg82c_8821c;
u32	reg838_8821c;
u32	reg830_8821c;
u32	rega24_8821c;
u32	rega28_8821c;
u32	regaac_8821c;

enum odm_bw_e	bw_8821c;
u8	central_ch_8821c;


boolean
phydm_rfe_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	u8					channel
)
{
#if 0
	/* Efuse is not wrote now */
	/* Need to check RFE type finally */
	/*if (p_dm_odm->rfe_type == 1) {*/
	if (channel <= 14) {
		/* signal source */
		odm_set_bb_reg(p_dm_odm, 0xcb0, (MASKBYTE2 | MASKLWORD), 0x704570);
		odm_set_bb_reg(p_dm_odm, 0xeb0, (MASKBYTE2 | MASKLWORD), 0x704570);
		odm_set_bb_reg(p_dm_odm, 0xcb4, MASKBYTE1, 0x45);
		odm_set_bb_reg(p_dm_odm, 0xeb4, MASKBYTE1, 0x45);
	} else if (channel > 35) {
		odm_set_bb_reg(p_dm_odm, 0xcb0, (MASKBYTE2 | MASKLWORD), 0x174517);
		odm_set_bb_reg(p_dm_odm, 0xeb0, (MASKBYTE2 | MASKLWORD), 0x174517);
		odm_set_bb_reg(p_dm_odm, 0xcb4, MASKBYTE1, 0x45);
		odm_set_bb_reg(p_dm_odm, 0xeb4, MASKBYTE1, 0x45);
	} else
		return false;

	/* chip top mux */
	odm_set_bb_reg(p_dm_odm, 0x64, BIT(29) | BIT(28), 0x3);
	odm_set_bb_reg(p_dm_odm, 0x4c, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(p_dm_odm, 0x40, BIT(2), 0x1);

	/* from s0 or s1 */
	odm_set_bb_reg(p_dm_odm, 0x1990, (BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0)), 0x30);
	odm_set_bb_reg(p_dm_odm, 0x1990, (BIT(11) | BIT(10)), 0x3);

	/* input or output */
	odm_set_bb_reg(p_dm_odm, 0x974, (BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0)), 0x3f);
	odm_set_bb_reg(p_dm_odm, 0x974, (BIT(11) | BIT(10)), 0x3);

	/* delay 400ns for PAPE */
	odm_set_bb_reg(p_dm_odm, 0x810, MASKBYTE3 | BIT(20) | BIT(21) | BIT(22) | BIT(23), 0x211);

	/* antenna switch table */
	odm_set_bb_reg(p_dm_odm, 0xca0, MASKLWORD, 0xa555);
	odm_set_bb_reg(p_dm_odm, 0xea0, MASKLWORD, 0xa555);

	/* inverse or not */
	odm_set_bb_reg(p_dm_odm, 0xcbc, (BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0)), 0x0);
	odm_set_bb_reg(p_dm_odm, 0xcbc, (BIT(11) | BIT(10)), 0x0);
	odm_set_bb_reg(p_dm_odm, 0xebc, (BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0)), 0x0);
	odm_set_bb_reg(p_dm_odm, 0xebc, (BIT(11) | BIT(10)), 0x0);
	/*}*/
#endif
	return true;
}

void
phydm_ccapar_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
#if 0
	u32	cca_ifem[9][4] = {
		/*20M*/
		{0x75D97010, 0x75D97010, 0x75D97010, 0x75D97010}, /*Reg82C*/
		{0x00000000, 0x00000000, 0x00000000, 0x00000000}, /*Reg830*/
		{0x00000000, 0x00000000, 0x00000000, 0x00000000}, /*Reg838*/
		/*40M*/
		{0x75D97010, 0x75D97010, 0x75D97010, 0x75D97010}, /*Reg82C*/
		{0x00000000, 0x79a0ea28, 0x00000000, 0x79a0ea28}, /*Reg830*/
		{0x87765541, 0x87766341, 0x87765541, 0x87766341}, /*Reg838*/
		/*80M*/
		{0x75D97010, 0x75D97010, 0x75D97010, 0x75D97010}, /*Reg82C*/
		{0x00000000, 0x00000000, 0x00000000, 0x00000000}, /*Reg830*/
		{0x00000000, 0x87746641, 0x00000000, 0x87746641}
	}; /*Reg838*/

	u32	cca_efem[9][4] = {
		/*20M*/
		{0x75A76010, 0x75A76010, 0x75A76010, 0x75A75010}, /*Reg82C*/
		{0x00000000, 0x00000000, 0x00000000, 0x00000000}, /*Reg830*/
		{0x87766651, 0x87766431, 0x87766451, 0x87766431}, /*Reg838*/
		/*40M*/
		{0x75A75010, 0x75A75010, 0x75A75010, 0x75A75010}, /*Reg82C*/
		{0x00000000, 0x00000000, 0x00000000, 0x00000000}, /*Reg830*/
		{0x87766431, 0x87766431, 0x87766431, 0x87766431}, /*Reg838*/
		/*80M*/
		{0x75BA7010, 0x75BA7010, 0x75BA7010, 0x75BA7010}, /*Reg82C*/
		{0x00000000, 0x00000000, 0x00000000, 0x00000000}, /*Reg830*/
		{0x87766431, 0x87766431, 0x87766431, 0x87766431}
	}; /*Reg838*/

	u8	row, col;
	u32	reg82c, reg830, reg838;

	if (p_dm_odm->cut_version != ODM_CUT_B)
		return;

	if (bw_8821c == ODM_BW20M)
		row = 0;
	else if (bw_8821c == ODM_BW40M)
		row = 3;
	else
		row = 6;

	if (central_ch_8821c <= 14) {
		if ((p_dm_odm->rx_ant_status == ODM_RF_A) || (p_dm_odm->rx_ant_status == ODM_RF_B))
			col = 0;
		else
			col = 1;
	} else {
		if ((p_dm_odm->rx_ant_status == ODM_RF_A) || (p_dm_odm->rx_ant_status == ODM_RF_B))
			col = 2;
		else
			col = 3;
	}

	if (p_dm_odm->rfe_type == 0) {/*iFEM*/
		reg82c = (cca_ifem[row][col] != 0) ? cca_ifem[row][col] : reg82c_8821c;
		reg830 = (cca_ifem[row + 1][col] != 0) ? cca_ifem[row + 1][col] : reg830_8821c;
		reg838 = (cca_ifem[row + 2][col] != 0) ? cca_ifem[row + 2][col] : reg838_8821c;
	} else {/*eFEM*/
		reg82c = (cca_efem[row][col] != 0) ? cca_efem[row][col] : reg82c_8821c;
		reg830 = (cca_efem[row + 1][col] != 0) ? cca_efem[row + 1][col] : reg830_8821c;
		reg838 = (cca_efem[row + 2][col] != 0) ? cca_efem[row + 2][col] : reg838_8821c;
	}

	odm_set_bb_reg(p_dm_odm, 0x82c, MASKDWORD, reg82c);
	odm_set_bb_reg(p_dm_odm, 0x830, MASKDWORD, reg830);
	odm_set_bb_reg(p_dm_odm, 0x838, MASKDWORD, reg838);

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Update CCA parameters for Bcut (Pkt%d, Intf%d, RFE%d), row = %d, col = %d\n",
		__func__, p_dm_odm->package_type, p_dm_odm->support_interface, p_dm_odm->rfe_type, row, col));
#endif
}

void
phydm_ccapar_by_bw_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	enum odm_bw_e				bandwidth
)
{
#if 0
	u32		reg82c;


	if (p_dm_odm->cut_version != ODM_CUT_A)
		return;

	/* A-cut */
	reg82c = odm_get_bb_reg(p_dm_odm, 0x82c, MASKDWORD);

	if (bandwidth == ODM_BW20M) {
		/* 82c[15:12] = 4 */
		/* 82c[27:24] = 6 */

		reg82c &= (~(0x0f00f000));
		reg82c |= ((0x4) << 12);
		reg82c |= ((0x6) << 24);
	} else if (bandwidth == ODM_BW40M) {
		/* 82c[19:16] = 9 */
		/* 82c[27:24] = 6 */

		reg82c &= (~(0x0f0f0000));
		reg82c |= ((0x9) << 16);
		reg82c |= ((0x6) << 24);
	} else if (bandwidth == ODM_BW80M) {
		/* 82c[15:12] 7 */
		/* 82c[19:16] b */
		/* 82c[23:20] d */
		/* 82c[27:24] 3 */

		reg82c &= (~(0x0ffff000));
		reg82c |= ((0xdb7) << 12);
		reg82c |= ((0x3) << 24);
	}

	odm_set_bb_reg(p_dm_odm, 0x82c, MASKDWORD, reg82c);
	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Update CCA parameters for Acut\n", __func__));
#endif
}

void
phydm_ccapar_by_rxpath_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
#if 0
	if (p_dm_odm->cut_version != ODM_CUT_A)
		return;

	if ((p_dm_odm->rx_ant_status == ODM_RF_A) || (p_dm_odm->rx_ant_status == ODM_RF_B)) {
		/* 838[7:4] = 8 */
		/* 838[11:8] = 7 */
		/* 838[15:12] = 6 */
		/* 838[19:16] = 7 */
		/* 838[23:20] = 7 */
		/* 838[27:24] = 7 */
		odm_set_bb_reg(p_dm_odm, 0x838, 0x0ffffff0, 0x777678);
	} else {
		/* 838[7:4] = 3 */
		/* 838[11:8] = 3 */
		/* 838[15:12] = 6 */
		/* 838[19:16] = 6 */
		/* 838[23:20] = 7 */
		/* 838[27:24] = 7 */
		odm_set_bb_reg(p_dm_odm, 0x838, 0x0ffffff0, 0x776633);
	}
	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Update CCA parameters for Acut\n", __func__));
#endif
}

void
phydm_rxdfirpar_by_bw_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	enum odm_bw_e				bandwidth
)
{
	if (bandwidth == ODM_BW40M) {
		/* RX DFIR for BW40 */
		odm_set_bb_reg(p_dm_odm, 0x948, BIT(29) | BIT(28), 0x2);
		odm_set_bb_reg(p_dm_odm, 0x94c, BIT(29) | BIT(28), 0x2);
		odm_set_bb_reg(p_dm_odm, 0xc20, BIT(31), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8f0, BIT(31), 0x0);
	} else if (bandwidth == ODM_BW80M) {
		/* RX DFIR for BW80 */
		odm_set_bb_reg(p_dm_odm, 0x948, BIT(29) | BIT(28), 0x2);
		odm_set_bb_reg(p_dm_odm, 0x94c, BIT(29) | BIT(28), 0x1);
		odm_set_bb_reg(p_dm_odm, 0xc20, BIT(31), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8f0, BIT(31), 0x1);
	} else {
		/* RX DFIR for BW20, BW10 and BW5*/
		odm_set_bb_reg(p_dm_odm, 0x948, BIT(29) | BIT(28), 0x2);
		odm_set_bb_reg(p_dm_odm, 0x94c, BIT(29) | BIT(28), 0x2);
		odm_set_bb_reg(p_dm_odm, 0xc20, BIT(31), 0x1);
		odm_set_bb_reg(p_dm_odm, 0x8f0, BIT(31), 0x0);
	}
}

boolean
phydm_write_txagc_1byte_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	u32					power_index,
	enum odm_rf_radio_path_e		path,
	u8					hw_rate
)
{
	u32	offset_txagc[2] = {0x1d00, 0x1d80};
	u8	rate_idx = (hw_rate & 0xfc), i;
	u8	rate_offset = (hw_rate & 0x3);
	u32	rate_mask = (0xff << (rate_offset << 3));
	u32	txagc_content = 0x0;

	/* For debug command only!!!! */

	/* Error handling */
	if ((path > ODM_RF_PATH_A) || (hw_rate > 0x53)) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: unsupported path (%d)\n", __func__, path));
		return false;
	}

#if 1
	/* For HW limitation, We can't write TXAGC once a byte. */
	for (i = 0; i < 4; i++) {
		if (i != rate_offset)
			txagc_content = txagc_content | (config_phydm_read_txagc_8821c(p_dm_odm, path, rate_idx + i) << (i << 3));
		else
			txagc_content = txagc_content | ((power_index & 0x3f) << (i << 3));
	}
	odm_set_bb_reg(p_dm_odm, (offset_txagc[path] + rate_idx), MASKDWORD, txagc_content);
#else
	odm_write_1byte(p_dm_odm, (offset_txagc[path] + hw_rate), (power_index & 0x3f));
#endif

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: path-%d rate index 0x%x (0x%x) = 0x%x\n",
		__func__, path, hw_rate, (offset_txagc[path] + hw_rate), power_index));
	return true;
}

void
phydm_init_hw_info_by_rfe_type_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	p_dm_odm->is_init_hw_info_by_rfe = false;
	/*2.4G default rf set with wlg or btg*/
	if (p_dm_odm->rfe_type == 2 || p_dm_odm->rfe_type == 4 || p_dm_odm->rfe_type == 7)
		p_dm_odm->default_rf_set_8821c = SWITCH_TO_BTG;
	else if (p_dm_odm->rfe_type == 0 || p_dm_odm->rfe_type == 1 || p_dm_odm->rfe_type == 3 || p_dm_odm->rfe_type == 5 || p_dm_odm->rfe_type == 6)
		p_dm_odm->default_rf_set_8821c = SWITCH_TO_WLG;
	else if (p_dm_odm->rfe_type == 0x22 || p_dm_odm->rfe_type == 0x24 || p_dm_odm->rfe_type == 0x27) {
		p_dm_odm->default_rf_set_8821c = SWITCH_TO_BTG;
		odm_cmn_info_init(p_dm_odm, ODM_CMNINFO_PACKAGE_TYPE, 1);
	} else if (p_dm_odm->rfe_type == 0x20 || p_dm_odm->rfe_type == 0x21 || p_dm_odm->rfe_type == 0x23 || p_dm_odm->rfe_type == 0x25 || p_dm_odm->rfe_type == 0x26) {
		p_dm_odm->default_rf_set_8821c = SWITCH_TO_WLG;
		odm_cmn_info_init(p_dm_odm, ODM_CMNINFO_PACKAGE_TYPE, 1);
	}

	if (p_dm_odm->rfe_type == 3 || p_dm_odm->rfe_type == 4 || p_dm_odm->rfe_type == 0x23 || p_dm_odm->rfe_type == 0x24)
		p_dm_odm->default_ant_num_8821c = SWITCH_TO_ANT2;
	else
		p_dm_odm->default_ant_num_8821c = SWITCH_TO_ANT1;

	p_dm_odm->is_init_hw_info_by_rfe = true;
	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("%s: RFE type (%d), rf set (%s)\n", __FUNCTION__, p_dm_odm->rfe_type, (p_dm_odm->default_rf_set_8821c == 0 ? "BTG" : "WLG")));
}

void
phydm_set_gnt_state_8821c(
	struct PHY_DM_STRUCT			*p_dm_odm,
	boolean				gnt_wl_state,
	boolean				gnt_bt_state
)
{
	u32		gnt_val = 0;

	odm_set_mac_reg(p_dm_odm, 0x70, BIT(26), 0x1);

	if (gnt_wl_state)
		gnt_val = 0x3300;
	else
		gnt_val = 0x1100;

	if (gnt_bt_state)
		gnt_val = gnt_val | 0xcc00;
	else
		gnt_val = gnt_val | 0x4400;

	odm_set_mac_reg(p_dm_odm, 0x1704, MASKLWORD, gnt_val);
	ODM_delay_us(50); /*waiting before access 0x1700 */
	odm_set_mac_reg(p_dm_odm, 0x1700, MASKDWORD, 0xc00f0038);
}
/* ======================================================================== */

/* ======================================================================== */
/* These following functions can be used by driver*/

u32
config_phydm_read_rf_reg_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	enum odm_rf_radio_path_e		rf_path,
	u32					reg_addr,
	u32					bit_mask
)
{
	u32	readback_value, direct_addr;
	u32	offset_read_rf[2] = {0x2800, 0x2c00};
	u32	power_RF[2] = {0x1c, 0xec};

	/* Error handling.*/
	if (rf_path > ODM_RF_PATH_A) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: unsupported path (%d)\n", __func__, rf_path));
		return INVALID_RF_DATA;
	}

	/*  Error handling. Check if RF power is enable or not */
	/*  0xffffffff means RF power is disable */
	if (odm_get_mac_reg(p_dm_odm, power_RF[rf_path], MASKBYTE3) != 0x7) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Read fail, RF is disabled\n", __func__));
		return INVALID_RF_DATA;
	}

	/* Calculate offset */
	reg_addr &= 0xff;
	direct_addr = offset_read_rf[rf_path] + (reg_addr << 2);

	/* RF register only has 20bits */
	bit_mask &= RFREGOFFSETMASK;

	/* Read RF register directly */
	readback_value = odm_get_bb_reg(p_dm_odm, direct_addr, bit_mask);
	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: RF-%d 0x%x = 0x%x, bit mask = 0x%x\n",
			__func__, rf_path, reg_addr, readback_value, bit_mask));
	return readback_value;
}

boolean
config_phydm_write_rf_reg_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	enum odm_rf_radio_path_e		rf_path,
	u32					reg_addr,
	u32					bit_mask,
	u32					data
)
{
	u32	data_and_addr = 0, data_original = 0;
	u32	offset_write_rf[2] = {0xc90, 0xe90};
	u32	power_RF[2] = {0x1c, 0xec};
	u8	bit_shift;

	/* Error handling.*/
	if (rf_path > ODM_RF_PATH_A) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: unsupported path (%d)\n", __func__, rf_path));
		return false;
	}

	/* Read RF register content first */
	reg_addr &= 0xff;
	bit_mask = bit_mask & RFREGOFFSETMASK;

	if (bit_mask != RFREGOFFSETMASK) {
		data_original = config_phydm_read_rf_reg_8821c(p_dm_odm, rf_path, reg_addr, RFREGOFFSETMASK);

		/* Error handling. RF is disabled */
		if (config_phydm_read_rf_check_8821c(data_original) == false) {
			ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Write fail, RF is disable\n", __func__));
			return false;
		}

		/* check bit mask */
		if (bit_mask != 0xfffff) {
			for (bit_shift = 0; bit_shift <= 19; bit_shift++) {
				if (((bit_mask >> bit_shift) & 0x1) == 1)
					break;
			}
			data = ((data_original)&(~bit_mask)) | (data << bit_shift);
		}
	} else if (odm_get_mac_reg(p_dm_odm, power_RF[rf_path], MASKBYTE3) != 0x7) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Write fail, RF is disabled\n", __func__));
		return false;
	}

	/* Put write addr in [27:20]  and write data in [19:00] */
	data_and_addr = ((reg_addr << 20) | (data & 0x000fffff)) & 0x0fffffff;

	/* Write operation */
	odm_set_bb_reg(p_dm_odm, offset_write_rf[rf_path], MASKDWORD, data_and_addr);
	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: RF-%d 0x%x = 0x%x (original: 0x%x), bit mask = 0x%x\n",
		__func__, rf_path, reg_addr, data, data_original, bit_mask));
	return true;
}

boolean
config_phydm_write_txagc_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	u32					power_index,
	enum odm_rf_radio_path_e		path,
	u8					hw_rate
)
{
	u32	offset_txagc[2] = {0x1d00, 0x1d80};
	u8	rate_idx = (hw_rate & 0xfc), i;
	u32	txagc_content = 0x0;

	/* Input need to be HW rate index, not driver rate index!!!! */

	if (p_dm_odm->is_disable_phy_api) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: disable PHY API for debug!!\n", __func__));
		return true;
	}

	/* Error handling */
	if ((path > ODM_RF_PATH_A) || (hw_rate > 0x53)) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: unsupported path (%d)\n", __func__, path));
		return false;
	}

	/* driver need to construct a 4-byte power index */
	odm_set_bb_reg(p_dm_odm, (offset_txagc[path] + rate_idx), MASKDWORD, power_index);

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: path-%d rate index 0x%x (0x%x) = 0x%x\n",
		__func__, path, hw_rate, (offset_txagc[path] + hw_rate), power_index));
	return true;
}

u8
config_phydm_read_txagc_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	enum odm_rf_radio_path_e		path,
	u8					hw_rate
)
{
	u8	read_back_data;

	/* Input need to be HW rate index, not driver rate index!!!! */

	/* Error handling */
	if ((path > ODM_RF_PATH_A) || (hw_rate > 0x53)) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: unsupported path (%d)\n", __func__, path));
		return INVALID_TXAGC_DATA;
	}

	/* Disable TX AGC report */
	odm_set_bb_reg(p_dm_odm, 0x1998, BIT(16), 0x0);							/* need to check */

	/* Set data rate index (bit0~6) and path index (bit7) */
	odm_set_bb_reg(p_dm_odm, 0x1998, MASKBYTE0, (hw_rate | (path << 7)));

	/* Enable TXAGC report */
	odm_set_bb_reg(p_dm_odm, 0x1998, BIT(16), 0x1);

	/* Read TX AGC report */
	read_back_data = (u8)odm_get_bb_reg(p_dm_odm, 0xd30, 0x7f0000);

	/* Driver have to disable TXAGC report after reading TXAGC (ref. user guide v11) */
	odm_set_bb_reg(p_dm_odm, 0x1998, BIT(16), 0x0);

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: path-%d rate index 0x%x = 0x%x\n", __func__, path, hw_rate, read_back_data));
	return read_back_data;
}

boolean
config_phydm_switch_band_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	u8					central_ch
)
{
	u32		rf_reg18;
	boolean		rf_reg_status = true;

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]======================>\n", __func__));

	if (p_dm_odm->is_disable_phy_api) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: disable PHY API for debug!!\n", __func__));
		return true;
	}

	rf_reg18 = config_phydm_read_rf_reg_8821c(p_dm_odm, ODM_RF_PATH_A, 0x18, RFREGOFFSETMASK);
	rf_reg_status = rf_reg_status & config_phydm_read_rf_check_8821c(rf_reg18);

	if (central_ch <= 14) {
		/* 2.4G */

		/* Enable CCK block */
		odm_set_bb_reg(p_dm_odm, 0x808, BIT(28), 0x1);

		/* Disable MAC CCK check */
		odm_set_bb_reg(p_dm_odm, 0x454, BIT(7), 0x0);

		/* Disable BB CCK check */
		odm_set_bb_reg(p_dm_odm, 0xa80, BIT(18), 0x0);

		/*CCA Mask*/
		odm_set_bb_reg(p_dm_odm, 0x814, 0x0000FC00, 15); /*default value*/

		/* RF band */
		rf_reg18 = (rf_reg18 & (~(BIT(16) | BIT(9) | BIT(8))));

		/* Switch WLG/BTG*/
		if (p_dm_odm->default_rf_set_8821c == SWITCH_TO_BTG)
			config_phydm_switch_rf_set_8821c(p_dm_odm, SWITCH_TO_BTG);
		else if (p_dm_odm->default_rf_set_8821c == SWITCH_TO_WLG)
			config_phydm_switch_rf_set_8821c(p_dm_odm, SWITCH_TO_WLG);

		/*RF TXA_TANK LUT mode*/
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, 0xdf, BIT(6), 0x1);

		/*RF TXA_PA_TANK*/
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, 0x64, 0x0000f, 0xf);

	} else if (central_ch > 35) {
		/* 5G */

		/* Enable BB CCK check */
		odm_set_bb_reg(p_dm_odm, 0xa80, BIT(18), 0x1);

		/* Enable CCK check */
		odm_set_bb_reg(p_dm_odm, 0x454, BIT(7), 0x1);

		/* Disable CCK block */
		odm_set_bb_reg(p_dm_odm, 0x808, BIT(28), 0x0);

		/*CCA Mask*/
		odm_set_bb_reg(p_dm_odm, 0x814, 0x0000FC00, 15); /*default value*/
		/*odm_set_bb_reg(p_dm_odm, 0x814, 0x0000FC00, 34); CCA mask = 13.6us*/

		/* RF band */
		rf_reg18 = (rf_reg18 & (~(BIT(16) | BIT(9) | BIT(8))));
		rf_reg18 = (rf_reg18 | BIT(8) | BIT(16));

		/* Switch WLA */
		config_phydm_switch_rf_set_8821c(p_dm_odm, SWITCH_TO_WLA);

		/*RF TXA_TANK LUT mode*/
		odm_set_rf_reg(p_dm_odm, ODM_RF_PATH_A, 0xdf, BIT(6), 0x0);

	} else {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch band (ch: %d)\n", __func__, central_ch));
		return false;
	}

	rf_reg_status = rf_reg_status & config_phydm_write_rf_reg_8821c(p_dm_odm, ODM_RF_PATH_A, 0x18, RFREGOFFSETMASK, rf_reg18);

	if (phydm_rfe_8821c(p_dm_odm, central_ch) == false)
		return false;

	if (rf_reg_status == false) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch band (ch: %d), because writing RF register is fail\n", __func__, central_ch));
		return false;
	}

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Success to switch band (ch: %d)\n", __func__, central_ch));
	return true;
}

boolean
config_phydm_switch_channel_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	u8					central_ch
)
{
	struct _dynamic_initial_gain_threshold_		*p_dm_dig_table = &p_dm_odm->dm_dig_table;
	u32		rf_reg18, rf_reg_b8 = 0;
	boolean		rf_reg_status = true;

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]====================>\n", __func__));

	if (p_dm_odm->is_disable_phy_api) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: disable PHY API for debug!!\n", __func__));
		return true;
	}

	central_ch_8821c = central_ch;
	rf_reg18 = config_phydm_read_rf_reg_8821c(p_dm_odm, ODM_RF_PATH_A, 0x18, RFREGOFFSETMASK);
	rf_reg_status = rf_reg_status & config_phydm_read_rf_check_8821c(rf_reg18);

	if (p_dm_odm->cut_version == ODM_CUT_A) {
		rf_reg_b8 = config_phydm_read_rf_reg_8821c(p_dm_odm, ODM_RF_PATH_A, 0xb8, RFREGOFFSETMASK);
		rf_reg_status = rf_reg_status & config_phydm_read_rf_check_8821c(rf_reg_b8);
	}

	/* Switch band and channel */
	if (central_ch <= 14) {
		/* 2.4G */

		/* 1. RF band and channel*/
		rf_reg18 = (rf_reg18 & (~(BIT(18) | BIT(17) | MASKBYTE0)));
		rf_reg18 = (rf_reg18 | central_ch);

		/* 2. AGC table selection */
		odm_set_bb_reg(p_dm_odm, 0xc1c, 0x00000F00, 0x0);
		p_dm_dig_table->agc_table_idx = 0x0;

		/* 3. Set central frequency for clock offset tracking */
		odm_set_bb_reg(p_dm_odm, 0x860, 0x1ffe0000, 0x96a);

		/* Fix A-cut LCK fail issue @ 5285MHz~5375MHz, 0xb8[19]=0x0 */
		if (p_dm_odm->cut_version == ODM_CUT_A)
			rf_reg_b8 = rf_reg_b8 | BIT(19);

		/* CCK TX filter parameters */
		if (central_ch == 14) {
			odm_set_bb_reg(p_dm_odm, 0xa24, MASKDWORD, 0x0000b81c);
			odm_set_bb_reg(p_dm_odm, 0xa28, MASKLWORD, 0x0000);
			odm_set_bb_reg(p_dm_odm, 0xaac, MASKDWORD, 0x00003667);
		} else {
			odm_set_bb_reg(p_dm_odm, 0xa24, MASKDWORD, rega24_8821c);
			odm_set_bb_reg(p_dm_odm, 0xa28, MASKLWORD, (rega28_8821c & MASKLWORD));
			odm_set_bb_reg(p_dm_odm, 0xaac, MASKDWORD, regaac_8821c);
		}

	} else if (central_ch > 35) {
		/* 5G */

		/* 1. RF band and channel*/
		rf_reg18 = (rf_reg18 & (~(BIT(18) | BIT(17) | MASKBYTE0)));
		rf_reg18 = (rf_reg18 | central_ch);

		if (central_ch >= 36 && central_ch <= 64)
			;
		else if ((central_ch >= 100) && (central_ch <= 140))
			rf_reg18 = (rf_reg18 | BIT(17));
		else if (central_ch > 140)
			rf_reg18 = (rf_reg18 | BIT(18));
		else {
			ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch channel (RF18) (ch: %d)\n", __func__, central_ch));
			return false;
		}

		/* 2. AGC table selection */
		if ((central_ch >= 36) && (central_ch <= 64)) {
			odm_set_bb_reg(p_dm_odm, 0xc1c, 0x00000F00, 0x1);
			p_dm_dig_table->agc_table_idx = 0x1;
		} else if ((central_ch >= 100) && (central_ch <= 144)) {
			odm_set_bb_reg(p_dm_odm, 0xc1c, 0x00000F00, 0x2);
			p_dm_dig_table->agc_table_idx = 0x2;
		} else if (central_ch >= 149) {
			odm_set_bb_reg(p_dm_odm, 0xc1c, 0x00000F00, 0x3);
			p_dm_dig_table->agc_table_idx = 0x3;
		} else {
			ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch channel (AGC) (ch: %d)\n", __func__, central_ch));
			return false;
		}

		/* 3. Set central frequency for clock offset tracking */
		if ((central_ch >= 36) && (central_ch <= 48))
			odm_set_bb_reg(p_dm_odm, 0x860, 0x1ffe0000, 0x494);
		else if ((central_ch >= 52) && (central_ch <= 64))
			odm_set_bb_reg(p_dm_odm, 0x860, 0x1ffe0000, 0x453);
		else if ((central_ch >= 100) && (central_ch <= 116))
			odm_set_bb_reg(p_dm_odm, 0x860, 0x1ffe0000, 0x452);
		else if ((central_ch >= 118) && (central_ch <= 177))
			odm_set_bb_reg(p_dm_odm, 0x860, 0x1ffe0000, 0x412);
		else {
			ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch channel (fc_area) (ch: %d)\n", __func__, central_ch));
			return false;
		}

		/* Fix A-cut LCK fail issue @ 5285MHz~5375MHz, 0xb8[19]=0x0 */
		if (p_dm_odm->cut_version == ODM_CUT_A) {
			if ((central_ch >= 57) && (central_ch <= 75))
				rf_reg_b8 = rf_reg_b8 & (~BIT(19));
			else
				rf_reg_b8 = rf_reg_b8 | BIT(19);
		}

	} else {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch band (ch: %d)\n", __func__, central_ch));
		return false;
	}

	rf_reg_status = rf_reg_status & config_phydm_write_rf_reg_8821c(p_dm_odm, ODM_RF_PATH_A, 0x18, RFREGOFFSETMASK, rf_reg18);

	if (p_dm_odm->cut_version == ODM_CUT_A)
		rf_reg_status = rf_reg_status & config_phydm_write_rf_reg_8821c(p_dm_odm, ODM_RF_PATH_A, 0xb8, RFREGOFFSETMASK, rf_reg_b8);

	if (rf_reg_status == false) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch channel (ch: %d), because writing RF register is fail\n", __func__, central_ch));
		return false;
	}

	phydm_ccapar_8821c(p_dm_odm);
	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Success to switch channel (ch: %d)\n", __func__, central_ch));
	return true;
}

boolean
config_phydm_switch_bandwidth_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	u8					primary_ch_idx,
	enum odm_bw_e				bandwidth
)
{
	u32		rf_reg18;
	boolean		rf_reg_status = true;

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]===================>\n", __func__));

	if (p_dm_odm->is_disable_phy_api) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: disable PHY API for debug!!\n", __func__));
		return true;
	}

	/* Error handling */
	if ((bandwidth >= ODM_BW_MAX) || ((bandwidth == ODM_BW40M) && (primary_ch_idx > 2)) || ((bandwidth == ODM_BW80M) && (primary_ch_idx > 4))) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch bandwidth (bw: %d, primary ch: %d)\n", __func__, bandwidth, primary_ch_idx));
		return false;
	}

	bw_8821c = bandwidth;
	rf_reg18 = config_phydm_read_rf_reg_8821c(p_dm_odm, ODM_RF_PATH_A, 0x18, RFREGOFFSETMASK);
	rf_reg_status = rf_reg_status & config_phydm_read_rf_check_8821c(rf_reg18);

	/* Switch bandwidth */
	switch (bandwidth) {
	case ODM_BW20M:
	{
		/* Small BW([7:6]) = 0, primary channel ([5:2]) = 0, rf mode([1:0]) = 20M */
		odm_set_bb_reg(p_dm_odm, 0x8ac, MASKBYTE0, ODM_BW20M);

		/* ADC clock = 160M clock for BW20 */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(9) | BIT(8)), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(16), 0x1);

		/* DAC clock = 160M clock for BW20 */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(21) | BIT(20)), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(28), 0x1);

		/* ADC buffer clock */
		odm_set_bb_reg(p_dm_odm, 0x8c4, BIT(30), 0x1);

		/* RF bandwidth */
		rf_reg18 = (rf_reg18 | BIT(11) | BIT(10));

		break;
	}
	case ODM_BW40M:
	{
		/* Small BW([7:6]) = 0, primary channel ([5:2]) = sub-channel, rf mode([1:0]) = 40M */
		odm_set_bb_reg(p_dm_odm, 0x8ac, MASKBYTE0, (((primary_ch_idx & 0xf) << 2) | ODM_BW40M));

		/* CCK primary channel */
		if (primary_ch_idx == 1)
			odm_set_bb_reg(p_dm_odm, 0xa00, BIT(4), primary_ch_idx);
		else
			odm_set_bb_reg(p_dm_odm, 0xa00, BIT(4), 0);

		/* ADC clock = 160M clock for BW40 */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(11) | BIT(10)), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(17), 0x1);

		/* DAC clock = 160M clock for BW20 */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(23) | BIT(22)), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(29), 0x1);

		/* ADC buffer clock */
		odm_set_bb_reg(p_dm_odm, 0x8c4, BIT(30), 0x1);

		/* RF bandwidth */
		rf_reg18 = (rf_reg18 & (~(BIT(11) | BIT(10))));
		rf_reg18 = (rf_reg18 | BIT(11));

		break;
	}
	case ODM_BW80M:
	{
		/* Small BW([7:6]) = 0, primary channel ([5:2]) = sub-channel, rf mode([1:0]) = 80M */
		odm_set_bb_reg(p_dm_odm, 0x8ac, MASKBYTE0, (((primary_ch_idx & 0xf) << 2) | ODM_BW80M));

		/* ADC clock = 160M clock for BW80 */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(13) | BIT(12)), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(18), 0x1);

		/* DAC clock = 160M clock for BW20 */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(25) | BIT(24)), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(30), 0x1);

		/* ADC buffer clock */
		odm_set_bb_reg(p_dm_odm, 0x8c4, BIT(30), 0x1);

		/* RF bandwidth */
		rf_reg18 = (rf_reg18 & (~(BIT(11) | BIT(10))));
		rf_reg18 = (rf_reg18 | BIT(10));

		break;
	}
	case ODM_BW5M:
	{
		/* Small BW([7:6]) = 1, primary channel ([5:2]) = 0, rf mode([1:0]) = 20M */
		odm_set_bb_reg(p_dm_odm, 0x8ac, MASKBYTE0, (BIT(6) | ODM_BW20M));

		/* ADC clock = 40M clock */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(9) | BIT(8)), 0x2);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(16), 0x0);

		/* DAC clock = 160M clock for BW20 */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(21) | BIT(20)), 0x2);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(28), 0x0);

		/* ADC buffer clock */
		odm_set_bb_reg(p_dm_odm, 0x8c4, BIT(30), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8c8, BIT(31), 0x1);

		/* RF bandwidth */
		rf_reg18 = (rf_reg18 | BIT(11) | BIT(10));

		break;
	}
	case ODM_BW10M:
	{
		/* Small BW([7:6]) = 1, primary channel ([5:2]) = 0, rf mode([1:0]) = 20M */
		odm_set_bb_reg(p_dm_odm, 0x8ac, MASKBYTE0, (BIT(7) | ODM_BW20M));

		/* ADC clock = 80M clock */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(9) | BIT(8)), 0x3);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(16), 0x0);

		/* DAC clock = 160M clock for BW20 */
		odm_set_bb_reg(p_dm_odm, 0x8ac, (BIT(21) | BIT(20)), 0x3);
		odm_set_bb_reg(p_dm_odm, 0x8ac, BIT(28), 0x0);

		/* ADC buffer clock */
		odm_set_bb_reg(p_dm_odm, 0x8c4, BIT(30), 0x0);
		odm_set_bb_reg(p_dm_odm, 0x8c8, BIT(31), 0x1);

		/* RF bandwidth */
		rf_reg18 = (rf_reg18 | BIT(11) | BIT(10));

		break;
	}
	default:
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch bandwidth (bw: %d, primary ch: %d)\n", __func__, bandwidth, primary_ch_idx));
	}

	/* Write RF register */
	rf_reg_status = rf_reg_status & config_phydm_write_rf_reg_8821c(p_dm_odm, ODM_RF_PATH_A, 0x18, RFREGOFFSETMASK, rf_reg18);

	if (rf_reg_status == false) {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Fail to switch bandwidth (bw: %d, primary ch: %d), because writing RF register is fail\n", __func__, bandwidth, primary_ch_idx));
		return false;
	}

	/* Modify RX DFIR parameters */
	phydm_rxdfirpar_by_bw_8821c(p_dm_odm, bandwidth);

	/* Modify CCA parameters */
	phydm_ccapar_by_bw_8821c(p_dm_odm, bandwidth);
	phydm_ccapar_8821c(p_dm_odm);

	/* Toggle RX path to avoid RX dead zone issue */
	/*odm_set_bb_reg(p_dm_odm, 0x808, MASKBYTE0, 0x0);*/
	/*odm_set_bb_reg(p_dm_odm, 0x808, MASKBYTE0, 0x11);*/



	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Success to switch bandwidth (bw: %d, primary ch: %d)\n", __func__, bandwidth, primary_ch_idx));
	return true;
}

boolean
config_phydm_switch_channel_bw_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	u8					central_ch,
	u8					primary_ch_idx,
	enum odm_bw_e				bandwidth
)
{
	u8			e_rf_path = 0;
	u32			rf_val_to_wr, rf_tmp_val, bit_shift, bit_mask;

	/* Switch band */
	if (config_phydm_switch_band_8821c(p_dm_odm, central_ch) == false)
		return false;

	/* Switch channel */
	if (config_phydm_switch_channel_8821c(p_dm_odm, central_ch) == false)
		return false;

	/* Switch bandwidth */
	if (config_phydm_switch_bandwidth_8821c(p_dm_odm, primary_ch_idx, bandwidth) == false)
		return false;

	return true;
}

boolean
config_phydm_trx_mode_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	enum odm_rf_path_e			tx_path,
	enum odm_rf_path_e			rx_path,
	boolean					is_tx2_path
)
{
	return true;
}

boolean
config_phydm_parameter_init_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm,
	enum odm_parameter_init_e	type
)
{
	if (type == ODM_PRE_SETTING) {
		odm_set_bb_reg(p_dm_odm, 0x808, (BIT(28) | BIT(29)), 0x0);
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Pre setting: disable OFDM and CCK block\n", __func__));
	} else if (type == ODM_POST_SETTING) {
		odm_set_bb_reg(p_dm_odm, 0x808, (BIT(28) | BIT(29)), 0x3);
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Post setting: enable OFDM and CCK block\n", __func__));
		reg82c_8821c = odm_get_bb_reg(p_dm_odm, 0x82c, MASKDWORD);
		reg838_8821c = odm_get_bb_reg(p_dm_odm, 0x838, MASKDWORD);
		reg830_8821c = odm_get_bb_reg(p_dm_odm, 0x830, MASKDWORD);
		rega24_8821c = odm_get_bb_reg(p_dm_odm, 0xa24, MASKDWORD);
		rega28_8821c = odm_get_bb_reg(p_dm_odm, 0xa28, MASKDWORD);
		regaac_8821c = odm_get_bb_reg(p_dm_odm, 0xaac, MASKDWORD);
	} else {
		ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: Wrong type!!\n", __func__));
		return false;
	}

	return true;
}

void
config_phydm_switch_rf_set_8821c(
	struct PHY_DM_STRUCT			*p_dm_odm,
	u8				rf_set
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*p_adapter	= p_dm_odm->adapter;
	PMGNT_INFO			p_mgnt_info = &(p_adapter->MgntInfo);
#endif

	u32		bb_reg32;

	odm_set_mac_reg(p_dm_odm, 0x1080, BIT(16), 0x1);
	odm_set_mac_reg(p_dm_odm, 0x00, BIT(26), 0x1);
	/*odm_set_mac_reg(p_dm_odm, 0x70, BIT(26), 0x1);*/
	/*odm_set_mac_reg(p_dm_odm, 0x1704, MASKLWORD, 0x4000);*/
	/*odm_set_mac_reg(p_dm_odm, 0x1700, (BIT(31) | BIT(30)), 0x3); */

	bb_reg32 = odm_get_bb_reg(p_dm_odm, 0xcb8, MASKDWORD);
	switch (rf_set) {

	case SWITCH_TO_BTG:

		p_dm_odm->current_rf_set_8821c = SWITCH_TO_BTG;

		bb_reg32 = (bb_reg32 | BIT(16));
		bb_reg32 &= (~(BIT(18) | BIT(20) | BIT(21) | BIT(22) | BIT(23)));
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		if (p_dm_odm->mp_mode == true && p_mgnt_info->RegPHYParaFromFolder == 0)
#else
		if (p_dm_odm->mp_mode == true)
#endif
		{
			odm_config_bb_with_header_file(p_dm_odm, CONFIG_BB_AGC_TAB_DIFF);
			/*Toggle initial gain twice for valid gain table*/
			odm_set_bb_reg(p_dm_odm, ODM_REG(IGI_A, p_dm_odm), ODM_BIT(IGI, p_dm_odm), 0x22);
			odm_set_bb_reg(p_dm_odm, ODM_REG(IGI_A, p_dm_odm), ODM_BIT(IGI, p_dm_odm), 0x20);
		}
		break;
	case SWITCH_TO_WLG:

		p_dm_odm->current_rf_set_8821c = SWITCH_TO_WLG;

		bb_reg32 = (bb_reg32 | BIT(20) | BIT(21) | BIT(22));
		bb_reg32 &= (~(BIT(16) | BIT(18) | BIT(23)));
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		if (p_dm_odm->mp_mode == true && p_mgnt_info->RegPHYParaFromFolder == 0)
#else
		if (p_dm_odm->mp_mode == true)
#endif
		{
			odm_config_bb_with_header_file(p_dm_odm, CONFIG_BB_AGC_TAB_DIFF);
			/*Toggle initial gain twice for valid gain table*/
			odm_set_bb_reg(p_dm_odm, ODM_REG(IGI_A, p_dm_odm), ODM_BIT(IGI, p_dm_odm), 0x22);
			odm_set_bb_reg(p_dm_odm, ODM_REG(IGI_A, p_dm_odm), ODM_BIT(IGI, p_dm_odm), 0x20);
		}
		break;
	case SWITCH_TO_WLA:

		p_dm_odm->current_rf_set_8821c = SWITCH_TO_WLA;

		bb_reg32 = (bb_reg32 | BIT(20) | BIT(22) | BIT(23));
		bb_reg32 &= (~(BIT(16) | BIT(18) | BIT(21)));

		break;
	case SWITCH_TO_BT:

		p_dm_odm->current_rf_set_8821c = SWITCH_TO_BT;

		break;
	default:
		break;

	}

	odm_set_bb_reg(p_dm_odm, 0xcb8, MASKDWORD, bb_reg32);

}

void
config_phydm_set_ant_path(
	struct PHY_DM_STRUCT				*p_dm_odm,
	u8				rf_set,
	u8				ant_num
)
{
	boolean		switch_polarity_inverse = false;
	u8			regval_0xcb7 = 0;

	p_dm_odm->current_ant_num_8821c = ant_num;
	config_phydm_switch_rf_set_8821c(p_dm_odm, rf_set);

	if (rf_set == SWITCH_TO_BT)
		phydm_set_gnt_state_8821c(p_dm_odm, false, true); /* GNT_WL=0, GNT_BT=1 for BT test */
	else
		phydm_set_gnt_state_8821c(p_dm_odm, true, false); /* GNT_WL=1, GNT_BT=0 for WL test */

	if (p_dm_odm->rfe_type == 0x5 || p_dm_odm->rfe_type == 0x6 || p_dm_odm->rfe_type == 0x25 || p_dm_odm->rfe_type == 0x26) /*switch does not exist*/
		return;

	if (p_dm_odm->current_ant_num_8821c) /*Ant1 = 0, Ant2 = 1*/
		switch_polarity_inverse = !switch_polarity_inverse;

	if (rf_set == SWITCH_TO_WLG)
		switch_polarity_inverse = !switch_polarity_inverse;

	/*set antenna control by WL 0xcb4[29:28]*/
	odm_set_mac_reg(p_dm_odm, 0x4c, BIT(24)|BIT(23), 0x2);

	/*set RFE_ctrl8 and RFE_ctrl9 as antenna control pins by software*/
	odm_set_bb_reg(p_dm_odm, 0xcb4, 0x000000ff, 0x77);

	/*0xcb4[29:28] = 2b'01 for no switch_polatiry_inverse, DPDT_SEL_N =1, DPDT_SEL_P =0*/
	regval_0xcb7 = (switch_polarity_inverse == false ? 0x1 : 0x2);

	odm_set_bb_reg(p_dm_odm, 0xcb4, 0x30000000, regval_0xcb7);

}

u32
query_phydm_trx_capability_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	u32 value32 = 0x00000000;

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: trx_capability = 0x%x\n", __func__, value32));
	return value32;
}

u32
query_phydm_stbc_capability_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	u32 value32 = 0x00010001;

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: stbc_capability = 0x%x\n", __func__, value32));
	return value32;
}

u32
query_phydm_ldpc_capability_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	u32 value32 = 0x01000100;

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: ldpc_capability = 0x%x\n", __func__, value32));
	return value32;
}

u32
query_phydm_txbf_parameters_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	u32 value32 = 0x00030003;

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: txbf_parameters = 0x%x\n", __func__, value32));
	return value32;
}

u32
query_phydm_txbf_capability_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	u32 value32 = 0x01010001;

	ODM_RT_TRACE(p_dm_odm, ODM_PHY_CONFIG, ODM_DBG_TRACE, ("[%s]: txbf_capability = 0x%x\n", __func__, value32));
	return value32;
}

u8
query_phydm_default_rf_set_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	return p_dm_odm->default_rf_set_8821c;
}

u8
query_phydm_current_rf_set_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	return p_dm_odm->current_rf_set_8821c;
}

u8
query_phydm_rfetype_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	return p_dm_odm->rfe_type;
}

u8
query_phydm_current_ant_num_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{
	u32				regval_0xcb4 = odm_get_bb_reg(p_dm_odm, 0xcb4, BIT(29)|BIT(28));

	if (p_dm_odm->current_rf_set_8821c == SWITCH_TO_BTG || p_dm_odm->current_rf_set_8821c == SWITCH_TO_WLA || p_dm_odm->current_rf_set_8821c == SWITCH_TO_BT) {
		if (regval_0xcb4 == 1)
			p_dm_odm->current_ant_num_8821c = SWITCH_TO_ANT1;
		else if (regval_0xcb4 == 2)
			p_dm_odm->current_ant_num_8821c = SWITCH_TO_ANT2;
	else
		if (regval_0xcb4 == 1)
			p_dm_odm->current_ant_num_8821c = SWITCH_TO_ANT2;
		else if (regval_0xcb4 == 2)
			p_dm_odm->current_ant_num_8821c = SWITCH_TO_ANT1;
	}

	return p_dm_odm->current_ant_num_8821c;
}

u8
query_phydm_ant_num_map_8821c(
	struct PHY_DM_STRUCT				*p_dm_odm
)
{

	u8		mapping_table = 0;

	/* mapping table meaning
	1: choose ant1 or ant2
	2: only ant1
	3: only ant2
	4: cannot choose
	*/

	if (p_dm_odm->rfe_type == 0 || p_dm_odm->rfe_type == 7 || p_dm_odm->rfe_type == 0x20 || p_dm_odm->rfe_type == 0x27)
		mapping_table = 1;
	else if (p_dm_odm->rfe_type == 1 || p_dm_odm->rfe_type == 2 || p_dm_odm->rfe_type == 0x21 || p_dm_odm->rfe_type == 0x22)
		mapping_table = 2;
	else if (p_dm_odm->rfe_type == 3 || p_dm_odm->rfe_type == 4 || p_dm_odm->rfe_type == 0x23 || p_dm_odm->rfe_type == 0x24)
		mapping_table = 3;
	else if (p_dm_odm->rfe_type == 5 || p_dm_odm->rfe_type == 6 || p_dm_odm->rfe_type == 0x25 || p_dm_odm->rfe_type == 0x26)
		mapping_table = 4;

	return mapping_table;
}

/* ======================================================================== */
#endif	/* RTL8821C_SUPPORT == 1 */

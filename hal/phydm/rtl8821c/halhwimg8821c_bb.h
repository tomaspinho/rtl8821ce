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

/*Image2HeaderVersion: 3.4*/
#if (RTL8821C_SUPPORT == 1)
#ifndef __INC_MP_BB_HW_IMG_8821C_H
#define __INC_MP_BB_HW_IMG_8821C_H


/******************************************************************************
*                           agc_tab.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_agc_tab(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm_odm
);
u32	odm_get_version_mp_8821c_agc_tab(void);

/******************************************************************************
*                           agc_tab_diff.TXT
******************************************************************************/

extern u32	array_mp_8821c_agc_tab_diff_wlg[520];
extern u32	array_mp_8821c_agc_tab_diff_btg[520];
void
odm_read_and_config_mp_8821c_agc_tab_diff(
	struct PHY_DM_STRUCT *p_dm_odm,
	u32	array[],
	u32	array_len
);
u32	odm_get_version_mp_8821c_agc_tab_diff(void);

/******************************************************************************
*                           phy_reg.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_phy_reg(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm_odm
);
u32	odm_get_version_mp_8821c_phy_reg(void);

/******************************************************************************
*                           phy_reg_mp.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_phy_reg_mp(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm_odm
);
u32	odm_get_version_mp_8821c_phy_reg_mp(void);

/******************************************************************************
*                           phy_reg_pg.TXT
******************************************************************************/

void
odm_read_and_config_mp_8821c_phy_reg_pg(/* tc: Test Chip, mp: mp Chip*/
	struct	PHY_DM_STRUCT *p_dm_odm
);
u32	odm_get_version_mp_8821c_phy_reg_pg(void);

#endif
#endif /* end of HWIMG_SUPPORT*/


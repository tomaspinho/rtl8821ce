/******************************************************************************
 *
 * Copyright(c) 2015 - 2016 Realtek Corporation. All rights reserved.
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
#define _RTL8821CE_HALMAC_C_
#include <drv_types.h>		/* struct dvobj_priv and etc. */
#include "../../hal_halmac.h"
#include "rtl8821ce.h"

static u8 pci_write_port_not_xmitframe(void *d,  u32 size, u8 *pBuf)
{
	struct dvobj_priv *pobj = (struct dvobj_priv *)d;
	struct pci_dev *pdev = pobj->ppcidev;
	PADAPTER padapter = dvobj_get_primary_adapter(pobj);
	u8 *txbd;
	u64 txbd_dma;
	u8 ret = _SUCCESS;
	dma_addr_t mapping;

	u16 seg_num = 2 << TX_BUFFER_SEG_NUM;

	u16 page_size_length = 0;

	/* map TX DESC buf_addr (including TX DESC + tx data) */
	mapping = pci_map_single(pdev, pBuf,
			size+TX_WIFI_INFO_SIZE, PCI_DMA_TODEVICE);


	/* Calculate page size.
	 * Total buffer length including TX_WIFI_INFO and PacketLen
	 */
	page_size_length =
		(size + TX_WIFI_INFO_SIZE) / HALMAC_TX_PAGE_SIZE_8821C;

	if (((size + TX_WIFI_INFO_SIZE) % HALMAC_TX_PAGE_SIZE_8821C) > 0)
		page_size_length++;

	txbd = pci_alloc_consistent(pdev,
		sizeof(struct tx_buf_desc), &txbd_dma);

	if (!txbd) {
		pci_unmap_single(pdev, mapping,
			size + TX_WIFI_INFO_SIZE, PCI_DMA_FROMDEVICE);

		return _FALSE;
	}

	/*
	 * Reset all tx buffer desciprtor content
	 * -- Reset first element
	 */
	_rtw_memset(txbd, 0, sizeof(struct tx_buf_desc));

	/*
	 * Fill buffer length of the first buffer,
	 * For 8821ce, it is required that TX_WIFI_INFO is put in first segment,
	 * and the size of the first segment cannot be larger than
	 * TX_WIFI_INFO_SIZE.
	 */
	SET_TX_BD_TX_BUFF_SIZE0(txbd, TX_WIFI_INFO_SIZE);
	SET_TX_BD_PSB(txbd, page_size_length);
	/* starting addr of TXDESC */
	SET_TX_BD_PHYSICAL_ADDR0_LOW(txbd, mapping);

	/*
	 * It is assumed that in linux implementation, packet is coalesced
	 * in only one buffer. Extension mode is not supported here
	 */
	SET_TXBUFFER_DESC_LEN_WITH_OFFSET(txbd, 1, size);

	SET_TXBUFFER_DESC_ADD_LOW_WITH_OFFSET(txbd, 1,
				      mapping + TX_WIFI_INFO_SIZE); /* pkt */

	/* Set BCN BD Reg */
	rtw_write32(padapter, REG_BCNQ_TXBD_DESA_8821C,
		    txbd_dma &  DMA_BIT_MASK(32));

	/* Need Comment */
	wmb();

	/* fill_txbd_own*/
	SET_TX_BD_OWN(txbd, 1);

	/* kick start */
	rtw_write8(padapter, REG_RX_RXBD_NUM + 1,
		rtw_read8(padapter, REG_RX_RXBD_NUM + 1) | BIT(4));

	udelay(100);

	pci_free_consistent(pdev, sizeof(*txbd), txbd, txbd_dma);

	pci_unmap_single(pdev, mapping,
			size + TX_WIFI_INFO_SIZE,	 PCI_DMA_FROMDEVICE);

	return ret;

}

static u8 pci_write_data_not_xmitframe(void *d, u8 *pBuf, u32 size, u8 qsel)
{
	struct dvobj_priv *pobj = (struct dvobj_priv *)d;
	PADAPTER padapter = dvobj_get_primary_adapter(pobj);
	PHALMAC_ADAPTER halmac = dvobj_to_halmac((struct dvobj_priv *)d);
	PHALMAC_API api = HALMAC_GET_API(halmac);
	u8 desclen = 0;
	u8 *buf = NULL;
	u8 ret = _FALSE;

	if ((size + TXDESC_OFFSET) > MAX_CMDBUF_SZ) {
		RTW_INFO("%s: total buffer size(%d) > MAX_CMDBUF_SZ(%d)\n"
			, __func__, size + TXDESC_OFFSET, MAX_CMDBUF_SZ);
		return _FALSE;
	}

	desclen = HALMAC_TX_DESC_SIZE_8821C;

	buf = rtw_zmalloc(desclen + size);

	if (!buf) {
		RTW_INFO("%s: rtw_zmalloc for rsvd Fail\n", __func__);
		return _FALSE;
	}

	_rtw_memcpy(buf + desclen, pBuf, size);

	SET_TX_DESC_TXPKTSIZE_8821C(buf, size);
	SET_TX_DESC_OFFSET_8821C(buf, desclen);
	SET_TX_DESC_QSEL_8821C(buf, qsel);

	api->halmac_fill_txdesc_checksum(halmac, buf);

	ret = pci_write_port_not_xmitframe(d, size, buf);

	if (ret == _SUCCESS)
		ret = _TRUE;
	else
		ret = _FALSE;

	rtw_mfree(buf, desclen + size);

	return _TRUE;
}

static u8 pci_write_data_rsvd_page_xmitframe(void *d, u8 *pBuf, u32 size)
{
	struct dvobj_priv *pobj = (struct dvobj_priv *)d;
	PADAPTER padapter = dvobj_get_primary_adapter(pobj);
	PHALMAC_ADAPTER halmac = dvobj_to_halmac((struct dvobj_priv *)d);
	struct xmit_priv        *pxmitpriv = &padapter->xmitpriv;
	struct xmit_frame       *pcmdframe = NULL;
	struct pkt_attrib       *pattrib = NULL;
	PHALMAC_API api = HALMAC_GET_API(halmac);
	u8 desclen = 0;
	u8 *txdesc = NULL;

	if (size + TXDESC_OFFSET > MAX_CMDBUF_SZ) {
		RTW_INFO("%s: total buffer size(%d) > MAX_CMDBUF_SZ(%d)\n"
			, __func__, size + TXDESC_OFFSET, MAX_CMDBUF_SZ);
		return _FALSE;
	}

	pcmdframe = rtw_alloc_cmdxmitframe(pxmitpriv);

	if (pcmdframe == NULL) {
		RTW_INFO("%s: alloc ReservedPagePacket fail!\n", __func__);
		return _FALSE;
	}

	desclen = HALMAC_TX_DESC_SIZE_8821C;
	txdesc = pcmdframe->buf_addr;

	_rtw_memcpy((txdesc + desclen), pBuf, size); /* shift desclen */

	/* update attribute */
	pattrib = &pcmdframe->attrib;
	update_mgntframe_attrib(padapter, pattrib);
	pattrib->qsel = QSLT_BEACON;
	pattrib->pktlen = size;
	pattrib->last_txcmdsz = size;

	dump_mgntframe(padapter, pcmdframe);

	return _TRUE;
}

static u8 pci_write_data_h2c_normal(void *d, u8 *pBuf, u32 size)
{
	struct dvobj_priv *pobj = (struct dvobj_priv *)d;
	PADAPTER padapter = dvobj_get_primary_adapter(pobj);
	PHALMAC_ADAPTER halmac = dvobj_to_halmac((struct dvobj_priv *)d);
	struct xmit_priv        *pxmitpriv = &padapter->xmitpriv;
	struct xmit_frame       *pcmdframe = NULL;
	struct pkt_attrib       *pattrib = NULL;
	PHALMAC_API api;
	u32 desclen;
	u8 *buf;

	if (size + TXDESC_OFFSET > MAX_XMIT_EXTBUF_SZ) {
		RTW_INFO("%s: total buffer size(%d) > MAX_XMIT_EXTBUF_SZ(%d)\n"
			, __func__, size + TXDESC_OFFSET, MAX_XMIT_EXTBUF_SZ);
		return _FALSE;
	}

	pcmdframe = alloc_mgtxmitframe(pxmitpriv);

	if (pcmdframe == NULL) {
		RTW_INFO("%s: alloc ReservedPagePacket fail!\n", __func__);
		return _FALSE;
	}

	api = HALMAC_GET_API(halmac);

	desclen = HALMAC_TX_DESC_SIZE_8821C;
	buf = pcmdframe->buf_addr;
	_rtw_memcpy(buf + desclen, pBuf, size); /* shift desclen */

	SET_TX_DESC_TXPKTSIZE_8821C(buf, size);
	SET_TX_DESC_OFFSET_8821C(buf, 0);
	SET_TX_DESC_QSEL_8821C(buf, HALMAC_QUEUE_SELECT_CMD);
	SET_TX_DESC_TXDESC_CHECKSUM_8821C(buf, 0);
	api->halmac_fill_txdesc_checksum(halmac, buf);

	/* update attribute */
	pattrib = &pcmdframe->attrib;
	update_mgntframe_attrib(padapter, pattrib);
	pattrib->qsel = QSLT_CMD;
	pattrib->pktlen = size;
	pattrib->last_txcmdsz = size;

	/* fill tx desc in dump_mgntframe */
	dump_mgntframe(padapter, pcmdframe);

	return _TRUE;
}

static u8 pci_write_data_rsvd_page(void *d, u8 *pBuf, u32 size)
{
	struct dvobj_priv *pobj = (struct dvobj_priv *)d;
	PADAPTER padapter = dvobj_get_primary_adapter(pobj);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	if (pHalData->not_xmitframe_fw_dl)
		return pci_write_data_not_xmitframe(d, pBuf, size, HALMAC_TXDESC_QSEL_BEACON);
	else
		return pci_write_data_rsvd_page_xmitframe(d, pBuf, size);
}

static u8 pci_write_data_h2c(void *d, u8 *pBuf, u32 size)
{
	struct dvobj_priv *pobj = (struct dvobj_priv *)d;
	PADAPTER padapter = dvobj_get_primary_adapter(pobj);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	if (pHalData->not_xmitframe_fw_dl)
		return pci_write_data_not_xmitframe(d, pBuf, size, HALMAC_TXDESC_QSEL_H2C_CMD);
	else
		return pci_write_data_h2c_normal(d, pBuf, size);
}

int rtl8821ce_halmac_init_adapter(PADAPTER padapter)
{
	struct dvobj_priv *d;
	PHALMAC_PLATFORM_API api;
	int err;

	d = adapter_to_dvobj(padapter);
	api = &rtw_halmac_platform_api;
	api->SEND_RSVD_PAGE = pci_write_data_rsvd_page;
	api->SEND_H2C_PKT = pci_write_data_h2c;

	err = rtw_halmac_init_adapter(d, api);

	return err;
}

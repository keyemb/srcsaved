/******************************************************************************
 * Copyright(c) 2008 - 2010 Realtek Corporation. All rights reserved.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
******************************************************************************/

#ifndef __REALTEK_HAL8192CCOM_H__
#define __REALTEK_HAL8192CCOM_H__

#include "../rtl_core.h"


typedef struct _TxPowerInfo{
	u8 CCKIndex[RF90_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT40_1SIndex[RF90_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT40_2SIndexDiff[RF90_PATH_MAX][CHANNEL_GROUP_MAX];
	char HT20IndexDiff[RF90_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 OFDMIndexDiff[RF90_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT40MaxOffset[RF90_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT20MaxOffset[RF90_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 TSSI_A;
	u8 TSSI_B;
}TxPowerInfo, *PTxPowerInfo;

void
ReadTxPowerInfo(
	struct net_device 	*dev,
	u8*				PROMContent,
	bool				AutoLoadFail
	);



#define CHIP_92C  					BIT(0)
#define CHIP_92C_1T2R				BIT(1)
#define CHIP_8723					BIT(2) 
#define CHIP_8723_DRV_REV			BIT(3) 
#define NORMAL_CHIP  				BIT(4)
#define CHIP_VENDOR_UMC				BIT(5)
#define CHIP_VENDOR_UMC_B_CUT		BIT(6) 

#define IS_NORMAL_CHIP(version)  		(((version) & NORMAL_CHIP) ? true : false) 
#define IS_92C_SERIAL(version)   		(((version) & CHIP_92C) ? true : false)
#define IS_8723_SERIES(version)   		(((version) & CHIP_8723) ? true : false)
#define IS_92C_1T2R(version)			(((version) & CHIP_92C) && ((version) & CHIP_92C_1T2R))
#define IS_VENDOR_UMC(version)			(((version) & CHIP_VENDOR_UMC) ? true : false)
#define IS_VENDOR_UMC_A_CUT(version)	(((version) & CHIP_VENDOR_UMC) ? (((version) & (BIT6|BIT7)) ? false : true) : false)
#define IS_VENDOR_8723_A_CUT(version)	(((version) & CHIP_VENDOR_UMC) ? (((version) & (BIT6)) ? false : true) : false)


/*
|    BIT 7   |     BIT6     |                BIT 5                | BIT 4              |       BIT 3     |  BIT 2   |  BIT 1   |   BIT 0    |
+--------+---------+---------------------- +------------ +----------- +------ +-----------------+
|Reserved | UMC BCut |Manufacturer(TSMC/UMC)  | TEST/NORMAL | 8723 Version | 8723?   | 1T2R?  | 88C/92C |
*/
typedef enum _VERSION_8192C{
	VERSION_TEST_CHIP_88C = 0x00,
	VERSION_TEST_CHIP_92C = 0x01,
	VERSION_NORMAL_TSMC_CHIP_88C = 0x10,
	VERSION_NORMAL_TSMC_CHIP_92C = 0x11,
	VERSION_NORMAL_TSMC_CHIP_92C_1T2R = 0x13,
	VERSION_NORMAL_UMC_CHIP_88C_A_CUT = 0x30,
	VERSION_NORMAL_UMC_CHIP_92C_A_CUT = 0x31,
	VERSION_NORMAL_UMC_CHIP_92C_1T2R_A_CUT = 0x33,		
	VERSION_NORMA_UMC_CHIP_8723_1T1R_A_CUT = 0x34,
	VERSION_NORMA_UMC_CHIP_8723_1T1R_B_CUT = 0x3c,
	VERSION_NORMAL_UMC_CHIP_88C_B_CUT = 0x70,
	VERSION_NORMAL_UMC_CHIP_92C_B_CUT = 0x71,
	VERSION_NORMAL_UMC_CHIP_92C_1T2R_B_CUT = 0x73,	
}VERSION_8192C,*PVERSION_8192C;

#define CHIP_BONDING_IDENTIFIER(_value)	(((_value)>>22)&0x3)
#define CHIP_BONDING_92C_1T2R	0x1
#define CHIP_BONDING_88C_USB_MCARD	0x2
#define CHIP_BONDING_88C_USB_HP	0x1

u8 GetEEPROMSize8192C(struct net_device *dev);
VERSION_8192C ReadChipVersion(struct net_device *dev);



bool
IsSwChnlInProgress8192C(struct net_device *dev);

u8
GetRFType8192C(struct net_device *dev);

void
SetTxAntenna8192C(struct net_device *dev, u8 SelectedAntenna);




enum ChannelPlan{
	CHPL_FCC	= 0,
	CHPL_IC		= 1,
	CHPL_ETSI	= 2,
	CHPL_SPAIN	= 3,
	CHPL_FRANCE	= 4,
	CHPL_MKK	= 5,
	CHPL_MKK1	= 6,
	CHPL_ISRAEL	= 7,
	CHPL_TELEC	= 8,
	CHPL_GLOBAL	= 9,
	CHPL_WORLD	= 10,
};

void
ReadChannelPlan(
	struct net_device 	*dev,
	u8*				PROMContent,
	bool				AutoLoadFail
	);


void
WKFMCAMAddOneEntry(
	struct net_device 	*dev,
	u8			Index, 
	u16			usConfig
);
void
WKFMCAMDeleteOneEntry(
		struct net_device 	*dev,
		u32 			Index
);
void SetBcnCtrlReg(struct net_device* dev, u8 SetBits, u8 ClearBits);
void rtl8192ce_BT_VAR_INIT(struct net_device* dev);
void rtl8192ce_ReadBluetoothCoexistInfoFromHWPG(
	struct net_device*	dev,
	bool				AutoLoadFail,
	u8*				hwinfo
	);
#endif


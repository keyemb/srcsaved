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

#include "r8192C_com.h"

u8 GetEEPROMSize8192C(struct net_device *dev)
{
	u8	size = 0;
	u32	curRCR;

	curRCR = read_nic_word(dev, REG_9346CR);
	size = (curRCR & BOOT_FROM_EEPROM) ? 6 : 4; 
	
	RT_TRACE(COMP_INIT, "EEPROM type is %s\n", size==4 ? "E-FUSE" : "93C46");
	return size;
}


VERSION_8192C ReadChipVersion(struct net_device *dev)
{
	u32			value32;
	VERSION_8192C	version = 0;
	u8			ChipVersion=0;
	struct r8192_priv 	*priv = rtllib_priv(dev);

	value32 = read_nic_dword(dev, REG_SYS_CFG);

	if (value32 & TRP_VAUX_EN)
	{
		version = (value32 & TYPE_ID) ?VERSION_TEST_CHIP_92C :VERSION_TEST_CHIP_88C;		
	}
	else
	{
		ChipVersion = NORMAL_CHIP;
		ChipVersion |= ((value32 & TYPE_ID) ? CHIP_92C : 0);
		ChipVersion |= ((value32 & VENDOR_ID) ? CHIP_VENDOR_UMC : 0);
		ChipVersion |= ((value32 & BT_FUNC) ? CHIP_8723: 0); 
		if(IS_VENDOR_UMC(ChipVersion))
			ChipVersion |= ((value32 & CHIP_VER_RTL_MASK) ? CHIP_VENDOR_UMC_B_CUT : 0);

		if(IS_92C_SERIAL(ChipVersion))
		{
			value32 = read_nic_dword(dev, REG_HPON_FSM);
			ChipVersion |= ((CHIP_BONDING_IDENTIFIER(value32) == CHIP_BONDING_92C_1T2R) ? CHIP_92C_1T2R : 0);			
		}
		else if(IS_8723_SERIES(ChipVersion))
		{
			value32 = read_nic_dword(dev, REG_GPIO_OUTSTS);
			ChipVersion |= ((value32 & RF_RL_ID) ? CHIP_8723_DRV_REV : 0);			
		}
		version = (VERSION_8192C)ChipVersion;
	}

	if(IS_8723_SERIES(ChipVersion))
	{
		priv->MultiFunc = RT_MULTI_FUNC_NONE;
		value32 = read_nic_dword(dev, REG_MULTI_FUNC_CTRL);
		priv->MultiFunc |=((value32 & WL_FUNC_EN) ?  RT_MULTI_FUNC_WIFI : 0);
		priv->MultiFunc |=((value32 & BT_FUNC_EN) ?  RT_MULTI_FUNC_BT : 0);
		priv->MultiFunc |=((value32 & GPS_FUNC_EN) ?  RT_MULTI_FUNC_GPS : 0);
		priv->PolarityCtl = ((value32 & WL_HWPDN_SL) ?  RT_POLARITY_HIGH_ACT : RT_POLARITY_LOW_ACT);
		RT_TRACE(COMP_INIT, "ReadChipVersion(): MultiFunc(%x), PolarityCtl(%x) \n", priv->MultiFunc, priv->PolarityCtl);		
	}

	priv->card_8192_version = version;

	if(IS_92C_1T2R(version))
		priv->rf_type = RF_1T2R;
	else if(IS_92C_SERIAL(version))
		priv->rf_type = RF_2T2R;
	else
		priv->rf_type = RF_1T1R;

	RT_TRACE(COMP_INIT, "RF_Type is %x!!\n", priv->rf_type);

	return version;

	printk("Chip RF Type: %s\n", (priv->rf_type == RF_2T2R)?"RF_2T2R":
		((priv->rf_type == RF_1T2R)? "RF_1T2R":"RF_1T1R"));

	return version;
}

static RT_STATUS _LLTWrite(
	struct net_device 	*dev,
	u32				address,
	u32				data
	)
{
	RT_STATUS	status = RT_STATUS_SUCCESS;
	long 			count = 0;
	u32 			value = _LLT_INIT_ADDR(address) | _LLT_INIT_DATA(data) | _LLT_OP(_LLT_WRITE_ACCESS);

	write_nic_dword(dev, REG_LLT_INIT, value);
	
	do{
		
		value = read_nic_dword(dev, REG_LLT_INIT);
		if(_LLT_NO_ACTIVE == _LLT_OP_VALUE(value)){
			break;
		}
		
		if(count > POLLING_LLT_THRESHOLD){
			RT_TRACE(COMP_INIT,"Failed to polling write LLT done at address %d!\n", address);
			status = RT_STATUS_FAILURE;
			break;
		}
	}while(count++);

	return status;
	
}


u8 _LLTRead(struct net_device *dev, u32	address)
{
	long		count = 0;
	u32		value = _LLT_INIT_ADDR(address) | _LLT_OP(_LLT_READ_ACCESS);

	write_nic_dword(dev, REG_LLT_INIT, value);

	do{
		
		value = read_nic_dword(dev, REG_LLT_INIT);
		if(_LLT_NO_ACTIVE == _LLT_OP_VALUE(value)){
			return (u8)value;
		}
		
		if(count > POLLING_LLT_THRESHOLD){
			RT_TRACE(COMP_INIT,"Failed to polling read LLT done at address %d!\n", address);
			break;
		}
	}while(count++);

	return 0xFF;

}


RT_STATUS InitLLTTable(struct net_device *dev, u32 boundary)
{
	RT_STATUS	status = RT_STATUS_SUCCESS;
	u32			i;

	for(i = 0 ; i < (boundary - 1) ; i++){
		status = _LLTWrite(dev, i , i + 1);
		if(RT_STATUS_SUCCESS != status){
			return status;
		}
	}

	status = _LLTWrite(dev, (boundary - 1), 0xFF); 
	if(RT_STATUS_SUCCESS != status){
		return status;
	}

	for(i = boundary ; i < LAST_ENTRY_OF_TX_PKT_BUFFER ; i++){
		status = _LLTWrite(dev, i, (i + 1)); 
		if(RT_STATUS_SUCCESS != status){
			return status;
		}
	}
	
	status = _LLTWrite(dev, LAST_ENTRY_OF_TX_PKT_BUFFER, boundary);
	if(RT_STATUS_SUCCESS != status){
		return status;
	}

	return status;
	
}

bool IsSwChnlInProgress8192C(struct net_device *dev)
{
	struct r8192_priv 	*priv = rtllib_priv(dev);
	return priv->SwChnlInProgress;
}

u8 GetRFType8192C(struct net_device *dev)
{
	struct r8192_priv 	*priv = rtllib_priv(dev);
	return (priv->rf_type);
}


void SetTxAntenna8192C(struct net_device *dev, u8	 SelectedAntenna)
{
	
	if(IS_HARDWARE_TYPE_8192C(dev) || 
		IS_HARDWARE_TYPE_8723(dev)){
		SetAntennaConfig92C( dev, SelectedAntenna);
	}
}


u16
_HalMapChannelPlan8192C(
	struct net_device *dev,
	u16		HalChannelPlan
	)
{
	u16	rtChannelDomain;

	switch(HalChannelPlan)
	{
		case EEPROM_CHANNEL_PLAN_GLOBAL_DOMAIN:
			rtChannelDomain = COUNTRY_CODE_GLOBAL_DOMAIN;
			break;
		case EEPROM_CHANNEL_PLAN_WORLD_WIDE_13:
			rtChannelDomain = COUNTRY_CODE_WORLD_WIDE_13;
			break;			
		default:
			rtChannelDomain = (u16)HalChannelPlan;
			break;
	}
	
	return 	rtChannelDomain;

}


void
ReadChannelPlan(
	struct net_device 	*dev,
	u8*				PROMContent,
	bool				AutoLoadFail
	)
{
	struct r8192_priv 	*priv = rtllib_priv(dev);
	u8			channelPlan;

	if(AutoLoadFail){
		channelPlan = CHPL_FCC;
	}
	else{
		 channelPlan = PROMContent[EEPROM_CHANNEL_PLAN];
	}

	if((priv->RegChannelPlan >= COUNTRY_CODE_MAX) || (channelPlan & EEPROM_CHANNEL_PLAN_BY_HW_MASK)){
		priv->ChannelPlan = _HalMapChannelPlan8192C(dev, (channelPlan & (~(EEPROM_CHANNEL_PLAN_BY_HW_MASK))));
		priv->bChnlPlanFromHW = (channelPlan & EEPROM_CHANNEL_PLAN_BY_HW_MASK) ? true : false; 
	}
	else{
		priv->ChannelPlan = (u16)priv->RegChannelPlan;
	}
#ifdef ENABLE_DOT11D
	switch(priv->ChannelPlan)
	{
		case COUNTRY_CODE_GLOBAL_DOMAIN:
		{
			PRT_DOT11D_INFO	pDot11dInfo = GET_DOT11D_INFO(priv->rtllib);
	
			pDot11dInfo->bEnabled = true;
		}
		RT_TRACE(COMP_INIT, "Enable dot11d when RT_CHANNEL_DOMAIN_GLOBAL_DOAMIN!\n");
		break;
	}
#endif
	RT_TRACE(COMP_INIT, "RegChannelPlan(%d) EEPROMChannelPlan(%d)", priv->RegChannelPlan, (u32)channelPlan);
	RT_TRACE(COMP_INIT, "ChannelPlan = %d\n" , priv->ChannelPlan);

}




void
_ReadPowerValueFromPROM(
	PTxPowerInfo	pwrInfo,
	u8*			PROMContent,
	bool			AutoLoadFail
	)
{
	u32 rfPath, eeAddr, group;

	memset(pwrInfo, 0, sizeof(TxPowerInfo));

	if(AutoLoadFail){		
		for(group = 0 ; group < CHANNEL_GROUP_MAX ; group++){
			for(rfPath = 0 ; rfPath < RF90_PATH_MAX ; rfPath++){
				pwrInfo->CCKIndex[rfPath][group]		= EEPROM_Default_TxPowerLevel;	
				pwrInfo->HT40_1SIndex[rfPath][group]	= EEPROM_Default_TxPowerLevel;
				pwrInfo->HT40_2SIndexDiff[rfPath][group]	= EEPROM_Default_HT40_2SDiff;
				pwrInfo->HT20IndexDiff[rfPath][group]		= EEPROM_Default_HT20_Diff;
				pwrInfo->OFDMIndexDiff[rfPath][group]	= EEPROM_Default_LegacyHTTxPowerDiff;
				pwrInfo->HT40MaxOffset[rfPath][group]	= EEPROM_Default_HT40_PwrMaxOffset;		
				pwrInfo->HT20MaxOffset[rfPath][group]	= EEPROM_Default_HT20_PwrMaxOffset;
			}
		}

		pwrInfo->TSSI_A = EEPROM_Default_TSSI;
		pwrInfo->TSSI_B = EEPROM_Default_TSSI;
		
		return;
	}
	
	for(rfPath = 0 ; rfPath < RF90_PATH_MAX ; rfPath++){
		for(group = 0 ; group < CHANNEL_GROUP_MAX ; group++){
			eeAddr = EEPROM_CCK_TX_PWR_INX + (rfPath * 3) + group;
			pwrInfo->CCKIndex[rfPath][group] = PROMContent[eeAddr];

			eeAddr = EEPROM_HT40_1S_TX_PWR_INX + (rfPath * 3) + group;
			pwrInfo->HT40_1SIndex[rfPath][group] = PROMContent[eeAddr];
		}
	}

	for(group = 0 ; group < CHANNEL_GROUP_MAX ; group++){
		for(rfPath = 0 ; rfPath < RF90_PATH_MAX ; rfPath++){
			pwrInfo->HT40_2SIndexDiff[rfPath][group] = 
			(PROMContent[EEPROM_HT40_2S_TX_PWR_INX_DIFF + group] >> (rfPath * 4)) & 0xF;

			pwrInfo->HT20IndexDiff[rfPath][group] =
			(PROMContent[EEPROM_HT20_TX_PWR_INX_DIFF + group] >> (rfPath * 4)) & 0xF;
			
			if(pwrInfo->HT20IndexDiff[rfPath][group] & BIT3)	
				pwrInfo->HT20IndexDiff[rfPath][group] |= 0xF0;

			pwrInfo->OFDMIndexDiff[rfPath][group] =
			(PROMContent[EEPROM_OFDM_TX_PWR_INX_DIFF+ group] >> (rfPath * 4)) & 0xF;

			pwrInfo->HT40MaxOffset[rfPath][group] =
			(PROMContent[EEPROM_HT40_MAX_PWR_OFFSET+ group] >> (rfPath * 4)) & 0xF;

			pwrInfo->HT20MaxOffset[rfPath][group] =
			(PROMContent[EEPROM_HT20_MAX_PWR_OFFSET+ group] >> (rfPath * 4)) & 0xF;
		}
	}

	pwrInfo->TSSI_A = PROMContent[EEPROM_TSSI_A];
	pwrInfo->TSSI_B = PROMContent[EEPROM_TSSI_B];

}


u32 _GetChannelGroup(u32	channel)
{

	if(channel < 3){ 	
		return 0;
	}
	else if(channel < 9){ 
		return 1;
	}

	return 2;				
}


void rtl8192ce_BT_VAR_INIT(struct net_device* dev)
{
	struct r8192_priv 	*priv = rtllib_priv(dev);

	priv->bt_coexist.BluetoothCoexist = priv->EEPROMBluetoothCoexist;
	priv->bt_coexist.BT_Ant_Num = priv->EEPROMBluetoothAntNum;
	priv->bt_coexist.BT_CoexistType = priv->EEPROMBluetoothType;

	if(priv->bRegBT_Iso==2)
		priv->bt_coexist.BT_Ant_isolation = priv->EEPROMBluetoothAntIsolation;
	else
		priv->bt_coexist.BT_Ant_isolation = priv->bRegBT_Iso;
	
	priv->bt_coexist.BT_RadioSharedType = priv->EEPROMBluetoothRadioShared;

	RTPRINT(FBT, BT_TRACE,("BT Coexistance = 0x%x\n", priv->bt_coexist.BluetoothCoexist));
	if(priv->bt_coexist.BluetoothCoexist)
	{
		if(priv->bt_coexist.BT_Ant_Num == Ant_x2)
		{
			RTPRINT(FBT, BT_TRACE,("BlueTooth BT_Ant_Num = Antx2\n"));
		}
		else if(priv->bt_coexist.BT_Ant_Num == Ant_x1)
		{
			RTPRINT(FBT, BT_TRACE,("BlueTooth BT_Ant_Num = Antx1\n"));
		}
		switch(priv->bt_coexist.BT_CoexistType)
		{
			case BT_2Wire:
				RTPRINT(FBT, BT_TRACE,("BlueTooth BT_CoexistType = BT_2Wire\n"));
				break;
			case BT_ISSC_3Wire:
				RTPRINT(FBT, BT_TRACE,("BlueTooth BT_CoexistType = BT_ISSC_3Wire\n"));
				break;
			case BT_Accel:
				RTPRINT(FBT, BT_TRACE,("BlueTooth BT_CoexistType = BT_Accel\n"));
				break;
			case BT_CSR_BC4:
				RTPRINT(FBT, BT_TRACE,("BlueTooth BT_CoexistType = BT_CSR_BC4\n"));
				break;
			case BT_RTL8756:
				RTPRINT(FBT, BT_TRACE,("BlueTooth BT_CoexistType = BT_RTL8756\n"));
				break;
			default:
				RTPRINT(FBT, BT_TRACE,("BlueTooth BT_CoexistType = Unknown\n"));
				break;
		}
		RTPRINT(FBT, BT_TRACE,("BlueTooth BT_Ant_isolation = %d\n", priv->bt_coexist.BT_Ant_isolation));

		if(priv->bRegBT_Sco==1)
			priv->bt_coexist.BT_Service = BT_OtherAction;
		else if(priv->bRegBT_Sco==2)
			priv->bt_coexist.BT_Service = BT_SCO;
		else if(priv->bRegBT_Sco==4)
			priv->bt_coexist.BT_Service = BT_Busy;
		else if(priv->bRegBT_Sco==5)
			priv->bt_coexist.BT_Service = BT_OtherBusy;		
		else
			priv->bt_coexist.BT_Service = BT_Idle;

		priv->bt_coexist.BtEdcaUL = 0;
		priv->bt_coexist.BtEdcaDL = 0;
		priv->bt_coexist.BtRssiState = 0xff;
		
		priv->rf_type = RF_1T1R;
		RTPRINT(FBT, BT_TRACE, ("BT temp set RF = 1T1R\n"));
		
		priv->rf_type = RF_1T1R;
		RTPRINT(FBT, BT_TRACE, ("BT temp set RF = 1T1R\n"));
			
		RTPRINT(FBT, BT_TRACE,("BT Service = BT_Idle\n"));
		RTPRINT(FBT, BT_TRACE,("BT_RadioSharedType = 0x%x\n", priv->bt_coexist.BT_RadioSharedType));
	}
}

void rtl8192ce_ReadBluetoothCoexistInfoFromHWPG(
	struct net_device*	dev,
	bool				AutoLoadFail,
	u8*				hwinfo
	)
{
	struct r8192_priv 	*priv = rtllib_priv(dev);
	u8			tempval;

#if 0
	pHalData->EEPROMBluetoothCoexist = 1;
	pHalData->EEPROMBluetoothType = BT_CSR_BC8;
	pHalData->EEPROMBluetoothAntNum = Ant_x2;
	pHalData->EEPROMBluetoothAntIsolation = 1;
	pHalData->EEPROMBluetoothRadioShared = BT_Radio_Shared;
#else
	if(!AutoLoadFail)
	{
		priv->EEPROMBluetoothCoexist = ((hwinfo[RF_OPTION1]&0xe0)>>5);	
		tempval = hwinfo[RF_OPTION4];
		priv->EEPROMBluetoothType = ((tempval&0xe)>>1);				
		priv->EEPROMBluetoothAntNum = (tempval&0x1);					
		priv->EEPROMBluetoothAntIsolation = ((tempval&0x10)>>4);			
		priv->EEPROMBluetoothRadioShared = ((tempval&0x20)>>5);			
	}
	else
	{
		priv->EEPROMBluetoothCoexist = 0;
		priv->EEPROMBluetoothType = BT_2Wire;
		priv->EEPROMBluetoothAntNum = Ant_x2;
		priv->EEPROMBluetoothAntIsolation = 0;
		priv->EEPROMBluetoothRadioShared = BT_Radio_Shared;
	}
#endif

	printk("=========>%s(): EEPROMBluetoothCoexist:%d, EEPROMBluetoothType:%d\n", __func__, priv->EEPROMBluetoothCoexist,priv->EEPROMBluetoothType);

	rtl8192ce_BT_VAR_INIT(dev);
}


void
ReadTxPowerInfo(
	struct net_device 	*dev,
	u8*				PROMContent,
	bool				AutoLoadFail
	)
{	
	struct r8192_priv 	*priv = rtllib_priv(dev);
	TxPowerInfo		pwrInfo;
	u32			rfPath, ch, group;
	u8			pwr, diff;

	_ReadPowerValueFromPROM(&pwrInfo, PROMContent, AutoLoadFail);

	if(!AutoLoadFail)
		priv->bTXPowerDataReadFromEEPORM = true;
	
	for(rfPath = 0 ; rfPath < RF90_PATH_MAX ; rfPath++){
		for(ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++){
			group = _GetChannelGroup(ch);

			priv->TxPwrLevelCck[rfPath][ch]		= pwrInfo.CCKIndex[rfPath][group];
			priv->TxPwrLevelHT40_1S[rfPath][ch]	= pwrInfo.HT40_1SIndex[rfPath][group];

			priv->TxPwrHt20Diff[rfPath][ch]		= pwrInfo.HT20IndexDiff[rfPath][group];
			priv->TxPwrLegacyHtDiff[rfPath][ch]	= pwrInfo.OFDMIndexDiff[rfPath][group];
			priv->PwrGroupHT20[rfPath][ch]		= pwrInfo.HT20MaxOffset[rfPath][group];
			priv->PwrGroupHT40[rfPath][ch]		= pwrInfo.HT40MaxOffset[rfPath][group];

			pwr		= pwrInfo.HT40_1SIndex[rfPath][group];
			diff		= pwrInfo.HT40_2SIndexDiff[rfPath][group];

			priv->TxPwrLevelHT40_2S[rfPath][ch]  	= (pwr > diff) ? (pwr - diff) : 0;
		}
	}

#ifdef DBG

	for(rfPath = 0 ; rfPath < RF90_PATH_MAX ; rfPath++){
		for(ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++){
			RTPRINT(FINIT, INIT_TxPower, 
				("RF(%d)-Ch(%d) [CCK / HT40_1S / HT40_2S] = [0x%x / 0x%x / 0x%x]\n", 
				rfPath, ch, priv->TxPwrLevelCck[rfPath][ch], 
				priv->TxPwrLevelHT40_1S[rfPath][ch], 
				priv->TxPwrLevelHT40_2S[rfPath][ch]));

		}
	}

	for(ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++){
		RTPRINT(FINIT, INIT_TxPower, ("RF-A Ht20 to HT40 Diff[%d] = 0x%x\n", ch, priv->TxPwrHt20Diff[RF90_PATH_A][ch]));
	}

	for(ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++){
		RTPRINT(FINIT, INIT_TxPower, ("RF-A Legacy to Ht40 Diff[%d] = 0x%x\n", ch, priv->TxPwrLegacyHtDiff[RF90_PATH_A][ch]));
	}
	
	for(ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++){
		RTPRINT(FINIT, INIT_TxPower, ("RF-B Ht20 to HT40 Diff[%d] = 0x%x\n", ch, priv->TxPwrHt20Diff[RF90_PATH_B][ch]));
	}
	
	for(ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++){
		RTPRINT(FINIT, INIT_TxPower, ("RF-B Legacy to HT40 Diff[%d] = 0x%x\n", ch, priv->TxPwrLegacyHtDiff[RF90_PATH_B][ch]));
	}
	
#endif
	
}

void
WKFMCAMAddOneEntry(
	struct net_device 	*dev,
	u8			Index, 
	u16			usConfig
)
{
	struct r8192_priv 	*priv = rtllib_priv(dev);
	PRT_POWER_SAVE_CONTROL	pPSC = GET_POWER_SAVE_CONTROL(priv);
	PRT_PM_WOL_PATTERN_INFO	pWoLPatternInfo = &(pPSC->PmWoLPatternInfo[0]);

	u32 CamCmd = 0;
	u32 CamContent = 0;
	u8 Addr_i=0;

	RT_TRACE( COMP_CMD, "===>WKFMCAMAddOneEntry(): usConfig=0x%02X\n", usConfig );
	
	RT_TRACE(COMP_CMD, "The mask index is %d\n", Index);

	for(Addr_i=0; Addr_i<CAM_CONTENT_COUNT; Addr_i++)
	{
		CamCmd= Addr_i+CAM_CONTENT_COUNT*Index;
		CamCmd= CamCmd |BIT31|BIT16;

		if(Addr_i < 4) 
		{
			CamContent = pWoLPatternInfo[Index].Mask[Addr_i];

			write_nic_dword(dev, REG_WKFMCAM_RWD, CamContent);  
			RT_TRACE(COMP_CMD, "WKFMCAMAddOneEntry(): WRITE %x: %x \n", REG_WKFMCAM_RWD, CamContent);
			write_nic_dword(dev, REG_WKFMCAM_CMD, CamCmd);  
			RT_TRACE(COMP_CMD, "WKFMCAMAddOneEntry(): WRITE %x: %x \n", REG_WKFMCAM_CMD, CamCmd);
		}
		else if(Addr_i == 4)
		{
			CamContent = BIT31 | pWoLPatternInfo[Index].CrcRemainder;

			write_nic_dword(dev, REG_WKFMCAM_RWD, CamContent);  
			RT_TRACE(COMP_CMD, "WKFMCAMAddOneEntry(): WRITE %x: %x \n", REG_WKFMCAM_RWD, CamContent);
			write_nic_dword(dev, REG_WKFMCAM_CMD, CamCmd);  
			RT_TRACE(COMP_CMD, "WKFMCAMAddOneEntry(): WRITE %x: %x \n", REG_WKFMCAM_CMD, CamCmd);
		}

	}
	
}

void
WKFMCAMDeleteOneEntry(
		struct net_device 	*dev,
		u32 			Index
)
{
	u32	Content = 0;
	u32	Command = 0;
	u8	i;
												
	for(i=0; i<8; i++)
	{
		Command = Index*CAM_CONTENT_COUNT + i;
		Command= Command | BIT31|BIT16;
	
		write_nic_dword(dev, REG_WKFMCAM_RWD, Content);
		RT_TRACE(COMP_CMD, "WKFMCAMDeleteOneEntry(): WRITE %x: %x \n", REG_WKFMCAM_RWD, Content);
		write_nic_dword(dev, REG_WKFMCAM_CMD, Command);
		RT_TRACE(COMP_CMD, "WKFMCAMDeleteOneEntry(): WRITE %x: %x \n", REG_WKFMCAM_CMD, Command);			
	}
}

void
SetBcnCtrlReg(
	struct net_device* dev,
	u8		SetBits,
	u8		ClearBits
	)
{
	struct r8192_priv 		*priv = rtllib_priv(dev);

	priv->RegBcnCtrlVal |= SetBits;
	priv->RegBcnCtrlVal &= ~ClearBits;

	write_nic_byte(dev, REG_BCN_CTRL, (u8)priv->RegBcnCtrlVal);
}

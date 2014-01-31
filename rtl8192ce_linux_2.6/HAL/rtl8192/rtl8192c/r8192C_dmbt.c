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
#include "../rtl_core.h"

bool
BT_BTStateChange(struct net_device *dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	
	u32 			Polling, Ratio_Tx, Ratio_PRI;
	u32 			BT_Tx, BT_PRI;
	u8			BT_State;
	static u8		ServiceTypeCnt = 0;
	u8			CurServiceType;

	if(priv->rtllib->state < RTLLIB_LINKED)
		return false;
	
	BT_State = read_nic_byte(dev, 0x4fd);
/*
	temp = PlatformEFIORead4Byte(Adapter, 0x488);
	BT_Tx = (u2Byte)(((temp<<8)&0xff00)+((temp>>8)&0xff));
	BT_PRI = (u2Byte)(((temp>>8)&0xff00)+((temp>>24)&0xff));

	temp = PlatformEFIORead4Byte(Adapter, 0x48c);
	Polling = ((temp<<8)&0xff000000) + ((temp>>8)&0x00ff0000) + 
			((temp<<8)&0x0000ff00) + ((temp>>8)&0x000000ff);
	
*/
	BT_Tx = read_nic_dword(dev, 0x488);
	
	RTPRINT(FBT, BT_TRACE,("Ratio 0x488  =%x\n", BT_Tx));
	BT_Tx =BT_Tx & 0x00ffffff;

	BT_PRI = read_nic_dword(dev, 0x48c);
	
	RTPRINT(FBT, BT_TRACE,("Ratio Ratio 0x48c  =%x\n", BT_PRI));
	BT_PRI =BT_PRI & 0x00ffffff;


	Polling = read_nic_dword(dev, 0x490);


	if(BT_Tx==0xffffffff && BT_PRI==0xffffffff && Polling==0xffffffff && BT_State==0xff)
		return false;

	BT_State &= BIT0;

	if(BT_State != priv->bt_coexist.BT_CUR_State)
	{
		priv->bt_coexist.BT_CUR_State = BT_State;
	
		if(priv->bRegBT_Sco == 3)
		{
			ServiceTypeCnt = 0;
		
			priv->bt_coexist.BT_Service = BT_Idle;

			RTPRINT(FBT, BT_TRACE,("BT_%s\n", BT_State?"ON":"OFF"));

			BT_State = BT_State | 
					((priv->bt_coexist.BT_Ant_isolation==1)?0:BIT1) |BIT2;

			write_nic_byte(dev, 0x4fd, BT_State);
			RTPRINT(FBT, BT_TRACE,("BT set 0x4fd to %x\n", BT_State));
		}
		
		return true;
	}
	RTPRINT(FBT, BT_TRACE,("bRegBT_Sco   %d\n", priv->bRegBT_Sco));

	Ratio_Tx = BT_Tx*1000/Polling;
	Ratio_PRI = BT_PRI*1000/Polling;

	priv->bt_coexist.Ratio_Tx=Ratio_Tx;
	priv->bt_coexist.Ratio_PRI=Ratio_PRI;
	
	RTPRINT(FBT, BT_TRACE,("Ratio_Tx=%d\n", Ratio_Tx));
	RTPRINT(FBT, BT_TRACE,("Ratio_PRI=%d\n", Ratio_PRI));

	
	if(BT_State && priv->bRegBT_Sco==3)
	{
		RTPRINT(FBT, BT_TRACE,("bRegBT_Sco  ==3 Follow Counter\n"));
		{
		/*
			Ratio_Tx = BT_Tx*1000/Polling;
			Ratio_PRI = BT_PRI*1000/Polling;

			pHalData->bt_coexist.Ratio_Tx=Ratio_Tx;
			pHalData->bt_coexist.Ratio_PRI=Ratio_PRI;
			
			RTPRINT(FBT, BT_TRACE, ("Ratio_Tx=%d\n", Ratio_Tx));
			RTPRINT(FBT, BT_TRACE, ("Ratio_PRI=%d\n", Ratio_PRI));

		*/	
			if((Ratio_Tx < 30)  && (Ratio_PRI < 30)) 
			  	CurServiceType = BT_Idle;
			else if((Ratio_PRI > 110) && (Ratio_PRI < 250))
				CurServiceType = BT_SCO;
			else if((Ratio_Tx >= 200)&&(Ratio_PRI >= 200))
				CurServiceType = BT_Busy;
			else if((Ratio_Tx >=350) && (Ratio_Tx < 500))
				CurServiceType = BT_OtherBusy;
			else if(Ratio_Tx >=500)
				CurServiceType = BT_PAN;
			else
				CurServiceType=BT_OtherAction;
		}

/*		if(pHalData->bt_coexist.bStopCount)
		{
			ServiceTypeCnt=0;
			pHalData->bt_coexist.bStopCount=FALSE;
		}
*/
#if 0
		{
			ServiceTypeCnt=2;
			LastServiceType=CurServiceType;
		}

		else if(CurServiceType == LastServiceType)
		{
			if(ServiceTypeCnt<3)
				ServiceTypeCnt++;
		}
		else
		{
			ServiceTypeCnt = 0;
			LastServiceType = CurServiceType;
		}

		if(ServiceTypeCnt==2)
#else
		if(CurServiceType != priv->bt_coexist.BT_Service)
#endif
		{
			priv->bt_coexist.BT_Service = CurServiceType;
			BT_State = BT_State | 
					((priv->bt_coexist.BT_Ant_isolation==1)?0:BIT1) |
					((priv->bt_coexist.BT_Service!=BT_Idle)?0:BIT2);


			if(priv->bt_coexist.BT_Service==BT_SCO)
			{
				RTPRINT(FBT, BT_TRACE,("BT TYPE Set to  ==> BT_SCO\n"));
			}
			else if(priv->bt_coexist.BT_Service==BT_Idle)
			{
				RTPRINT(FBT, BT_TRACE,("BT TYPE Set to  ==> BT_Idle\n"));
			}
			else if(priv->bt_coexist.BT_Service==BT_OtherAction)
			{
				RTPRINT(FBT, BT_TRACE,("BT TYPE Set to  ==> BT_OtherAction\n"));
			}
			else if(priv->bt_coexist.BT_Service==BT_Busy)
			{
				RTPRINT(FBT, BT_TRACE,("BT TYPE Set to  ==> BT_Busy\n"));
			}
			else if(priv->bt_coexist.BT_Service==BT_PAN)
			{
				RTPRINT(FBT, BT_TRACE, ("BT TYPE Set to  ==> BT_PAN\n"));
			}
			else
			{
				RTPRINT(FBT, BT_TRACE,("BT TYPE Set to ==> BT_OtherBusy\n"));
			}
				
			if(priv->bt_coexist.BT_Service!=BT_Idle)
			{
			
				write_nic_word(dev, 0x504, 0x0ccc);
				write_nic_byte(dev, 0x506, 0x54);
				write_nic_byte(dev, 0x507, 0x54);
			
			}
			else
			{
				write_nic_byte(dev, 0x506, 0x00);
				write_nic_byte(dev, 0x507, 0x00);			
			}
				
			write_nic_byte(dev, 0x4fd, BT_State);
			RTPRINT(FBT, BT_TRACE,("BT_SCO set 0x4fd to %x\n", BT_State));
			return true;
		}
	}

	return false;

}

bool
BT_WifiConnectChange(struct net_device *dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	static bool	bMediaConnect = false;

	if((priv->rtllib->state < RTLLIB_LINKED) || (!priv->rtllib->is_roaming))
	{
		bMediaConnect = false;
	}
	else
	{
		if(!bMediaConnect)
		{
			bMediaConnect = true;
			return true;
		}
		bMediaConnect = true;
	}

	return false;
}

#define BT_RSSI_STATE_NORMAL_POWER	BIT0
#define BT_RSSI_STATE_AMDPU_OFF		BIT1
#define BT_RSSI_STATE_SPECIAL_LOW		BIT2
#define BT_RSSI_STATE_BG_EDCA_LOW		BIT3
#define BT_RSSI_STATE_TXPOWER_LOW		BIT4

#define GET_UNDECORATED_AVERAGE_RSSI(_priv)	\
		(((struct r8192_priv *)(_priv))->rtllib->iw_mode == IW_MODE_ADHOC)?		\
			(((struct r8192_priv *)(_priv))->EntryMinUndecoratedSmoothedPWDB):	\
			(((struct r8192_priv *)(_priv))->undecorated_smoothed_pwdb)
			
u8
BT_RssiStateChange(
	struct net_device *dev
	)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	long			UndecoratedSmoothedPWDB;
	u8			CurrBtRssiState = 0x00;

	if(priv->rtllib->state == RTLLIB_LINKED)	
	{
		UndecoratedSmoothedPWDB = GET_UNDECORATED_AVERAGE_RSSI(priv);
	}
	else 
	{
		if(priv->EntryMinUndecoratedSmoothedPWDB == 0)
			UndecoratedSmoothedPWDB = 100;	
		else
			UndecoratedSmoothedPWDB = priv->EntryMinUndecoratedSmoothedPWDB;
	}

	if(UndecoratedSmoothedPWDB >= 67)
		CurrBtRssiState &= (~BT_RSSI_STATE_NORMAL_POWER);
	else if(UndecoratedSmoothedPWDB < 62)
		CurrBtRssiState |= BT_RSSI_STATE_NORMAL_POWER;

	if(UndecoratedSmoothedPWDB >= 40)
		CurrBtRssiState &= (~BT_RSSI_STATE_AMDPU_OFF);
	else if(UndecoratedSmoothedPWDB <= 32)
		CurrBtRssiState |= BT_RSSI_STATE_AMDPU_OFF;

	if(UndecoratedSmoothedPWDB < 35)
		CurrBtRssiState |=  BT_RSSI_STATE_SPECIAL_LOW;
	else
		CurrBtRssiState &= (~BT_RSSI_STATE_SPECIAL_LOW);

	if(UndecoratedSmoothedPWDB >= 30)
		CurrBtRssiState |=  BT_RSSI_STATE_TXPOWER_LOW;
	else if(UndecoratedSmoothedPWDB < 25)
		CurrBtRssiState &= (~BT_RSSI_STATE_TXPOWER_LOW);
	
	if(UndecoratedSmoothedPWDB < 15)
		CurrBtRssiState |=  BT_RSSI_STATE_BG_EDCA_LOW;
	else
		CurrBtRssiState &= (~BT_RSSI_STATE_BG_EDCA_LOW);
	
	if(CurrBtRssiState != priv->bt_coexist.BtRssiState)
	{
		priv->bt_coexist.BtRssiState = CurrBtRssiState;
		return true;
	}
	else
	{
		return false;
	}
}

void
dm_BTCoexist(struct net_device *dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	PRT_HIGH_THROUGHPUT	pHTInfo = priv->rtllib->pHTInfo;
	PRX_TS_RECORD	pRxTs = NULL;
	u8			BT_gpio_mux;

	bool			bWifiConnectChange, bBtStateChange,bRssiStateChange;

	if( (priv->bt_coexist.BluetoothCoexist) &&
		(priv->bt_coexist.BT_CoexistType == BT_CSR_BC4) )
	{
		bWifiConnectChange = BT_WifiConnectChange(dev);
		bBtStateChange = BT_BTStateChange(dev);
		bRssiStateChange = BT_RssiStateChange(dev);
		
		RTPRINT(FBT, BT_TRACE,("bWifiConnectChange %d, bBtStateChange  %d,LastTxPowerLvl  %x,  DynamicTxHighPowerLvl  %x\n",
			bWifiConnectChange,bBtStateChange,bRssiStateChange,priv->DynamicTxHighPowerLvl));

		BT_gpio_mux = read_nic_byte(dev, REG_GPIO_MUXCFG);		

		if( bWifiConnectChange ||bBtStateChange  ||bRssiStateChange )
		{
			if(priv->bt_coexist.BT_CUR_State)
			{
				if(priv->bBT_Ampdu)
				{
					if(priv->rtllib->pHTInfo->IOTPeer == HT_IOT_PEER_CISCO)
					{
						if(priv->bt_coexist.BT_Service!=BT_Idle)
						{
							if(pHTInfo->bAcceptAddbaReq)
							{
								RTPRINT(FBT, BT_TRACE, ("BT_Disallow AMPDU \n"));	
								pHTInfo->bAcceptAddbaReq = false;
								if(GetTs(priv->rtllib, (PTS_COMMON_INFO*)(&pRxTs), priv->rtllib->current_network.bssid, 0, RX_DIR, false))
									TsInitDelBA(priv->rtllib, (PTS_COMMON_INFO)pRxTs, RX_DIR);
							}
						}
						else
						{
							if(!pHTInfo->bAcceptAddbaReq)
							{
								RTPRINT(FBT, BT_TRACE, ("BT_Allow AMPDU  RSSI >=40\n"));	
								pHTInfo->bAcceptAddbaReq = true;
							}
						}
					}
					else
					{
						if(!pHTInfo->bAcceptAddbaReq)
						{
							RTPRINT(FBT, BT_TRACE, ("BT_Allow AMPDU BT Idle\n"));	
							pHTInfo->bAcceptAddbaReq = true;
						}
					}
				}
#if 0
				else if((priv->bt_coexist.BT_Service==BT_SCO) || (priv->bt_coexist.BT_Service==BT_Busy))
				{
					if(priv->bt_coexist.BtRssiState & BT_RSSI_STATE_AMDPU_OFF)
					{
						if(priv->bBT_Ampdu && pHTInfo->bAcceptAddbaReq)
						{
							RTPRINT(FBT, BT_TRACE,("BT_Disallow AMPDU RSSI <=32\n"));	
							pHTInfo->bAcceptAddbaReq = false;
							if(GetTs(priv->rtllib, (PTS_COMMON_INFO*)(&pRxTs), priv->rtllib->current_network.bssid, 0, RX_DIR, false))
								TsInitDelBA(priv->rtllib, (PTS_COMMON_INFO)pRxTs, RX_DIR);
						}
					}
					else
					{
						if(priv->bBT_Ampdu && pHTInfo->bAcceptAddbaReq)
						{
							RTPRINT(FBT, BT_TRACE,("BT_Allow AMPDU  RSSI >=40\n"));	
							pHTInfo->bAcceptAddbaReq = true;
						}
					}
				}
				else
				{
					if(priv->bBT_Ampdu && !pHTInfo->bAcceptAddbaReq)
					{
						RTPRINT(FBT, BT_TRACE,("BT_Allow AMPDU BT not in SCO or BUSY\n"));	
						pHTInfo->bAcceptAddbaReq = true;
					}
				}
#endif

				if(priv->bt_coexist.BT_Ant_isolation)
				{			
					RTPRINT(FBT, BT_TRACE,("BT_IsolationLow\n"));

#if 0
					RTPRINT(FBT, BT_TRACE,("BT_Update Rate table\n"));
					rtl8192ce_UpdateHalRATRTable(
								dev, 
								priv->rtllib->dot11HTOperationalRateSet,NULL);
#endif
					
					if(1)
					{
						if(priv->rtllib->pHTInfo->IOTPeer == HT_IOT_PEER_CISCO &&
							priv->bt_coexist.BT_Service==BT_OtherAction)
						{
							RTPRINT(FBT, BT_TRACE, ("BT_Turn ON Coexist\n"));
							write_nic_byte(dev, REG_GPIO_MUXCFG, 0xa0);	
						}
						else
						{
							if((priv->bt_coexist.BT_Service==BT_Busy) &&
								(priv->bt_coexist.BtRssiState & BT_RSSI_STATE_NORMAL_POWER))
							{
								RTPRINT(FBT, BT_TRACE, ("BT_Turn ON Coexist\n"));
								write_nic_byte(dev, REG_GPIO_MUXCFG, 0xa0);
							}
							else if((priv->bt_coexist.BT_Service==BT_OtherAction) &&
									(priv->rtllib->mode < WIRELESS_MODE_N_24G) &&
									(priv->bt_coexist.BtRssiState & BT_RSSI_STATE_SPECIAL_LOW))
							{
								RTPRINT(FBT, BT_TRACE,("BT_Turn ON Coexist\n"));
								write_nic_byte(dev, REG_GPIO_MUXCFG, 0xa0);
							}
							else if(priv->bt_coexist.BT_Service==BT_PAN)
							{
								RTPRINT(FBT, BT_TRACE, ("BT_Turn ON Coexist\n"));
								write_nic_byte(dev, REG_GPIO_MUXCFG, 0x00);	
							}
							else
							{
								RTPRINT(FBT, BT_TRACE,("BT_Turn OFF Coexist\n"));
								write_nic_byte(dev, REG_GPIO_MUXCFG, 0x00);
							}
						}
					}
					else
					{
						RTPRINT(FBT, BT_TRACE, ("BT: There is no Wifi traffic!! Turn off Coexist\n"));
						write_nic_byte(dev, REG_GPIO_MUXCFG, 0x00);
					}

					if(1)
					{
						if(priv->bt_coexist.BT_Service==BT_PAN)
						{
							RTPRINT(FBT, BT_TRACE, ("BT_Turn ON Coexist(Reg0x44 = 0x10100)\n"));
							write_nic_dword(dev, REG_GPIO_PIN_CTRL, 0x10100);	
						}
						else
						{
							RTPRINT(FBT, BT_TRACE, ("BT_Turn OFF Coexist(Reg0x44 = 0x0)\n"));
							write_nic_dword(dev, REG_GPIO_PIN_CTRL, 0x0);	
						}
					}
					else
					{
						RTPRINT(FBT, BT_TRACE, ("BT: There is no Wifi traffic!! Turn off Coexist(Reg0x44 = 0x0)\n"));
						write_nic_dword(dev, REG_GPIO_PIN_CTRL, 0x0);	
					}
					
					if(priv->bt_coexist.BtRssiState & BT_RSSI_STATE_NORMAL_POWER)
					{
						if(priv->bt_coexist.BT_Service==BT_OtherBusy)
						{
							priv->bt_coexist.BtEdcaUL = 0x5ea72b;
							priv->bt_coexist.BtEdcaDL = 0x5ea72b;
						}
						else if(priv->bt_coexist.BT_Service==BT_Busy)
						{
							priv->bt_coexist.BtEdcaUL = 0x5eb82f;
							priv->bt_coexist.BtEdcaDL = 0x5eb82f;
							RTPRINT(FBT, BT_TRACE, ("BT in BT_Busy state parameter(0x%x) = 0x%x\n", REG_EDCA_BE_PARAM, 0x5eb82f));		
						}
						else if(priv->bt_coexist.BT_Service==BT_SCO)
						{
							if(priv->bt_coexist.Ratio_Tx>160)
							{
								priv->bt_coexist.BtEdcaUL = 0x5ea72f;
								priv->bt_coexist.BtEdcaDL = 0x5ea72f;
							}
							else
							{
								priv->bt_coexist.BtEdcaUL = 0x5ea32b;
								priv->bt_coexist.BtEdcaDL = 0x5ea42b;
							}									
						}
						else
						{
							priv->bt_coexist.BtEdcaUL = 0;
							priv->bt_coexist.BtEdcaDL = 0;
							RTPRINT(FBT, BT_TRACE, ("BT in  State  %d  and parameter(0x%x) use original setting.\n",priv->bt_coexist.BT_Service, REG_EDCA_BE_PARAM));
						}

						if((priv->bt_coexist.BT_Service!=BT_Idle) &&
							(priv->rtllib->mode == WIRELESS_MODE_G ||(priv->rtllib->mode == (WIRELESS_MODE_G|WIRELESS_MODE_B)) ) &&
							(priv->bt_coexist.BtRssiState & BT_RSSI_STATE_BG_EDCA_LOW))
						{
							priv->bt_coexist.BtEdcaUL = 0x5eb82b;
							priv->bt_coexist.BtEdcaDL = 0x5eb82b;
							RTPRINT(FBT, BT_TRACE, ("BT set parameter(0x%x) = 0x%x\n", REG_EDCA_BE_PARAM, 0x5eb82b));		
						}
					}
					else
					{
						priv->bt_coexist.BtEdcaUL = 0;
						priv->bt_coexist.BtEdcaDL = 0;
					}

					if(priv->bt_coexist.BT_Service!=BT_Idle)
					{
						RTPRINT(FBT, BT_TRACE, ("BT Set RfReg0x1E[7:4] = 0x%x \n", 0xf));
						PHY_SetRFReg(dev, PathA, 0x1e, 0xf0, 0xf);
					}
					else
					{
						RTPRINT(FBT, BT_TRACE, ("BT Set RfReg0x1E[7:4] = 0x%x \n", priv->bt_coexist.BtRfRegOrigin1E));
						PHY_SetRFReg(dev, PathA, 0x1e, 0xf0, priv->bt_coexist.BtRfRegOrigin1E);
					}	


					if(!priv->rtllib->bdynamic_txpower_enable)
					{
						if(priv->bt_coexist.BT_Service != BT_Idle)
						{
							if(priv->bt_coexist.BtRssiState & BT_RSSI_STATE_TXPOWER_LOW)
							{
								priv->DynamicTxHighPowerLvl = TxHighPwrLevel_BT2;
								RTPRINT(FBT, BT_TRACE, ("TxHighPwrLevel(-12)\n"));
							}
							else
							{
								priv->DynamicTxHighPowerLvl = TxHighPwrLevel_BT1;
								RTPRINT(FBT, BT_TRACE, ("TxHighPwrLevel(-6)\n"));
							}
						}
						else
						{
							priv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
							RTPRINT(FBT, BT_TRACE, ("TxHighPwrLevel_Normal\n"));
						}

						PHY_SetTxPowerLevel8192C(dev, priv->chan);
					}
					
				}
				else
				{
					RTPRINT(FBT, BT_TRACE,("BT_IsolationHigh\n"));
				}
			}
			else
			{
				if(priv->bBT_Ampdu && !pHTInfo->bAcceptAddbaReq)
				{
					RTPRINT(FBT, BT_TRACE,("BT_Allow AMPDU bt is off\n"));	
					pHTInfo->bAcceptAddbaReq = true;
				}

				RTPRINT(FBT, BT_TRACE,("BT_Turn OFF Coexist bt is off \n"));
				write_nic_byte(dev, REG_GPIO_MUXCFG, 0x00);

				RTPRINT(FBT, BT_TRACE, ("BT Set RfReg0x1E[7:4] = 0x%x \n", priv->bt_coexist.BtRfRegOrigin1E));
				PHY_SetRFReg(dev, PathA, 0x1e, 0xf0, priv->bt_coexist.BtRfRegOrigin1E);

				priv->bt_coexist.BtEdcaUL = 0;
				priv->bt_coexist.BtEdcaDL = 0;

#if 0
				RTPRINT(FBT, BT_TRACE,("BT_Update Rate table\n"));
				if(pMgntInfo->bUseRAMask)
				{
					Adapter->HalFunc.UpdateHalRAMaskHandler(
							Adapter,
							FALSE,
							0,
							NULL,
							NULL,
							pMgntInfo->RateAdaptive.RATRState,
							RAMask_Normal,
							NULL);
				}
				else
				{
					Adapter->HalFunc.UpdateHalRATRTableHandler(
								Adapter, 
								&pMgntInfo->dot11OperationalRateSet,
								pMgntInfo->dot11HTOperationalRateSet,NULL);
				}
#endif
			}
		}
	}
}


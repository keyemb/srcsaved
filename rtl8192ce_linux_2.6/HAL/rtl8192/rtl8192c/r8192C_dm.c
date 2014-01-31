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


void PWDB_Monitor(struct net_device *dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);
	long			tmpEntryMaxPWDB=0, tmpEntryMinPWDB=0xff;
	u8			H2C_Parameter[3] ={0};

	return;


#ifdef BT_TODO
	if(priv->BtInfo.BtOperationOn)
	{
		for(i=0; i<MAX_BT_ASOC_ENTRY_NUM; i++)
		{
			if(priv->BtInfo.BtAsocEntry[i].bUsed)
			{
				if(priv->BtInfo.BtAsocEntry[i].UndecoratedSmoothedPWDB < tmpBTEntryMinPWDB)
					tmpBTEntryMinPWDB = priv->BtInfo.BtAsocEntry[i].UndecoratedSmoothedPWDB;
				if(priv->BtInfo.BtAsocEntry[i].UndecoratedSmoothedPWDB > tmpBTEntryMaxPWDB)
					tmpBTEntryMaxPWDB = priv->BtInfo.BtAsocEntry[i].UndecoratedSmoothedPWDB;

				H2C_Parameter[2] = (u1Byte)(pMgntInfo->BtInfo.BtAsocEntry[i].UndecoratedSmoothedPWDB & 0xFF);
				H2C_Parameter[0] = (MAX_FW_SUPPORT_MACID_NUM-1-i);
				RTPRINT(FDM, DM_BT30, ("RSSI report for BT[%d], H2C_Par = 0x%x\n", i, H2C_Parameter[0]));
				FillH2CCmd92C(dev, H2C_RSSI_REPORT, 3, H2C_Parameter);
				RTPRINT_ADDR(FDM, DM_PWDB, ("BT_Entry Mac :"), pMgntInfo->BtInfo.BtAsocEntry[i].BTRemoteMACAddr)
				RTPRINT(FDM, DM_PWDB, ("BT rx pwdb[%d] = 0x%lx(%ld)\n", i, pMgntInfo->BtInfo.BtAsocEntry[i].UndecoratedSmoothedPWDB, 
					priv->BtInfo.BtAsocEntry[i].UndecoratedSmoothedPWDB));
			}
		}
		if(tmpBTEntryMaxPWDB != 0)	
		{
			priv->BT_EntryMaxUndecoratedSmoothedPWDB = tmpBTEntryMaxPWDB;
			RTPRINT(FDM, DM_PWDB, ("BT_EntryMaxPWDB = 0x%lx(%ld)\n", 
				tmpBTEntryMaxPWDB, tmpBTEntryMaxPWDB));
		}
		else
		{
			priv->BT_EntryMaxUndecoratedSmoothedPWDB = 0;
		}
		if(tmpBTEntryMinPWDB != 0xff) 
		{
			priv->BT_EntryMinUndecoratedSmoothedPWDB = tmpBTEntryMinPWDB;
			RTPRINT(FDM, DM_PWDB, ("BT_EntryMinPWDB = 0x%lx(%ld)\n", 
				tmpBTEntryMinPWDB, tmpBTEntryMinPWDB));
		}
		else
		{
			priv->BT_EntryMinUndecoratedSmoothedPWDB = 0;
		}
	}
#endif

#if 0
	for(i = 0; i < ASSOCIATE_ENTRY_NUM; i++)
	{
		if(Adapter->MgntInfo.NdisVersion == RT_NDIS_VERSION_6_2)
		{
			if(ACTING_AS_AP(ADJUST_TO_ADAPTIVE_ADAPTER(Adapter, FALSE)))
				pEntry = AsocEntry_EnumStation(ADJUST_TO_ADAPTIVE_ADAPTER(Adapter, FALSE), i);
			else
				pEntry = AsocEntry_EnumStation(ADJUST_TO_ADAPTIVE_ADAPTER(Adapter, TRUE), i);
		}
		else
		{
			pEntry = AsocEntry_EnumStation(ADJUST_TO_ADAPTIVE_ADAPTER(Adapter, TRUE), i);	
		}

		if(pEntry!=NULL)
		{
			if(pEntry->bAssociated)
			{
				RTPRINT_ADDR(FDM, DM_PWDB, ("pEntry->MacAddr ="), pEntry->MacAddr);
				RTPRINT(FDM, DM_PWDB, ("pEntry->rssi = 0x%x(%d)\n", 
					pEntry->rssi_stat.UndecoratedSmoothedPWDB,
					pEntry->rssi_stat.UndecoratedSmoothedPWDB));
				if(pEntry->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
					tmpEntryMinPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;
				if(pEntry->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
					tmpEntryMaxPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;
				
				H2C_Parameter = pEntry->rssi_stat.UndecoratedSmoothedPWDB << 16;
				H2C_Parameter |= (pEntry->AID+1);
				FillH2CCmd92C(Adapter, H2C_RSSI_REPORT, 3, (pu1Byte)(&H2C_Parameter));

			}
		}
		else
		{
			break;
		}
	}
#endif

	if(tmpEntryMaxPWDB != 0)	
	{
		priv->EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;
	}
	else
	{
		priv->EntryMaxUndecoratedSmoothedPWDB = 0;
	}
	if(tmpEntryMinPWDB != 0xff) 
	{
		priv->EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;
	}
	else
	{
		priv->EntryMinUndecoratedSmoothedPWDB = 0;
	}

#if 0
	H2C_Parameter = priv->undecorated_smoothed_pwdb <<16;
#else
	H2C_Parameter[2] = (u8)(priv->undecorated_smoothed_pwdb & 0xFF);
	H2C_Parameter[0] = 0;
#endif
	FillH2CCmd92C(dev, H2C_RSSI_REPORT, 3, H2C_Parameter);
}

/*-----------------------------------------------------------------------------
 * Function:	dm_CheckRfCtrlGPIO()
 *
 * Overview:	Copy 8187B template for 9xseries.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	01/10/2008	MHC		Create Version 0.  
 *
 *---------------------------------------------------------------------------*/

extern	void	dm_CheckRfCtrlGPIO(void *data)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20))
	struct r8192_priv *priv = container_of_dwork_rsl(data,struct r8192_priv,gpio_change_rf_wq);
	struct net_device *dev = priv->rtllib->dev;
#else
	struct net_device *dev = (struct net_device *)data;
	struct r8192_priv *priv = rtllib_priv(dev);
#endif

	rtl8192ce_GPIOChangeRFWorkItemCallBack(dev);
	return;
}

void
dm_CheckProtection(struct net_device *dev)
{
#ifdef MERGE_TO_DO
	struct r8192_priv* 		priv = rtllib_priv(dev);
	u8			CurRate, RateThreshold;

	if(priv->rtllib->pHTInfo->bCurBW40MHz)
		RateThreshold = MGN_MCS1;
	else
		RateThreshold = MGN_MCS3;

	if(priv->TxStats.CurrentInitTxRate <= RateThreshold)
	{
		priv->bDmDisableProtect = true;
		DbgPrint("Forced disable protect: %x\n", priv->TxStats.CurrentInitTxRate);
	}
	else
	{
		priv->bDmDisableProtect = false;
		DbgPrint("Enable protect: %x\n", priv->TxStats.CurrentInitTxRate);
	}
#endif
}

void
dm_CheckStatistics(struct net_device *dev)
{
#ifdef MERGE_TO_DO
	struct r8192_priv* 		priv = rtllib_priv(dev);
	
	if(priv->rtllib->state < RTLLIB_LINKED )
		return;

	priv->rtllib->GetHwRegHandler( dev, HW_VAR_INIT_TX_RATE, (u8*)(&priv->stats.CurrentInitTxRate) );


	
	priv->rtllib->GetHwRegHandler( dev, HW_VAR_RETRY_COUNT, (u8*)(&priv->stats.NumTxRetryCount) );
#endif
}


void
dm_InterruptMigration(struct net_device *dev)
{

	struct r8192_priv* 		priv = rtllib_priv(dev);
	bool			bCurrentIntMt, bCurrentACIntDisable;
	bool			IntMtToSet = false; 
	bool			ACIntToSet = false;
	
	
	priv->rtllib->GetHwRegHandler(dev, HW_VAR_INT_MIGRATION, (u8*)(&bCurrentIntMt));
	priv->rtllib->GetHwRegHandler(dev, HW_VAR_INT_AC, (u8*)(&bCurrentACIntDisable));

	if(/*!pMgntInfo->bWiFiConfg && */priv->rtllib->state >= RTLLIB_LINKED &&
		priv->rtllib->LinkDetectInfo.bHigherBusyTraffic)
	{			
		IntMtToSet = true;

		if(priv->rtllib->LinkDetectInfo.bHigherBusyRxTraffic )
			ACIntToSet = true;				
	}		
	
	if( bCurrentIntMt != IntMtToSet )
		priv->rtllib->SetHwRegHandler(dev, HW_VAR_INT_MIGRATION, (u8*)&IntMtToSet);

	if( bCurrentACIntDisable != ACIntToSet )
		priv->rtllib->SetHwRegHandler(dev, HW_VAR_INT_AC, (u8*)&ACIntToSet);
	
}


void dm_check_ac_dc_power(struct net_device *dev) 
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static char *ac_dc_check_script_path = "/etc/realtek/wireless-rtl-ac-dc-power.sh";
	char *argv[] = {ac_dc_check_script_path,DRV_NAME,NULL};
	static char *envp[] = {"HOME=/", 
			"TERM=linux", 
			"PATH=/usr/bin:/bin", 
			 NULL};

	if(priv->ResetProgress == RESET_TYPE_SILENT)
	{
		RT_TRACE((COMP_INIT | COMP_POWER | COMP_RF), "GPIOChangeRFWorkItemCallBack(): Silent Reseting!!!!!!!\n");
		return;
	}

	if(priv->rtllib->state != RTLLIB_LINKED) {
		return;
	}
	call_usermodehelper(ac_dc_check_script_path,argv,envp,1);

	return;
}

extern void DeInitHalDm(struct net_device *dev)
{
}

extern void InitHalDm(struct net_device *dev)
{
	struct r8192_priv* priv = rtllib_priv(dev);

	priv->DM_Type = DM_Type_ByDriver;
	
	ODM_DMInit(dev);

#ifdef ENABLE_GPIO_RADIO_CTL
	INIT_DELAYED_WORK_RSL(&priv->gpio_change_rf_wq, (void *)dm_CheckRfCtrlGPIO,dev);
#endif
}

void
HalDmWatchDog(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	RT_RF_POWER_STATE 	rfState;
	unsigned long flag;
	
	RTPRINT(FDM, DM_Monitor, ("HalDmWatchDog() ==>\n"));

	dm_check_ac_dc_power(dev);

	priv->rtllib->GetHwRegHandler(dev, HW_VAR_RF_STATE, (u8*)(&rfState));

	spin_lock_irqsave(&priv->rf_ps_lock,flag);
			
	if((rfState == eRfOn) && (!priv->RFChangeInProgress))
	{
		priv->RFChangeInProgress = true;
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
		
		dm_CheckStatistics(dev);

		PWDB_Monitor(dev);

		ODM_DMWatchdog(dev);
		
	
		
		spin_lock_irqsave(&priv->rf_ps_lock,flag);
		priv->RFChangeInProgress = false;
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
	}
	else
	{
		spin_unlock_irqrestore(&priv->rf_ps_lock,flag);
	}

	RTPRINT(FDM, DM_Monitor, ("HalDmWatchDog() <==\n"));
}

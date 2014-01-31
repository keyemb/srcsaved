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

#ifndef	__HAL8192CPCIDM_H__
#define __HAL8192CPCIDM_H__


#define deinit_hal_dm			DeInitHalDm
#define init_hal_dm				InitHalDm
#define hal_dm_watchdog			HalDmWatchDog

#define		DM_Type_ByFW		0
#define		DM_Type_ByDriver	1


void DeInitHalDm(struct net_device *dev);
void HalDmWatchDog(struct net_device *dev);

extern	void	dm_CheckRfCtrlGPIO(void *data);
#endif	



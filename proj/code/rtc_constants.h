#ifndef _LCOM_RTC_CONSTANTS_H_
#define _LCOM_RTC_CONSTANTS_H_

/** @defgroup rtc_constants RTC Constants Module
 * @{
 *
 * Module that has constants for the RTC module
 */

// CONSTANTS ***********************************************************

#define BIT(n) (0x01<<(n))

// PORT ADDRESSES
#define RTC_ADDR_REG 0x70 /**< @brief RTC Address Reg */
#define RTC_DATA_REG 0x71 /**< @brief RC Data Reg */

// REG ADDRESSES
#define RTC_REG_SECONDS 0x0
#define RTC_REG_SECONDS_ALARM 0x1
#define RTC_REG_MINUTES 0x2
#define RTC_REG_MINUTES_ALARM 0x3
#define RTC_REG_HOURS 0x4
#define RTC_REG_HOURS_ALARM 0x5
#define RTC_REG_WEEK_DAY 0x6
#define RTC_REG_MONTH_DAY 0x7
#define RTC_REG_MONTH 0x8
#define RTC_REG_YEAR 0x9

#define RTC_REG_A 0xA /**< @brief RTC A Reg address */
#define RTC_REG_B 0xB /**< @brief RTC B Reg address */
#define RTC_REG_C 0xC /**< @brief RTC C Reg address */
#define RTC_REG_D 0xD /**< @brief RTC D Reg address */

// CONSTANTS
#define RTC_IRQ 0x8 /**< @brief RTC IRQ */

#define DELAY_244 244

#define RATE_MAX_mask 0xF

// MASKS *****************************************
#define UIP_mask 0x80
#define RATE_mask 0x0F

#define SET_mask 0x80
#define PIE_mask 0x40
#define AIE_mask 0x20
#define UIE_mask 0x10
#define SQWE_mask 0x08
#define DM_mask 0x04
#define HOUR_12_24_mask 0x02
#define DSE_mask 0x01

#define IRQF_mask 0x80
#define PF_mask 0x40
#define AF_mask 0x20
#define UF_mask 0x10

#define VRT_mask 0x80

#define left_mask 0xF0
#define right_mask 0x0F

/** @} */

#endif

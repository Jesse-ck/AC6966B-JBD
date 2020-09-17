#include "app_config.h"
#include "syscfg_id.h"
#include "smartbox_high_low_vol.h"

#include "smartbox_setting_sync.h"
#include "smartbox_setting_opt.h"
#include "le_smartbox_module.h"

#include "audio_dec.h"

#if (SMART_BOX_EN && RCSP_ADV_HIGH_LOW_SET)

struct _HIGH_LOW_VOL {
    int low_vol;
    int high_vol;
};

static struct _HIGH_LOW_VOL high_low_vol = {
    .low_vol  = 12,
    .high_vol = 12,
};

extern int get_bt_tws_connect_status();
extern int audio_out_eq_spec_set_gain(u8 idx, int gain);

static void get_high_low_vol_info(u8 *vol_gain_param)
{
    memcpy(vol_gain_param, &high_low_vol, sizeof(struct _HIGH_LOW_VOL));
}

static void set_high_low_vol_info(u8 *vol_gain_param)
{
    memcpy(&high_low_vol, vol_gain_param, sizeof(struct _HIGH_LOW_VOL));
}

static void update_high_low_vol_vm_value(u8 *vol_gain_param)
{
    syscfg_write(CFG_RCSP_ADV_HIGH_LOW_VOL, vol_gain_param, sizeof(struct _HIGH_LOW_VOL));
}

static void high_low_vol_setting_sync(u8 *vol_gain_param)
{
#if TCFG_USER_TWS_ENABLE
    if (get_bt_tws_connect_status()) {
        update_smartbox_setting(BIT(ATTR_TYPE_HIGH_LOW_VOL));
    }
#endif
}

static void high_low_vol_state_update(void)
{
    static int low_vol = 0;
    static int high_vol = 0;
    struct high_bass param = {0};
    if (low_vol != high_low_vol.low_vol) {
        param.gain = high_low_vol.low_vol - 12;
        mix_out_high_bass(AUDIO_EQ_BASS, &param);
        low_vol = high_low_vol.low_vol;
    }
    if (high_vol != high_low_vol.high_vol) {
        param.gain = high_low_vol.high_vol - 12;
        mix_out_high_bass(AUDIO_EQ_HIGH, &param);
        high_vol = high_low_vol.high_vol;
    }
}

static void deal_high_low_vol(u8 *vol_gain_data, u8 write_vm, u8 tws_sync)
{
    if (vol_gain_data) {
        set_high_low_vol_info(vol_gain_data);
        printf("low %d, high %d\n", high_low_vol.low_vol, high_low_vol.high_vol);
    }
    if (write_vm) {
        update_high_low_vol_vm_value((u8 *)&high_low_vol);
    }
    if (tws_sync) {
        high_low_vol_setting_sync((u8 *)&high_low_vol);
    }
    high_low_vol_state_update();
}

static SMARTBOX_SETTING_OPT high_low_vol_opt = {
    .data_len = 8,
    .setting_type = ATTR_TYPE_HIGH_LOW_VOL,
    .syscfg_id = CFG_RCSP_ADV_HIGH_LOW_VOL,
    .deal_opt_setting = deal_high_low_vol,
    .set_setting = set_high_low_vol_info,
    .get_setting = get_high_low_vol_info,
    .custom_setting_init = NULL,
    .custom_setting_release = NULL,
    .custom_vm_info_update = NULL,
    .custom_setting_update = NULL,
    .custom_sibling_setting_deal = NULL,
};
REGISTER_APP_SETTING_OPT(high_low_vol_opt);

#endif

#ifndef _EQ_APPLY_H_
#define _EQ_APPLY_H_

#include "typedef.h"
#include "asm/hw_eq.h"
#include "application/audio_eq.h"
#include "application/audio_drc.h"
#include "application/eq_config.h"
#include "media/audio_stream.h"

struct audio_eq_drc_parm {
    u8 eq_en: 1;				//eq是否使能 1:使能  0:关闭
    u8 drc_en: 1;           //drc是否使能 1:使能  0:关闭
    u8 high_bass: 1;        //高低音是否使能， 1：使能  0：关闭
    u8 async_en: 1;         //是否使能异步eq  1:使能  0：关闭
    u8 out_32bit: 1;        //eq后是否输出32bit  1:使能： 0关闭
    u8 divide_en: 1;        //各个声道eq drc效果是否独立，0：使用同个效果
    u8 mode_en: 1;          //没离线文件时，是否支持使用默认系数表做eq
    u8 online_en: 1;        //是否支持在线调试 1：支持  0：不支持
    u8 ch_num: 3;           //dac通道数
    u8 four_ch: 1;          //四声道eq drc是否使能， 1：使能   0：关闭
    u8 input_four_ch_dat: 1; //输入数据是否是4声道 1：是  0：不是
    u8 eq_name_four;        //四通道时，RL RR通道的eq_name
    u8 eq_name;             //FL FR通道的eq_name 普通音乐eq 使用song_eq_mode,通话下行eq 使用call_eq_mode
    u16 sr;                 //采样率

    audio_eq_filter_cb eq_cb; //获取eq系数的回调函数
    audio_drc_filter_cb drc_cb;//获取drc系数的回调
};
/*
*cmd :
*/
#define AUDIO_EQ_SET_CH        0
#define AUDIO_EQ_CLR_DAT       1
#define AUDIO_EQ_HIGH          2
#define AUDIO_EQ_BASS          3
#define AUDIO_EQ_GET_DATA_LEN  4
#define AUDIO_EQ_HIGH_BASS_DIS 5


struct high_bass {
    int freq;     //频率写0， 内部会用默认125hz  和12khz
    int gain;    //增益范围 -12~12
};

typedef struct eq_fade {
    u16 tmr;
    int cur_gain[2];
    int use_gain[2];
} audio_eq_fade_cfg;


struct audio_eq_drc {
    struct audio_eq *eq;
    struct audio_drc *drc;

    struct audio_eq *four_ch_eq;
    struct audio_drc *four_ch_drc;

    struct eq_fade *fade_cfg;//高低音系数表

    //4路32bit异步输出
    s16 *four_ch_buf[2];
    s16 *four_ch_buf_out;
    u8  four_ch_eq_work;

    //2路32bit异步输出
    s16 *eq_out_buf;
    int eq_out_buf_len;
    int eq_out_points;
    int eq_out_total;
    u8 eq_async_remain;
    u8 remain;
    u8 high_bass_dis;

    struct audio_eq_drc_parm parm;
    struct audio_stream_entry entry;	// 音频流入口
};


/*----------------------------------------------------------------------------*/
/**@brief    打开eq drc，支持接入audio_stream
   @param    parm: 功能使用参数,详见结构体定义
   @return   返回eq drc句柄
   @note
*/
/*----------------------------------------------------------------------------*/
struct audio_eq_drc *audio_eq_drc_open(struct audio_eq_drc_parm *parm);
/*----------------------------------------------------------------------------*/
/**@brief    eq drc接入音频流，数据处理回调
   @param    entry: 数据流节点
   @param    in:输入数据结构
   @param    out:输出数据结构
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void audio_eq_drc_close(struct audio_eq_drc *hdl);

/*----------------------------------------------------------------------------*/
/**@brief    参数更新
   @param    hdl:句柄
   @param    cmd:命令
   @param    parm:参数
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
int  audio_eq_drc_parm_update(struct  audio_eq_drc  *hdl,  u32 cmd, void  *parm);




/*----------------------------------------------------------------------------*/
/**@brief    audio_eq_open重新封装，简化使用,该接口不接入audio_stream流处理
   @param    *parm: eq参数句柄,参数详见结构体struct audio_eq_param
   @return   eq句柄
   @note
*/
/*----------------------------------------------------------------------------*/
struct audio_eq *audio_dec_eq_open(struct audio_eq_param *parm);
/*----------------------------------------------------------------------------*/
/**@brief    audio_eq_close重新封装，简化使用,该接口不接入audio_stream流处理
   @param    eq句柄
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void audio_dec_eq_close(struct audio_eq *eq);

/*----------------------------------------------------------------------------*/
/**@brief    audio_drc_open重新封装，简化使用,该接口不接入audio_stream流处理
   @param    *parm: drc参数句柄,参数详见结构体struct audio_drc_param
   @return   eq句柄
   @note
*/
/*----------------------------------------------------------------------------*/
struct audio_drc *audio_dec_drc_open(struct audio_drc_param *parm);
/*----------------------------------------------------------------------------*/
/**@brief    audio_drc_close重新封装，简化使用,该接口不接入audio_stream流处理
   @param    drc句柄
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void audio_dec_drc_close(struct audio_drc *drc);
#endif

#include "DialplateView.h"
#include <stdarg.h>
#include <stdio.h>

using namespace Page;

void DialplateView::Create(lv_obj_t* root)
{
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);

    BottomInfo_Create(root);
    TopInfo_Create(root);
    BtnCont_Create(root);
}

void DialplateView::SetSpeedMeter(int speed)
{
    int Speed[4] = {0};
    int i;

    static indicInfo_t info[4] = {
    {ui.topInfo.meterSpeed.meter, ui.topInfo.meterSpeed.indic_100},
    {ui.topInfo.meterSpeed.meter, ui.topInfo.meterSpeed.indic_200},
    {ui.topInfo.meterSpeed.meter, ui.topInfo.meterSpeed.indic_300},
    {ui.topInfo.meterSpeed.meter, ui.topInfo.meterSpeed.needle},
    };

    if (speed < 0)
        return;
    if (speed > 300)
        speed = 300;

    for (i = 0; i < speed/100; i++)
    {
      Speed[i] = 100;
    }
    Speed[i] = speed % 101;
    Speed[3] = speed % 101;

    /* anim */
    for (int i = 0; i < 4; i++) {
        lv_obj_add_anim((lv_obj_t *)&info[i],
                        NULL,
                        [](void* obj, int var) {
                            indicInfo_t* info = (indicInfo_t*)obj;
                            if (info->meter == NULL || info->indic == NULL)
                                return;
                            lv_meter_set_indicator_end_value(   info->meter,
                                                                info->indic,
                                                                var);
                        },
                        info[i].indic->end_value,
                        Speed[i],
                        1000);
    }
}

void DialplateView::TopInfo_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_HOR_RES, 142);

    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x333333), 0);

    lv_obj_set_style_radius(cont, 27, 0);
    lv_obj_set_y(cont, -36);
    ui.topInfo.cont = cont;

    lv_obj_t* label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_32"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, "00");
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
    ui.topInfo.labelSpeed = label;

    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_13"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, "km/h");
    lv_obj_align_to(label, ui.topInfo.labelSpeed, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    ui.topInfo.labelUint = label;

    lv_obj_t*  meter = lv_meter_create(cont);
    lv_obj_remove_style_all(meter);
    lv_obj_set_size(meter, 160, 160);
    lv_obj_set_style_transform_height(meter, 10, 0);
    lv_obj_set_style_transform_width(meter, 10, 0);
    lv_obj_set_style_radius(meter, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(meter, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_color(meter, lv_color_white(), 0);
    lv_obj_align(meter, LV_ALIGN_BOTTOM_MID, 0, 80);

    /*Add a scale first*/
    lv_meter_scale_t* scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 51, 2, 5, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale, 5, 2, 10, lv_color_white(), 15);
    lv_meter_set_scale_range(meter, scale, 0, 100, 160, 190);

    /*Add a three arc indicator*/
    lv_meter_indicator_t* indic1 = lv_meter_add_arc(meter, scale, 5, lv_palette_main(LV_PALETTE_LIGHT_GREEN), 0);
    lv_meter_indicator_t* indic2 = lv_meter_add_arc(meter, scale, 5, lv_color_hex(0xff931e), 5);
    lv_meter_indicator_t* indic3 = lv_meter_add_arc(meter, scale, 5, lv_palette_main(LV_PALETTE_RED), 10);
    lv_meter_indicator_t* needle = lv_meter_add_needle_line(meter, scale, 2, lv_color_white(), -7);

    ui.topInfo.meterSpeed = {
        meter,indic1,indic2,indic3,needle
    };

}

void DialplateView::BottomInfo_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_size(cont, LV_HOR_RES, 90);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 106);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_set_flex_align(
        cont,
        LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    ui.bottomInfo.cont = cont;

    const char* unitText[4] =
    {
        "AVG",
        "Time",
        "Trip",
        "Calorie"
    };

    for (int i = 0; i < ARRAY_SIZE(ui.bottomInfo.labelInfoGrp); i++)
    {
        SubInfoGrp_Create(
            cont,
            &(ui.bottomInfo.labelInfoGrp[i]),
            unitText[i]
        );
    }
}

void DialplateView::SubInfoGrp_Create(lv_obj_t* par, SubInfo_t* info, const char* unitText)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 93, 39);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        cont,
        LV_FLEX_ALIGN_SPACE_AROUND,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    lv_obj_t* label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_17"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    info->lableValue = label;

    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_13"), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xb3b3b3), 0);
    lv_label_set_text(label, unitText);
    info->lableUnit = label;

    info->cont = cont;
}

void DialplateView::BtnCont_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_HOR_RES, 40);
    lv_obj_align_to(cont, ui.bottomInfo.cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    /*lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_place(
        cont,
        LV_FLEX_PLACE_SPACE_AROUND,
        LV_FLEX_PLACE_CENTER,
        LV_FLEX_PLACE_CENTER
    );*/

    ui.btnCont.cont = cont;

    ui.btnCont.btnMap = Btn_Create(cont, Resource.GetImage("locate"), -80);
    ui.btnCont.btnRec = Btn_Create(cont, Resource.GetImage("start"), 0);
    ui.btnCont.btnMenu = Btn_Create(cont, Resource.GetImage("menu"), 80);
}

lv_obj_t* DialplateView::Btn_Create(lv_obj_t* par, const void* img_src, lv_coord_t x_ofs)
{
    lv_obj_t* obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 40, 31);

    lv_obj_align(obj, LV_ALIGN_CENTER, x_ofs, 0);
    lv_obj_set_style_bg_img_src(obj, img_src, 0);

    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_width(obj, 45, LV_STATE_PRESSED);
    lv_obj_set_style_height(obj, 25, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff931e), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(obj, 9, 0);

    static lv_style_transition_dsc_t tran;
    static const lv_style_prop_t prop[] = { LV_STYLE_WIDTH, LV_STYLE_HEIGHT, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(
        &tran,
        prop,
        lv_anim_path_ease_out,
        200,
        0,
        nullptr
    );
    lv_obj_set_style_transition(obj, &tran, LV_STATE_PRESSED);
    lv_obj_set_style_transition(obj, &tran, LV_STATE_FOCUSED);

    lv_obj_update_layout(obj);

    return obj;
}

void DialplateView::AppearAnimStart(bool playback)
{
#define ANIM_DEF(start_time, obj, attr, start, end) \
     {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true}

#define ANIM_OPA_DEF(start_time, obj) \
     ANIM_DEF(start_time, obj, opa_scale, LV_OPA_TRANSP, LV_OPA_COVER)

    static lv_coord_t y_tar_top = lv_obj_get_y(ui.topInfo.cont);
    static lv_coord_t y_tar_bottom = lv_obj_get_y(ui.bottomInfo.cont);
    static lv_coord_t h_tar_btn = lv_obj_get_height(ui.btnCont.btnRec);

    lv_anim_timeline_t anim_timeline[] =
    {
        ANIM_DEF(0, ui.topInfo.cont, y, -lv_obj_get_height(ui.topInfo.cont), y_tar_top),

        ANIM_DEF(200, ui.bottomInfo.cont, y, -lv_obj_get_height(ui.bottomInfo.cont), y_tar_bottom),
        ANIM_OPA_DEF(200, ui.bottomInfo.cont),

        ANIM_DEF(500, ui.btnCont.btnMap, height, 0, h_tar_btn),
        ANIM_DEF(600, ui.btnCont.btnRec, height, 0, h_tar_btn),
        ANIM_DEF(700, ui.btnCont.btnMenu, height, 0, h_tar_btn),
        LV_ANIM_TIMELINE_END
    };

    lv_anim_timeline_start(anim_timeline, playback);
}

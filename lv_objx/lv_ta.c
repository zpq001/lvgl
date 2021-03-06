/**
 * @file lv_ta.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TA != 0

#include "lv_ta.h"
#include "lvgl/lv_misc/anim.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
/*Test configuration*/
#ifndef LV_TA_MAX_LENGTH
#define LV_TA_MAX_LENGTH    256
#endif

#ifndef LV_TA_CUR_BLINK_TIME
#define LV_TA_CUR_BLINK_TIME 400    /*ms*/
#endif

#define LV_TA_DEF_WIDTH     (120 * LV_DOWNSCALE)
#define LV_TA_DEF_HEIGHT    (80 * LV_DOWNSCALE)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_ta_design(lv_obj_t * ta, const area_t * mask, lv_design_mode_t mode);
static bool lv_ta_scrling_design(lv_obj_t * scrling, const area_t * mask, lv_design_mode_t mode);
static void lv_ta_hide_cursor(lv_obj_t * ta, uint8_t hide);
static void lv_ta_save_valid_cursor_x(lv_obj_t * ta);
static void lv_tas_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_tas_t lv_tas_def;
static lv_tas_t lv_tas_simple;
static lv_tas_t lv_tas_transp;

lv_design_f_t ancestor_design_f;
lv_design_f_t scrl_design_f;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*----------------- 
 * Create function
 *-----------------*/

/**
 * Create a text area objects
 * @param par pointer to an object, it will be the parent of the new text area
 * @param copy pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
lv_obj_t * lv_ta_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor object*/
    lv_obj_t * new_ta = lv_page_create(par, copy);
    dm_assert(new_ta);
    
    /*Allocate the object type specific extended data*/
    lv_ta_ext_t * ext = lv_obj_alloc_ext(new_ta, sizeof(lv_ta_ext_t));
    dm_assert(ext);
    ext->cur_hide = 0;
    ext->cursor_pos = 0;
    ext->cursor_valid_x = 0;
    ext->label = NULL;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_ta);
    if(scrl_design_f == NULL) scrl_design_f = lv_obj_get_design_f(ext->page.scrl);

    lv_obj_set_signal_f(new_ta, lv_ta_signal);
    lv_obj_set_design_f(new_ta, lv_ta_design);

    /*Init the new text area object*/
    if(copy == NULL) {
    	ext->label = lv_label_create(new_ta, NULL);

    	lv_obj_set_design_f(ext->page.scrl, lv_ta_scrling_design);
    	lv_label_set_long_mode(ext->label, LV_LABEL_LONG_BREAK);
    	lv_label_set_text(ext->label, "Text area");
    	lv_page_glue_obj(ext->label, true);
    	lv_obj_set_click(ext->label, false);
    	lv_obj_set_style(new_ta, lv_tas_get(LV_TAS_DEF, NULL));
    	lv_obj_set_size(new_ta, LV_TA_DEF_WIDTH, LV_TA_DEF_HEIGHT);
    }
    /*Copy an existing object*/
    else {
    	lv_obj_set_design_f(ext->page.scrl, lv_ta_scrling_design);
    	lv_ta_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->label = lv_label_create(new_ta, copy_ext->label);
    	lv_page_glue_obj(ext->label, true);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_ta);
    }
    
    /*Create a cursor blinker animation*/
    anim_t a;
    a.var = new_ta;
    a.fp = (anim_fp_t)lv_ta_hide_cursor;
    a.time = LV_TA_CUR_BLINK_TIME;
    a.act_time = 0;
    a.end_cb = NULL;
    a.start = 0;
    a.end= 1;
    a.repeat = 1;
    a.repeat_pause = 0;
    a.playback = 1;
    a.playback_pause = 0;
    a.path = anim_get_path(ANIM_PATH_STEP);
    anim_create(&a);

    return new_ta;
}

/**
 * Signal function of the text area
 * @param ta pointer to a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_ta_signal(lv_obj_t * ta, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_page_signal(ta, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    	lv_tas_t * style = lv_obj_get_style(ta);
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			/* Nothing to clean up.
    			 * (The created label will be deleted automatically) */
    			break;
    		case LV_SIGNAL_STYLE_CHG:
    		    if(ext->label) {
                    lv_obj_set_style(ext->label, &style->labels);
                    lv_obj_set_width(ext->label, lv_obj_get_width(ta) - 2 *
                            (style->pages.bg_rects.hpad + style->pages.scrl_rects.hpad));
                    lv_label_set_text(ext->label, NULL);
    		    }
    			break;
    		/*Set the label width according to the text area width*/
    		case LV_SIGNAL_CORD_CHG:
    		    if(ext->label != NULL) {
                    lv_obj_set_width(ext->label, lv_obj_get_width(ta) - 2 *
                            (style->pages.bg_rects.hpad + style->pages.scrl_rects.hpad));
                    lv_label_set_text(ext->label, NULL);
    		    }
    			break;
    		default:
    			break;
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Insert a character to the current cursor position
 * @param ta pointer to a text area object
 * @param c a character
 */
void lv_ta_add_char(lv_obj_t * ta, char c)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);

	const char * label_txt = lv_label_get_text(ext->label);

	/*Test the new length: txt length + 1 (closing'\0') + 1 (c character)*/
    if((strlen(label_txt) + 2) > LV_TA_MAX_LENGTH) return;
    char buf[LV_TA_MAX_LENGTH];

    /*Insert the character*/
	memcpy(buf, label_txt, ext->cursor_pos);
	buf[ext->cursor_pos] = c;
	memcpy(buf+ext->cursor_pos+1, label_txt+ext->cursor_pos, strlen(label_txt) - ext->cursor_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ext->label, buf);

	/*Move the cursor after the new character*/
	lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) + 1);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
}

/**
 * Insert a text to the current cursor position
 * @param ta pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void lv_ta_add_text(lv_obj_t * ta, const char * txt)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);

	const char * label_txt = lv_label_get_text(ext->label);
    uint16_t label_len = strlen(label_txt);
    uint16_t txt_len = strlen(txt);

    /*Test the new length (+ 1 for the closing '\0')*/
    if((label_len + txt_len + 1) > LV_TA_MAX_LENGTH) return;

    /*Insert the text*/
    char buf[LV_TA_MAX_LENGTH];

	memcpy(buf, label_txt, ext->cursor_pos);
	memcpy(buf + ext->cursor_pos, txt, txt_len);
	memcpy(buf + ext->cursor_pos + txt_len, label_txt+ext->cursor_pos, label_len - ext->cursor_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ext->label, buf);

	/*Move the cursor after the new text*/
	lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) + txt_len);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
}

/**
 * Set the text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void lv_ta_set_text(lv_obj_t * ta, const char * txt)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	lv_label_set_text(ext->label, txt);
	lv_ta_set_cursor_pos(ta, LV_TA_CUR_LAST);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
}

/**
 * Delete a the left character from the current cursor position
 * @param ta pointer to a text area object
 */
void lv_ta_del(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	uint16_t cur_pos = ext->cursor_pos;

	if(cur_pos == 0) return;

	/*Delete a character*/
	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ext->label);
	uint16_t label_len = strlen(label_txt);
	memcpy(buf, label_txt, cur_pos - 1);
	memcpy(buf+cur_pos - 1, label_txt + cur_pos, label_len - cur_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ext->label, buf);

	/*Move the cursor to the place of the deleted character*/
	lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) - 1);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
}


/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 *             < 0 : index from the end of the text
 *             LV_TA_CUR_LAST: go after the last character
 */
void lv_ta_set_cursor_pos(lv_obj_t * ta, int16_t pos)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	uint16_t txt_len = strlen(lv_label_get_text(ext->label));

	if(pos < 0) pos = txt_len + pos;

	if(pos > txt_len || pos == LV_TA_CUR_LAST) pos = txt_len;

	ext->cursor_pos = pos;

	/*Position the label to make the cursor visible*/
	lv_obj_t * label_par = lv_obj_get_parent(ext->label);
	point_t cur_pos;
	lv_tas_t * style = lv_obj_get_style(ta);
	const font_t * font_p = font_get(style->labels.font);
	area_t label_cords;
    area_t ta_cords;
	lv_label_get_letter_pos(ext->label, pos, &cur_pos);
	lv_obj_get_cords(ta, &ta_cords);
    lv_obj_get_cords(ext->label, &label_cords);

	/*Check the top*/
	if(lv_obj_get_y(label_par) + cur_pos.y < 0) {
		lv_obj_set_y(label_par, - cur_pos.y);
	}

	/*Check the bottom*/
	if(label_cords.y1 + cur_pos.y + font_get_height(font_p) + style->pages.scrl_rects.vpad > ta_cords.y2) {
		lv_obj_set_y(label_par, -(cur_pos.y - lv_obj_get_height(ta) +
				                     font_get_height(font_p) + 2 * style->pages.scrl_rects.vpad));
	}

	lv_obj_inv(ta);
}


/**
 * Move the cursor one character right
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_right(lv_obj_t * ta)
{
	uint16_t cp = lv_ta_get_cursor_pos(ta);
	cp++;
	lv_ta_set_cursor_pos(ta, cp);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
}

/**
 * Move the cursor one character left
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_left(lv_obj_t * ta)
{
	uint16_t cp = lv_ta_get_cursor_pos(ta);
	if(cp > 0)  {
		cp--;
		lv_ta_set_cursor_pos(ta, cp);

		/*It is a valid x step so save it*/
		lv_ta_save_valid_cursor_x(ta);
	}
}

/**
 * Move the cursor one line down
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_down(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	point_t pos;

	/*Get the position of the current letter*/
	lv_label_get_letter_pos(ext->label, lv_ta_get_cursor_pos(ta), &pos);

	/*Increment the y with one line and keep the valid x*/
	lv_labels_t * label_style = lv_obj_get_style(ext->label);
	const font_t * font_p = font_get(label_style->font);
	pos.y += font_get_height(font_p) + label_style->line_space + 1;
	pos.x = ext->cursor_valid_x;

	/*Do not go below he last line*/
	if(pos.y < lv_obj_get_height(ext->label)) {
		/*Get the letter index on the new cursor position and set it*/
		uint16_t new_cur_pos = lv_label_get_letter_on(ext->label, &pos);
		lv_ta_set_cursor_pos(ta, new_cur_pos);
	}
}

/**
 * Move the cursor one line up
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_up(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	point_t pos;

	/*Get the position of the current letter*/
	lv_label_get_letter_pos(ext->label, lv_ta_get_cursor_pos(ta), &pos);

	/*Decrement the y with one line and keep the valid x*/
	lv_labels_t * label_style = lv_obj_get_style(ext->label);
	const font_t * font = font_get(label_style->font);
	pos.y -= font_get_height(font) + label_style->line_space - 1;
	pos.x = ext->cursor_valid_x;

	/*Get the letter index on the new cursor position and set it*/
	uint16_t new_cur_pos = lv_label_get_letter_on(ext->label, &pos);
	lv_ta_set_cursor_pos(ta, new_cur_pos);
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the i the text area
 * @param ta obj pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_txt(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	return lv_label_get_text(ext->label);
}

/**
 * Get the current cursor position in character index
 * @param ta pointer to a text area object
 * @return the cursor position
 */
uint16_t lv_ta_get_cursor_pos(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	return ext->cursor_pos;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_tas_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_tas_t style
 */
lv_tas_t * lv_tas_get(lv_tas_builtin_t style, lv_tas_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_tas_init();
		style_inited = true;
	}

	lv_tas_t  *style_p;

	switch(style) {
		case LV_TAS_DEF:
			style_p = &lv_tas_def;
			break;
        case LV_TAS_SIMPLE:
            style_p = &lv_tas_simple;
            break;
        case LV_TAS_TRANSP:
            style_p = &lv_tas_transp;
            break;
		default:
			style_p = &lv_tas_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_tas_t));
		else memcpy(copy, &lv_tas_def, sizeof(lv_tas_t));
	}

	return style_p;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the text areas
 * @param ta pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_ta_design(lv_obj_t * ta, const area_t * masp, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_design_f(ta, masp, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		ancestor_design_f(ta, masp, mode);

    } else if(mode == LV_DESIGN_DRAW_POST) {
		ancestor_design_f(ta, masp, mode);
    }
    return true;
}

/**
 * An extended scrolling design of the page. Calls the normal design function and it draws a cursor.
 * @param label pointer to a text area object
 * @param mask  the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @return return true/false, depends on 'mode'
 */
static bool lv_ta_scrling_design(lv_obj_t * scrling, const area_t * mask, lv_design_mode_t mode)
{
	if(mode == LV_DESIGN_COVER_CHK) {
		/*Return false if the object is not covers the mask_p area*/
		return scrl_design_f(scrling, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		scrl_design_f(scrling, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_POST) {
		scrl_design_f(scrling, mask, mode);

		/*Draw the cursor too*/
		lv_obj_t * ta = lv_obj_get_parent(scrling);
		lv_ta_ext_t * ta_ext = lv_obj_get_ext(ta);
		lv_tas_t * ta_style = lv_obj_get_style(ta);

		if(ta_style->cursor_show != 0 && ta_ext->cur_hide == 0) {
			uint16_t cur_pos = lv_ta_get_cursor_pos(ta);
			point_t letter_pos;
			lv_label_get_letter_pos(ta_ext->label, cur_pos, &letter_pos);

			area_t cur_area;
			lv_labels_t * labels_p = lv_obj_get_style(ta_ext->label);
			cur_area.x1 = letter_pos.x + ta_ext->label->cords.x1 - (ta_style->cursor_width >> 1);
			cur_area.y1 = letter_pos.y + ta_ext->label->cords.y1;
			cur_area.x2 = letter_pos.x + ta_ext->label->cords.x1 + (ta_style->cursor_width >> 1);
			cur_area.y2 = letter_pos.y + ta_ext->label->cords.y1 + font_get_height(font_get(labels_p->font));

			lv_rects_t cur_rects;
			lv_rects_get(LV_RECTS_DEF, &cur_rects);
			cur_rects.round = 0;
			cur_rects.bwidth = 0;
			cur_rects.objs.color = ta_style->cursor_color;
			cur_rects.gcolor = ta_style->cursor_color;
			lv_draw_rect(&cur_area, mask, &cur_rects, OPA_COVER);
		}
	}

	return true;
}


/**
 * Set the cursor visibility to make a blinking cursor
 * @param ta pointer to a text area
 * @param hide 1: hide the cursor, 0: draw it
 */
static void lv_ta_hide_cursor(lv_obj_t * ta, uint8_t hide)
{
	lv_ta_ext_t * ta_ext = lv_obj_get_ext(ta);
	if(hide != ta_ext->cur_hide) {
        ta_ext->cur_hide = hide  == 0 ? 0 : 1;
        lv_obj_inv(ta);
	}
}

/**
 * Save the cursor x position as valid. It is important when jumping up/down to a shorter line
 * @param ta pointer to a text area object
 */
static void lv_ta_save_valid_cursor_x(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	point_t cur_pos;
	lv_label_get_letter_pos(ext->label, ext->cursor_pos, &cur_pos);
	ext->cursor_valid_x = cur_pos.x;
}

/**
 * Initialize the text area styles
 */
static void lv_tas_init(void)
{
	/*Default style*/
	lv_pages_get(LV_PAGES_DEF, &lv_tas_def.pages);
	lv_tas_def.pages.sb_mode = LV_PAGE_SB_MODE_DRAG;

	lv_labels_get(LV_LABELS_TXT, &lv_tas_def.labels);
	lv_tas_def.labels.objs.color = COLOR_MAKE(0x20, 0x20, 0x20);

	lv_tas_def.cursor_color = COLOR_MAKE(0x10, 0x10, 0x10);
	lv_tas_def.cursor_width = 1 * LV_DOWNSCALE;	/*>=1 px for visible cursor*/
	lv_tas_def.cursor_show = 1;

	memcpy(&lv_tas_simple, &lv_tas_def, sizeof(lv_tas_t));
    lv_pages_get(LV_PAGES_SIMPLE, &lv_tas_simple.pages);
    lv_tas_simple.pages.sb_mode = LV_PAGE_SB_MODE_DRAG;
    lv_tas_simple.pages.scrl_rects.objs.transp = 0; /*if transp == 1 the cursor will not be drawn*/
    lv_tas_simple.pages.bg_rects.objs.color = COLOR_WHITE;
    lv_tas_simple.pages.bg_rects.gcolor = COLOR_SILVER;
    lv_tas_simple.pages.bg_rects.bcolor = COLOR_GRAY;

    memcpy(&lv_tas_transp, &lv_tas_def, sizeof(lv_tas_t));
    lv_pages_get(LV_PAGES_TRANSP, &lv_tas_transp.pages);
    lv_tas_transp.pages.scrl_rects.objs.transp = 0; /*if transp == 1 the cursor will not be drawn*/

}
#endif

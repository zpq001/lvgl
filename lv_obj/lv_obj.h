/**
 * @file lv_obj.h
 * 
 */

#ifndef LV_OBJ_H
#define LV_OBJ_H

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/lv_misc/area.h>
#include <stddef.h>
#include <stdbool.h>
#include "misc/mem/dyn_mem.h"
#include "misc/mem/linked_list.h"
#include "misc/others/color.h"

/*********************
 *      DEFINES
 *********************/
/*Error check of lv_conf.h*/
#if LV_HOR_RES == 0 || LV_VER_RES == 0
#error "LV: LV_HOR_RES and LV_VER_RES must be greater then 0"
#endif

#if LV_DOWNSCALE != 1 && LV_DOWNSCALE != 2
#error "LV: LV_DOWNSCALE can be only 1 or 2"
#endif

#if LV_VDB_SIZE == 0 && LV_ANTIALIAS != 0
#error "LV: If LV_VDB_SIZE == 0 the antialaissing must be disabled"
#endif

/*New defines*/
#define LV_OBJ_DEF_WIDTH  (80 * LV_DOWNSCALE)
#define LV_OBJ_DEF_HEIGHT  (60 * LV_DOWNSCALE)

#define ANIM_IN					0x00	/*Animation to show an object. 'OR' it with lv_anim_builtin_t*/
#define ANIM_OUT				0x80    /*Animation to hide an object. 'OR' it with lv_anim_builtin_t*/
#define ANIM_DIR_MASK			0x80	/*ANIM_IN/ANIM_OUT mask*/

/**********************
 *      TYPEDEFS
 **********************/

struct __LV_OBJ_T;

typedef enum
{
    LV_DESIGN_DRAW_MAIN,
    LV_DESIGN_DRAW_POST,
    LV_DESIGN_COVER_CHK,
}lv_design_mode_t;

typedef bool (* lv_design_f_t) (struct __LV_OBJ_T * obj, const area_t * mask_p, lv_design_mode_t mode);

typedef enum
{
	LV_SIGNAL_CLEANUP,
    LV_SIGNAL_PRESSED,
	LV_SIGNAL_PRESSING,
    LV_SIGNAL_PRESS_LOST,
    LV_SIGNAL_RELEASED,
    LV_SIGNAL_LONG_PRESS,
    LV_SIGNAL_LONG_PRESS_REP,
    LV_SIGNAL_DRAG_BEGIN,
    LV_SIGNAL_DRAG_END,        
    LV_SIGNAL_CHILD_CHG,
    LV_SIGNAL_CORD_CHG,
    LV_SIGNAL_STYLE_CHG,
	LV_SIGNAL_REFR_EXT_SIZE,
}lv_signal_t;

typedef bool (* lv_signal_f_t) (struct __LV_OBJ_T * obj, lv_signal_t sign, void * param);

typedef struct __LV_OBJ_T
{
    struct __LV_OBJ_T * par;
    ll_dsc_t child_ll;
    
    area_t cords;

    lv_signal_f_t signal_f;
    lv_design_f_t design_f;
    
    void * ext;           /*The object attributes can be extended here*/
    void * style_p;       /*Object specific style*/

#if LV_OBJ_FREE_P != 0
    void * free_p;        /*Application specific pointer (set it freely)*/
#endif

    /*Attributes and states*/
    uint8_t click_en     :1;    /*1: can be pressed by a display input device*/
    uint8_t drag_en      :1;    /*1: enable the dragging*/
    uint8_t drag_throw_en:1;    /*1: Enable throwing with drag*/
    uint8_t drag_parent  :1;    /*1. Parent will be dragged instead*/
    uint8_t style_iso	 :1;	/*1: The object has got an own style*/
    uint8_t hidden       :1;    /*1: Object is hidden*/
    uint8_t top_en       :1;    /*1: If the object or its children  is clicked it goes to the foreground*/
    uint8_t reserved     :1;

    uint8_t protect;            /*Automatically happening actions can be prevented. 'OR'ed values from lv_obj_prot_t*/

    cord_t ext_size;			/*EXTtend the size of the object in every direction. Used to draw shadow, shine etc.*/

    uint8_t free_num; 		    /*Application specific identifier (set it freely)*/
	opa_t opa;

    
}lv_obj_t;

/*Protect some attributes (max. 8 bit)*/
typedef enum
{
    LV_PROTECT_NONE      = 0x00,
    LV_PROTECT_CHILD_CHG = 0x01, /*Disable the child change signal. Used by the library*/
    LV_PROTECT_OPA       = 0x02, /*Prevent lv_obj_set_opar to modify the opacity*/
    LV_PROTECT_PARENT    = 0x04, /*Prevent automatic parent change (e.g. in lv_page)*/
    LV_PROTECT_POS       = 0x08, /*Prevent automatic positioning (e.g. in lv_rect layout)*/
}lv_protect_t;

typedef enum
{
    LV_ALIGN_CENTER = 0,
	LV_ALIGN_IN_TOP_LEFT,
	LV_ALIGN_IN_TOP_MID,
	LV_ALIGN_IN_TOP_RIGHT,
	LV_ALIGN_IN_BOTTOM_LEFT,
	LV_ALIGN_IN_BOTTOM_MID,
	LV_ALIGN_IN_BOTTOM_RIGHT,
	LV_ALIGN_IN_LEFT_MID,
	LV_ALIGN_IN_RIGHT_MID,
	LV_ALIGN_OUT_TOP_LEFT,
	LV_ALIGN_OUT_TOP_MID,
	LV_ALIGN_OUT_TOP_RIGHT,
	LV_ALIGN_OUT_BOTTOM_LEFT,
	LV_ALIGN_OUT_BOTTOM_MID,
	LV_ALIGN_OUT_BOTTOM_RIGHT,
	LV_ALIGN_OUT_LEFT_TOP,
	LV_ALIGN_OUT_LEFT_MID,
	LV_ALIGN_OUT_LEFT_BOTTOM,
	LV_ALIGN_OUT_RIGHT_TOP,
	LV_ALIGN_OUT_RIGHT_MID,
	LV_ALIGN_OUT_RIGHT_BOTTOM,
}lv_align_t;


typedef struct
{
	color_t color;
	uint8_t transp :1;
}lv_objs_t;

typedef enum
{
	LV_OBJS_DEF,
	LV_OBJS_SCR,
	LV_OBJS_TRANSP,
}lv_objs_builtin_t;

typedef enum
{
	LV_ANIM_NONE = 0,
	LV_ANIM_FADE,			/*Animate the opacity*/
	LV_ANIM_FLOAT_TOP, 		/*Float from/to the top*/
	LV_ANIM_FLOAT_LEFT,		/*Float from/to the left*/
	LV_ANIM_FLOAT_BOTTOM,	/*Float from/to the bottom*/
	LV_ANIM_FLOAT_RIGHT,	/*Float from/to the right*/
	LV_ANIM_GROW_H,			/*Grow/shrink  horizontally*/
	LV_ANIM_GROW_V,			/*Grow/shrink  vertically*/
}lv_anim_builtin_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the 'lv' library.
 */
void lv_init(void);

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_inv(lv_obj_t * obj);

/**
 * Notify an object about its style is modified
 * @param obj pointer to an object
 */
void lv_obj_refr_style(lv_obj_t * obj);

/**
 * Notify all object if a style is modified
 * @param style pinter to a style. Only objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_style_refr_all(void * style);

/**
 * Create a basic object
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 * @param copy pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent, lv_obj_t * copy);

/**
 * Delete 'obj' and all of its children
 * @param obj
 */
void lv_obj_del(lv_obj_t * obj);

/**
 * Signal function of the basic object
 * @param obj pointer to an object
 * @param sign signal type
 * @param param parameter for the signal (depends on signal type)
 * @return false: the object become invalid (e.g. deleted)
 */
bool lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

/**
 * Return with a pointer to built-in style and/or copy it to a variable
 * @param style a style name from lv_objs_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_objs_t style
 */
lv_objs_t * lv_objs_get(lv_objs_builtin_t style, lv_objs_t * copy_p);

/**
 * Load a new screen
 * @param scr pointer to a screen
 */
void lv_scr_load(lv_obj_t * scr);

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj pointer to an object
 * @param parent pointer to the new parent object
 */
void lv_obj_set_parent(lv_obj_t * obj, lv_obj_t * parent);

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void lv_obj_set_pos(lv_obj_t * obj, cord_t x, cord_t y);

/**
 * Set relative the position of an object (relative to the parent).
 * The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent. (will be multiplied with LV_DOWNSCALE)
 * @param y new distance from the top of the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_pos_us(lv_obj_t * obj, cord_t x, cord_t y);

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t * obj, cord_t x);

/**
 * Set the x coordinate of a object.
 * The coordinate will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_x_us(lv_obj_t * obj, cord_t x);

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t * obj, cord_t y);

/**
 * Set the y coordinate of a object.
 * The coordinate will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param y new distance from the top of the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_y_us(lv_obj_t * obj, cord_t y);

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t * obj, cord_t w, cord_t h);

/**
 * Set the size of an object. The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param w new width (will be multiplied with LV_DOWNSCALE)
 * @param h new height (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_size_us(lv_obj_t * obj, cord_t w, cord_t h);

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t * obj, cord_t w);

/**
 * Set the width of an object.  The width will be upscaled to compensate LV_DOWNSCALE
 * @param obj pointer to an object
 * @param w new width (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_width_us(lv_obj_t * obj, cord_t w);

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t * obj, cord_t h);

/**
 * Set the height of an object.  The height will be upscaled to compensate LV_DOWNSCALE
 * @param obj pointer to an object
 * @param h new height (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_height_us(lv_obj_t * obj, cord_t h);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align(lv_obj_t * obj,lv_obj_t * base, lv_align_t align, cord_t x_mod, cord_t y_mod);

/**
 * Align an object to an other object. The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment (will be multiplied with LV_DOWNSCALE)
 * @param y_mod y coordinate shift after alignment (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_align_us(lv_obj_t * obj,lv_obj_t * base, lv_align_t align, cord_t x_mod, cord_t y_mod);

/**
 * Set the extended size of an object
 * @param obj pointer to an object
 * @param ext_size the extended size
 */
void lv_obj_set_ext_size(lv_obj_t * obj, cord_t ext_size);

/**
 * Set a new style for an object
 * @param obj pointer to an object
 * @param style_p pointer to the new style
 */
void lv_obj_set_style(lv_obj_t * obj, void * style);

/**
 * Isolate the style of an object. In other words a unique style will be created
 * for this object which can be freely modified independently from the style of the
 * other objects.
 */
void * lv_obj_iso_style(lv_obj_t * obj, uint32_t style_size);

/**
 * Set the opacity of an object
 * @param obj pointer to an object
 * @param opa 0 (transparent) .. 255(fully cover)
 */
void lv_obj_set_opa(lv_obj_t * obj, uint8_t opa);

/**
 * Set the opacity of an object and all of its children
 * @param obj pointer to an object
 * @param opa 0 (transparent) .. 255(fully cover)
 */
void lv_obj_set_opar(lv_obj_t * obj, uint8_t opa);

/**
 * Hide an object. It won't be visible and clickable.
 * @param obj pointer to an object
 * @param en true: hide the object
 */
void lv_obj_set_hidden(lv_obj_t * obj, bool en);

/**
 * Enable or disable the clicking of an object
 * @param obj pointer to an object
 * @param en true: make the object clickable
 */
void lv_obj_set_click(lv_obj_t * obj, bool en);

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj pointer to an object
 * @param en true: enable the auto top feature
 */
void lv_obj_set_top(lv_obj_t * obj, bool en);

/**
 * Enable the dragging of an object
 * @param obj pointer to an object
 * @param en true: make the object dragable
 */
void lv_obj_set_drag(lv_obj_t * obj, bool en);

/**
 * Enable the throwing of an object after is is dragged
 * @param obj pointer to an object
 * @param en true: enable the drag throw
 */
void lv_obj_set_drag_throw(lv_obj_t * obj, bool en);

/**
 * Enable to use parent for drag related operations.
 * If trying to drag the object the parent will be moved instead
 * @param obj pointer to an object
 * @param en true: enable the 'drag parent' for the object
 */
void lv_obj_set_drag_parent(lv_obj_t * obj, bool en);

/**
 * Set a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from lv_obj_prot_t
 */
void lv_obj_set_protect(lv_obj_t * obj, uint8_t prot);

/**
 * Clear a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from lv_obj_prot_t
 */
void lv_obj_clr_protect(lv_obj_t * obj, uint8_t prot);

/**
 * Set the signal function of an object.
 * Always call the previous signal function in the new.
 * @param obj pointer to an object
 * @param fp the new signal function
 */
void lv_obj_set_signal_f(lv_obj_t * obj, lv_signal_f_t fp);

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param fp the new design function
 */
void lv_obj_set_design_f(lv_obj_t * obj, lv_design_f_t fp);

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return Normal pointer to the allocated ext
 */
void * lv_obj_alloc_ext(lv_obj_t * obj, uint16_t ext_size);

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object
 * @param obj pointer to an object
 */
void lv_obj_refr_ext_size(lv_obj_t * obj);

/**
 * Set an application specific number for an object.
 * It can help to identify objects in the application.
 * @param obj pointer to an object
 * @param free_num the new free number
 */
void lv_obj_set_free_num(lv_obj_t * obj, uint8_t free_num);

/**
 * Set an application specific  pointer for an object.
 * It can help to identify objects in the application.
 * @param obj pointer to an object
 * @param free_p the new free pinter
 */
void lv_obj_set_free_p(lv_obj_t * obj, void * free_p);

/**
 * Animate an object
 * @param obj pointer to an object to animate
 * @param type type of animation from 'lv_anim_builtin_t'. 'OR' it with ANIM_IN or ANIM_OUT
 * @param time time of animation in milliseconds
 * @param delay delay before the animation in milliseconds
 * @param cb a function to call when the animation is ready
 */
void lv_obj_anim(lv_obj_t * obj, lv_anim_builtin_t type, uint16_t time, uint16_t delay, void (*cb) (lv_obj_t *));

/**
 * Return with the actual screen
 * @return pointer to to the actual screen object
 */
lv_obj_t * lv_scr_act(void);

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
lv_obj_t * lv_obj_get_scr(lv_obj_t * obj);

/**
 * Returns with the parent of an object
 * @param obj pointer to an object
 * @return pointer to the parent of  'obj'
 */
lv_obj_t * lv_obj_get_parent(lv_obj_t * obj);

/**
 * Iterate through the children of an object
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child(lv_obj_t * obj, lv_obj_t * child);

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_get_child_num(lv_obj_t * obj);

/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates
 */
void lv_obj_get_cords(lv_obj_t * obj, area_t * cords_p);

/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent
 */
cord_t lv_obj_get_x(lv_obj_t * obj);

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent
 */
cord_t lv_obj_get_y(lv_obj_t * obj);

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
cord_t lv_obj_get_width(lv_obj_t * obj);

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
cord_t lv_obj_get_height(lv_obj_t * obj);

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
cord_t lv_obj_getext_size(lv_obj_t * obj);

/**
 * Get the style pointer of an object
 * @param obj pointer to an object
 * @return pointer to a style
 */
void * lv_obj_get_style(lv_obj_t * obj);

/**
 * Get the opacity of an object
 * @param obj pointer to an object
 * @return 0 (transparent) .. 255 (fully cover)
 */
opa_t lv_obj_get_opa(lv_obj_t * obj);

/**
 * Get the hidden attribute of an object
 * @param obj pointer to an object
 * @return true: the object is hidden
 */
bool lv_obj_get_hidden(lv_obj_t * obj);

/**
 * Get the click enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is clickable
 */
bool lv_obj_get_click(lv_obj_t * obj);

/**
 * Get the top enable attribute of an object
 * @param obj pointer to an object
 * @return true: the auto top feture is enabled
 */
bool lv_obj_get_top(lv_obj_t * obj);

/**
 * Get the drag enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is dragable
 */
bool lv_obj_get_drag(lv_obj_t * obj);

/**
 * Get the drag thow enable attribute of an object
 * @param obj pointer to an object
 * @return true: drag throw is enabled
 */
bool lv_obj_get_drag_throw(lv_obj_t * obj);

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool lv_obj_get_drag_parent(lv_obj_t * obj);

/**
 * Get the style isolation attribute of an object
 * @param obj pointer to an object
 * @return pointer to a style
 */
bool lv_obj_get_style_iso(lv_obj_t * obj);

/**
 * Get the protect field of an object
 * @param obj pointer to an object
 * @return protect field ('OR'ed values of lv_obj_prot_t)
 */
uint8_t lv_obj_get_protect(lv_obj_t * obj);

/**
 * Check at least one bit of a given protect bitfield is set
 * @param obj pointer to an object
 * @param prot protect bits to test ('OR'ed values of lv_obj_prot_t)
 * @return false: none of the given bits are set, true: at least one bit is set
 */
bool lv_obj_is_protected(lv_obj_t * obj, uint8_t prot);

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
lv_signal_f_t   lv_obj_get_signal_f(lv_obj_t * obj);

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
lv_design_f_t lv_obj_get_design_f(lv_obj_t * obj);

/**
 * Get the ext pointer
 * @param obj pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext)->data1
 */
void * lv_obj_get_ext(lv_obj_t * obj);

/**
 * Get the free number
 * @param obj pointer to an object
 * @return the free number
 */
uint8_t lv_obj_get_free_num(lv_obj_t * obj);

/**
 * Get the free pointer
 * @param obj pointer to an object
 * @return the free pointer
 */
void * lv_obj_get_free_p(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#define LV_SA(obj, style_type) ((style_type *) obj->style_p)
#define LV_EA(obj, ext_type) ((ext_type *) obj->ext)

#endif

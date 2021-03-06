#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	KEY_POWER					= 1,
	KEY_UP						= 2,
	KEY_DOWN					= 3,
	KEY_LEFT					= 4,
	KEY_RIGHT					= 5,
	KEY_VOLUME_UP				= 6,
	KEY_VOLUME_DOWN				= 7,
	KEY_VOLUME_MUTE				= 8,
	KEY_TAB						= 9,
	KEY_HOME					= 10,
	KEY_BACK					= 11,
	KEY_MENU					= 12,
	KEY_ENTER					= 13,

	KEY_L_CTRL					= 14,
	KEY_R_CTRL					= 15,
	KEY_L_ALT					= 16,
	KEY_R_ALT					= 17,
	KEY_L_SHIFT					= 18,
	KEY_R_SHIFT					= 19,

	KEY_F1						= 20,
	KEY_F2						= 21,
	KEY_F3						= 22,
	KEY_F4						= 23,
	KEY_F5						= 24,
	KEY_F6						= 25,
	KEY_F7						= 26,
	KEY_F8						= 27,
	KEY_F9						= 28,
	KEY_F10						= 29,
	KEY_F11						= 30,
	KEY_F12						= 31,

	KEY_SPACE					= 32,	/*   */
	KEY_EXCLAMATION_MARK		= 33,	/* ! */
	KEY_QUOTATION_MARK			= 34,	/* " */
	KEY_POUNDSIGN				= 35,	/* # */
	KEY_DOLLAR					= 36,	/* $ */
	KEY_PERCENT					= 37,	/* % */
	KEY_AMPERSAND				= 38,	/* & */
	KEY_APOSTROPHE				= 39,	/* ' */
	KEY_PARENTHESIS_LEFT		= 40,	/* ( */
	KEY_PARENTHESIS_RIGHT		= 41,	/* ) */
	KEY_ASTERISK				= 42,	/* * */
	KEY_PLUS					= 43,	/* + */
	KEY_COMMA					= 44,	/* , */
	KEY_MINUS					= 45,	/* - */
	KEY_FULL_STOP				= 46,	/* . */
	KEY_SOLIDUS					= 47,	/* / */

	KEY_0						= 48,	/* 0 */
	KEY_1						= 49,	/* 1 */
	KEY_2						= 50,	/* 2 */
	KEY_3						= 51,	/* 3 */
	KEY_4						= 52,	/* 4 */
	KEY_5						= 53,	/* 5 */
	KEY_6						= 54,	/* 6 */
	KEY_7						= 55,	/* 7 */
	KEY_8						= 56,	/* 8 */
	KEY_9						= 57,	/* 9 */

	KEY_COLON					= 58,	/* : */
	KEY_SEMICOLON				= 59,	/* ; */
	KEY_LESS_THAN				= 60,	/* < */
	KEY_EQUAL					= 61,	/* = */
	KEY_GREATER_THAN			= 62,	/* > */
	KEY_QUESTION_MARK			= 63,	/* ? */
	KEY_AT						= 64,	/* @ */

	KEY_A						= 65,	/* A */
	KEY_B						= 66,	/* B */
	KEY_C						= 67,	/* C */
	KEY_D						= 68,	/* D */
	KEY_E						= 69,	/* E */
	KEY_F						= 70,	/* F */
	KEY_G						= 71,	/* G */
	KEY_H						= 72,	/* H */
	KEY_I						= 73,	/* I */
	KEY_J						= 74,	/* J */
	KEY_K						= 75,	/* K */
	KEY_L						= 76,	/* L */
	KEY_M						= 77,	/* M */
	KEY_N						= 78,	/* N */
	KEY_O						= 79,	/* O */
	KEY_P						= 80,	/* P */
	KEY_Q						= 81,	/* Q */
	KEY_R						= 82,	/* R */
	KEY_S						= 83,	/* S */
	KEY_T						= 84,	/* T */
	KEY_U						= 85,	/* U */
	KEY_V						= 86,	/* V */
	KEY_W						= 87,	/* W */
	KEY_X						= 88,	/* X */
	KEY_Y						= 89,	/* Y */
	KEY_Z						= 90,	/* Z */

	KEY_SQUARE_BRACKET_LEFT		= 91,	/* [ */
	KEY_REVERSE_SOLIDUS			= 92,	/* \ */
	KEY_SQUARE_BRACKET_RIGHT	= 93,	/* ] */
	KEY_CIRCUMFLEX_ACCENT		= 94,	/* ^ */
	KEY_LOW_LINE				= 95,	/* _ */
	KEY_GRAVE_ACCENT			= 96,	/* ` */

	KEY_a						= 97,	/* a */
	KEY_b						= 98,	/* b */
	KEY_c						= 99,	/* c */
	KEY_d						= 100,	/* d */
	KEY_e						= 101,	/* e */
	KEY_f						= 102,	/* f */
	KEY_g						= 103,	/* g */
	KEY_h						= 104,	/* h */
	KEY_i						= 105,	/* i */
	KEY_j						= 106,	/* j */
	KEY_k						= 107,	/* k */
	KEY_l						= 108,	/* l */
	KEY_m						= 109,	/* m */
	KEY_n						= 110,	/* n */
	KEY_o						= 111,	/* o */
	KEY_p						= 112,	/* p */
	KEY_q						= 113,	/* q */
	KEY_r						= 114,	/* r */
	KEY_s						= 115,	/* s */
	KEY_t						= 116,	/* t */
	KEY_u						= 117,	/* u */
	KEY_v						= 118,	/* v */
	KEY_w						= 119,	/* w */
	KEY_x						= 120,	/* x */
	KEY_y						= 121,	/* y */
	KEY_z						= 122,	/* z */

	KEY_CURLY_BRACKET_LEFT		= 123,	/* { */
	KEY_VERTICAL_LINE			= 124,	/* | */
	KEY_CURLY_BRACKET_RIGHT		= 125,	/* } */
	KEY_TILDE					= 126,	/* ~ */
	KEY_DELETE					= 127,

	/* Latin 1 extensions */
	KEY_NO_BREAK_SPACE			= 160,	/*   */
	KEY_EXCLAM_DOWN				= 161,	/* ?? */
	KEY_CENT					= 162,	/* ?? */
	KEY_STERLING				= 163,	/* ?? */
	KEY_CURRENCY				= 164,	/* ?? */
	KEY_YEN						= 165,	/* ?? */
	KEY_BROKEN_BAR				= 166,	/* ?? */
	KEY_SECTION					= 167,	/* ?? */
	KEY_DIAERESIS				= 168,	/* ?? */
	KEY_COPYRIGHT				= 169,	/* ?? */
	KEY_ORDFEMININE				= 170,	/* ?? */
	KEY_GUILLEMOTLEFT			= 171,	/* ?? */
	KEY_NOTSIGN					= 172,	/* ?? */
	KEY_HYPHEN					= 173,	/* ??  */
	KEY_REGISTERED				= 174,	/* ?? */
	KEY_MACRON					= 175,	/* ?? */
	KEY_DEGREE					= 176,	/* ?? */
	KEY_PLUSMINUS				= 177,	/* ?? */
	KEY_TWOSUPERIOR				= 178,	/* ?? */
	KEY_THREESUPERIOR			= 179,	/* ?? */
	KEY_ACUTE					= 180,	/* ?? */
	KEY_MU						= 181,	/* ?? */
	KEY_PARAGRAPH				= 182,	/* ?? */
	KEY_PERIODCENTERED			= 183,	/* ?? */
	KEY_CEDILLA					= 184,	/* ?? */
	KEY_ONESUPERIOR				= 185,	/* ?? */
	KEY_MASCULINE				= 186,	/* ?? */
	KEY_GUILLEMOTRIGHT			= 187,	/* ?? */
	KEY_ONEQUARTER 				= 188,	/* ?? */
	KEY_ONEHALF 				= 189,	/* ?? */
	KEY_THREEQUARTERS 			= 190,	/* ?? */
	KEY_QUESTIONDOWN 			= 191,	/* ?? */
	KEY_AGRAVE 					= 192,	/* ?? */
	KEY_AACUTE 					= 193,	/* ?? */
	KEY_ACIRCUMFLEX 			= 194,	/* ?? */
	KEY_ATILDE 					= 195,	/* ?? */
	KEY_ADIAERESIS 				= 196,	/* ?? */
	KEY_ARING 					= 197,	/* ?? */
	KEY_AE 						= 198,	/* ?? */
	KEY_CCEDILLA 				= 199,	/* ?? */
	KEY_EGRAVE 					= 200,	/* ?? */
	KEY_EACUTE 					= 201,	/* ?? */
	KEY_ECIRCUMFLEX 			= 202,	/* ?? */
	KEY_EDIAERESIS 				= 203,	/* ?? */
	KEY_IGRAVE 					= 204,	/* ?? */
	KEY_IACUTE 					= 205,	/* ?? */
	KEY_ICIRCUMFLEX 			= 206,	/* ?? */
	KEY_IDIAERESIS 				= 207,	/* ?? */
	KEY_ETH 					= 208,	/* ?? */
	KEY_NTILDE 					= 209,	/* ?? */
	KEY_OGRAVE 					= 210,	/* ?? */
	KEY_OACUTE 					= 211,	/* ?? */
	KEY_OCIRCUMFLEX 			= 212,	/* ?? */
	KEY_OTILDE 					= 213,	/* ?? */
	KEY_ODIAERESIS 				= 214,	/* ?? */
	KEY_MULTIPLY 				= 215,	/* ?? */
	KEY_OOBLIQUE 				= 216,	/* ?? */
	KEY_UGRAVE 					= 217,	/* ?? */
	KEY_UACUTE 					= 218,	/* ?? */
	KEY_UCIRCUMFLEX 			= 219,	/* ?? */
	KEY_UDIAERESIS 				= 220,	/* ?? */
	KEY_YACUTE 					= 221,	/* ?? */
	KEY_THORN 					= 222,	/* ?? */
	KEY_ssharp 					= 223,	/* ?? */
	KEY_agrave 					= 224,	/* ?? */
	KEY_aacute 					= 225,	/* ?? */
	KEY_acircumflex				= 226,	/* ?? */
	KEY_atilde 					= 227,	/* ?? */
	KEY_adiaeresis 				= 228,	/* ?? */
	KEY_aring					= 229,	/* ?? */
	KEY_ae 						= 230,	/* ?? */
	KEY_ccedilla 				= 231,	/* ?? */
	KEY_egrave 					= 232,	/* ?? */
	KEY_eacute 					= 233,	/* ?? */
	KEY_ecircumflex 			= 234,	/* ?? */
	KEY_ediaeresis 				= 235,	/* ?? */
	KEY_igrave					= 236,	/* ?? */
	KEY_iacute					= 237,	/* ?? */
	KEY_icircumflex 			= 238,	/* ?? */
	KEY_idiaeresis				= 239,	/* ?? */
	KEY_eth 					= 240,	/* ?? */
	KEY_ntilde 					= 241,	/* ?? */
	KEY_ograve 					= 242,	/* ?? */
	KEY_oacute 					= 243,	/* ?? */
	KEY_ocircumflex 			= 244,	/* ?? */
	KEY_otilde 					= 245,	/* ?? */
	KEY_odiaeresis 				= 246,	/* ?? */
	KEY_DIVISION 				= 247,	/* ?? */
	KEY_ooblique 				= 248,	/* ?? */
	KEY_ugrave 					= 249,	/* ?? */
	KEY_uacute 					= 250,	/* ?? */
	KEY_ucircumflex 			= 251,	/* ?? */
	KEY_udiaeresis				= 252,	/* ?? */
	KEY_yacute 					= 253,	/* ?? */
	KEY_thorn 					= 254,	/* ?? */
	KEY_ydiaeresis 				= 255,	/* ?? */
};

#ifdef __cplusplus
}
#endif

#endif /* __KEYBOARD_H__ */

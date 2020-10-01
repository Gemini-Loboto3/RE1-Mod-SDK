; macro to define variables
%macro var 2
	global _%1
	_%1: equ %2
%endmacro

; macro to define functions
%macro func 2
	global _%1
	_%1:
	jmp[.a]
	.a: dd %2
%endmacro

; macro to define c++ function jumpers
%macro funp 2
	global _%1
	_%1: dd %2
%endmacro

; ---------------------------------------
; - VARIABLES							-
; ---------------------------------------
section .data

; DEFINITIONS
var pMarni,			0x4CFD38
var hWnd,			0x6A5F8C
var ExitAppFlg1,	0x4B3298
var nPlayMovie,		0x4B32A4
var Movie_usevfw,	0x4B34F0

var Marni_tile,		0xC30020
var Mess_sprt,		0xC30030
var Marni_line,		0xC30068
var G,				0xC33090
var GsEnv,			0x4AA300
var Game_timer,		0xAA8E10

; ---------------------------------------
; - FUNCTIONS							-
; ---------------------------------------
section .text

; DEFINITIONS
func Joint_move,			0x4010D0
func Remove_item,			0x4063A0
func Stage_init_items,		0x413EF0
func Cut_set,				0x4149D0
func Flg_ck,				0x415010
func Init_system,			0x416CB0
func Ck_gpu_win_init,		0x416C50
func SetBGClear,			0x416F30
func SetClearRGB,			0x416F50
func StMask,				0x4172C0
func DisplayRects,			0x4172F0
func DriverNames,			0x417410
; scheduler
func Task_execute,			0x417CF0
func Task_sleep,			0x417D20
func Task_exit,				0x417D40
func Task_chain,			0x417D50
func Task_suspend,			0x417D80
func Task_signal,			0x417DA0
; gte
func ApplyMatrixLV,			0x433230
func ApplyMatrix,			0x433310
func RotMatrix,				0x433350
func MultMatrix,			0x433430
func MultMatrix2,			0x4336D0
func CompMatrix,			0x4336F0
func CompM,					0x433750
func CompMatrix1,			0x433770
func MulRotMatrix0,			0x4337B0
func ScaleMatrixL,			0x433800
func VectorPow,				0x4338E0
func LoadAverage12,			0x433910
func SquareRoot0,			0x433A60
func SquareRoot12,			0x433A90
func OuterProduct0,			0x433AB0
func GsSetRefView2,			0x433BE0
func GsSetProjection,		0x433E40
func SetTransMatrix,		0x433F20
func SetRotMatrix,			0x433F40
func TransSvector,			0x433FB0
func RotAverage4,			0x434060
func TransMatrix,			0x4340E0
func SetPolyFT4,			0x434120

func Open_register_data,	0x4343B0
func Load_register_data,	0x434400
func Save_register_data,	0x434900
func load_file,				0x43E970
func UploadTexture,			0x43F290
func UnloadTexture,			0x43F2E0
func Draw_glyph,			0x441120
func Add_sprt,				0x441920
func Add_tile,				0x443350
func UploadTexture_id,		0x443950
func Add_line,				0x443C40
func Load_itemboxTim,		0x44A570
func PlayMovie,				0x44C840
func Snd_se_on,				0x468310
func Snd_em,				0x4684A0
func pak_dec,				0x46CBB0
func Goto00,				0x460BC0
func DrawDummyFrame,		0x484B00
func Title,					0x48DC20
func Get_item_name,			0x491440

funp marniSurface_Lock,			0x41E750
funp marniSurface_Unlock,		0x41E7B0
funp marniSurface_GetColor,		0x41F550
funp marniSurface_CreateWork,	0x41F9C0
funp marniSurface_Release,		0x41FC70
funp marniDestroy,				0x47BA10

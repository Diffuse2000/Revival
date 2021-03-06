#ifndef FDS_VARS_H_INCLUDED
#define FDS_VARS_H_INCLUDED
#include "FDS_DEFS.H"
#include "BaseDefs.h"

// _data must be allocated
class mmreg
{
public:
	union
	{
		byte _d8[8];
		word _d16[4];
		sword _sd16[4];
		dword _d32[2];
		uint64 _d64;
	};

	static void clearstate();

	// arithmetic operations
	// x.op(r) means x += r
	// x.op(r1,r2) means x = r1+r2


	// addition
	inline void addb(mmreg &r);
	inline void addb(mmreg &r1, mmreg &r2);	
	inline void addw(mmreg &r);
	inline void addw(mmreg &r1, mmreg &r2);
	inline void addd(mmreg &r);
	inline void addd(mmreg &r1, mmreg &r2);
	inline void addq(mmreg &r);
	inline void addq(mmreg &r1, mmreg &r2);

	// subtraction
	inline void subb(mmreg &r);
	inline void subb(mmreg &r1, mmreg &r2);	
	inline void subw(mmreg &r);
	inline void subw(mmreg &r1, mmreg &r2);
	inline void subd(mmreg &r);
	inline void subd(mmreg &r1, mmreg &r2);
	inline void subq(mmreg &r);
	inline void subq(mmreg &r1, mmreg &r2);

	// cxx commands are used for convinient packing/unpacking
	inline void cbw();
	inline void cwb();
	inline void packbw();
	inline void packwb();

	// multiplication
	inline void mulh(mmreg &r1);
	inline void mulh(mmreg &r1, mmreg &r2);
	inline void shrb(uint64 c);
	inline void shrw(uint64 c);
	inline void shrd(uint64 c);
	inline void shrq(uint64 c);
	inline void sarb(uint64 c);
	inline void sarw(uint64 c);
	inline void sard(uint64 c);
	inline void sarq(uint64 c);
	inline void shlb(uint64 c);
	inline void shlw(uint64 c);
	inline void shld(uint64 c);
	inline void shlq(uint64 c);
};
// implementation using inline functions
#include "MISC/mmreg.inl"
#include "Material.h"
#include "Vertex.h"
#include "Face.h"
#include "Camera.h"
#include "Edge.h"

#pragma pack(push, 1)
// Math variables and structures
// [4 Bytes]
struct Four_Chars
{
	unsigned char A,B,C,D;
};
// [4 Bytes]
union Radix256
{
	unsigned long DW; //the whole DWord
	Four_Chars CH; // A = Highest byte,D = Lowest Byte.
};

// [8 Bytes]
struct Complex
{
	float x, y;
};


// [16 Bytes]
struct Plane
{
	Vector N;
	float O;
};

struct Line
{
	Vector O,D;
};

struct Vector4
{
	float x, y, z, T;
};

extern float SinTab[1440];
extern float CosTab[1440];
extern Matrix Mat_ID;

// Digital Image (32bit) with variable size.
typedef struct
{
	long x, y;
	DWord *Data;
	char *FileName;
} Image;

typedef struct
{
	long U,V;   // 24:8 Fixed point mapping coordinates.
//	long RGB;   // Lighting/Spectral components.	
//	long PARAGRAPH_ALIGN;
	long R,G,B;
} GridPoint;

typedef struct
{
	long U,V;   // 24:8 Fixed point mapping coordinates.
	union
	{
		__int64 RGBA;   // Lighting/Spectral components.
		struct
		{
			word R, G, B, A;
		};
	};
//	long PARAGRAPH_ALIGN;
//	long R,G,B;
} NewGridPoint;

struct GridPointTG
{
	long u, v;	
	mmreg BGRA;
};

struct GridPointT
{
	long u, v;	
};

typedef struct
{
	float x ,y;  // Position on screen
	long RGB;   // Lighting/Spectral components
	long PARAGRAPH_ALIGN;
} AGridPoint;

typedef struct
{
	int x, y;
	float z, U, V;
} Bi_Triangle;

typedef struct
{
	int x, y;
	float z, U, V;
	float R,G,B;
} Bi_GTriangle;

typedef struct
{
	float x, y;
	float z,U,V;
} Bi_Floating;

// [32 Bytes]
// Total: [48 Bytes+64K per Charperpixel,+1Kb if Palettized]

struct Scene; // Temporary declaration for use with the Material structure.

struct CurLight
{
	Color Col;
	Vector Pos; // position in relation to model
	float Range; // light range (before mesh scaling)
	float Range2;
	float rRange;
	dword	dummy1,dummy2;
};

struct MusicModuleInfo
{
	dword ModuleHandle;
	dword Order;
	dword Pattern;
	dword Row;
};

extern "C" int MMXState;

extern char G_String[256]; //General string.

// illumination
extern float Ambient_Factor;
extern float Diffusive_Factor;
extern float Specular_Factor;
extern float Range_Factor;
extern float ImageSize;

// Global variables that hold the current scene info


extern Scene            * CurScene;       // Globally referenced scene.

extern float           CurFrame;       // Current frame
extern Camera           * View;           // Main active camera
extern Camera             FC;             // Free Camera..(Future Crew rulez)
extern Material           Default_Mat;    // Default Material...
extern Material         * MatLib;         // yosi

extern unsigned short int OldFPUCW,FPUCW;
extern dword OldMXCSR, CurMXCSR;
extern long Timer_Freq;

extern char **MsgStr;
extern long *MsgClock;
extern DWord *MsgID;


extern char *TxtrBuf,*TxtrAlign;
// Face List. Allocate before use.
extern Face **FList;
extern Face **SList;
extern float FOVX,FOVY;
extern Face *DoFace; //Currently rendered face. Used for further mapper info.
extern float dTime;
extern float Exec_FPS;
extern long Polys,Frames;
extern long CPolys,COmnies,CPcls,CAll,Polys;
extern long Bytes1[257];
extern long Bytes2[257];
extern long Bytes3[257];
extern long Bytes4[257];
extern float C_rFZP,C_FZP, C_rNZP, C_NZP;
extern dword g_renderedPolys, g_MipLevel;

extern "C" float g_zscale;
extern "C" float g_zscale256;

extern int32_t g_FrameTime;


extern DWord Colors32[256];

// Register structures
struct _RMWORDREGS {
	unsigned short ax, bx, cx, dx, si, di, cflag;
};

struct _RMBYTEREGS {
	unsigned char   al, ah, bl, bh, cl, ch, dl, dh;
};

typedef union {
	struct  _RMWORDREGS x;
	struct  _RMBYTEREGS h;
} RMREGS;

typedef struct {
	unsigned short  es;
	unsigned short  cs;
	unsigned short  ss;
	unsigned short  ds;
} RMSREGS;

typedef struct {
	long    edi;
	long    esi;
	long    ebp;
	long    reserved;
	long    ebx;
	long    edx;
	long    ecx;
	long    eax;
	short   flags;
	short   es,ds,fs,gs,ip,cs,sp,ss;
} _RMREGS;

// Module memory block
// Total size [32+Name size] bytes.
struct Module_MB
{
	DWord Address;
	DWord Handle;  //For low blocks, this will be [Selector:Segment].
	DWord Size;
	Module_MB *Next,*Prev,*Father,*Child,*Brother; //Brother I can see the light!
	// This holds Linked list, Hierarchial tree and Binary tree at the same
	// time. Linked list used for [de]Allocation,h.tree for info/delete
	char *Name;
};

struct Aligned_MB
{
	long Pos; //Position in aligned memory space.
	Aligned_MB *Prev,*Next;
};

extern DWord DPMI_InitFlag;
extern void *DPMI_Main;
extern Module_MB *DPMI_Core;
//char *DPMI_Heap; //Internal Heap (for Aligned memory support)
//DWord DPMI_Heap_Handle;

#pragma pack(push, 4)
// VESA variables and structures
typedef char VPalette[768]; // Direct Video 8-BIT Palette

// Quantized point in 2D
/*struct QPoint
{
	long X,Y;
};

struct Rectangle
{
	QPoint *UL,*LR; //Upper left, Lower right
};

// will only be logically supported at this point.
struct Window
{
	Rectangle Loc;   //Location on screen
	QPoint DispOffs; //Display offset
};*/

// [92 Bytes]
struct VESA_Surface
{
	byte *Data,*Targ;
	float *ZBuffer;
	long X,Y;
	long BPP,CPP;
	long BPSL;
	long CX,CY,X_1,Y_1; //X/2,Y/2,X-1,Y-1 respectively.
	float FOV;          //In degrees (for convinience)
	float PX,PY;        //Perspective ratios
	long PageSize;      //Size of allocation in Data.
	DWord Flags;        //Attributes for the surface.
	long *YTable;       //Y Offset table
//	Window *Wnd;        //Lousy piece of shit
	char *Pal;          //SHHIIIIT
	void (*Flip)(VESA_Surface *VS); //appropriate Flipping procedure to LFB.
	VESA_Surface *Next,*Prev;
	void *Handle;
};


// [13 Bytes]
// Packed for [24 Bytes]
typedef struct
{
	byte *Ch;
	byte *Len;
	char X,Y,BPP;
	unsigned short XY;
} Font;

// [11 Bytes]
// Packed for [16 Bytes]
struct VESA_Listinfo
{
	long X,Y;             // X and Y resolution
	unsigned short Mode;  // True mode notation (Displayed in Hex)
	char BPP;             // Bits per Pixel
};

struct VideoMode
{
	long XRes,YRes;
	long BPP;
	long BPSL;
	long Driver_ID;
	long Mode_ID;
	char *VideoMem;
	long PhysMem;
	DWord Flags;
};

extern short CurrentGraphicsMode;
extern void *VESAModule,*VESABuffer;
extern short *VESA_ModesAvail;
extern long VESA_NumModes;
extern long *YOffs;
extern char Palette_Reserved;

// General info
extern char Graphmode;  //set if client is on active g.mode state.
//otherwise it won't be able to determine what
//to do with the surfaces...
extern char Granola;         //GRANOLA=GRANULARITY ;), that is 64Kb/Vesamode->WinGranularity
extern "C" long VESA_BPSL;       //Crazy bytes per scan line crap. not currently supported.
extern "C" long BPP_Shift;
extern "C" long PageSize;
extern char Screenmem_Model; //1=LFB,2=Banks,3=Fake 14bit (not supported)
extern VESA_Surface *Screen; //this is where the LFB surface resides
extern VESA_Surface *MainSurf;   //this is regular surface u flip fastest,aka Page2 aka VPage
extern VESA_Surface *Last_Surface;
extern long VESA_Ver;

extern char Direct_FLIP;

// Global Surface Scalar variables (Fastest access)
extern "C" long XRes,YRes,XRes_1,YRes_1,CntrX,CntrY,BPP,CPP;
extern float CntrEX,CntrEY;
extern long PageSize,PageSizeW,PageSizeDW;

extern VideoMode VMode;
extern byte *VGAPtr,*VPage; //Direct access pointers to LFB/Banks and v.Page[2].
extern float *ZBuffer;
extern Font *Active_Font;    //Currently Active font
extern Font *Font1; //a font
extern char *VESA_Pal;

extern float CParticle_ISize;


extern void (*Flip)(VESA_Surface *VS);
extern VESA_Surface Layer2;

extern long Basic_FOffs; //basic font offset

// ISR variables and structures
const long Rate_Div = 1193180;
const unsigned int Basic_Rate = 0xFFFF;
extern volatile int32_t Timer;
extern long Old_Int_Count,Old_Int_Rate;
extern volatile char Keyboard[128];

//extern Material Flare_IR,Flare_IG,Flare_IB;
extern Material Flare;

// Span buffer variables
//extern char *Bitmap;

// Frustrum variables
extern float FOVX,FOVY;
extern float FillerPixelcount;

// Filler externals
extern dword zReject, zPass;
extern __int64 precisePixelCount;

typedef int (__stdcall *VPFunc)(
								void *lpAddress,		// address of region of committed pages
								unsigned long dwSize,			// size of the region
								unsigned long flNewProtect,	// desired access protection
								unsigned long *lpflOldProtect // address of variable to get old protection
								);

//extern long ASM_XRes;
extern float *InvTbl;


extern long Kbpos;

// Particle System Variables
// [192 Bytes]
struct Particle
{
	Vertex V;
	Face F;
	Vector Conv,Vel;
	QColor Color;
	float Grav,Charge;
	float Mass;
	void (*GFX)(Particle *P);
	float Radius;
	float PRad;
	DWord Flags;
};





////////////////////////////////////
// Radiosity unit - Global variables
extern DWord SCacheSize;

#pragma pack(push , 1)

//stuff
extern "C" dword WobNumOfHorizontalBlocks;
extern "C" dword WobNumOfVerticalBlocks;
extern "C" dword WobBlockJump;
extern "C" dword WobBPSL;

extern "C" dword MMXWobNumOfHorizontalBlocks;
extern "C" dword MMXWobNumOfVerticalBlocks;
extern "C" dword MMXWobBlockJump;
extern "C" dword MMXWobBPSL;

extern "C" DWord WOBPOINTSHEIGHT;
extern long Grid_X,Grid_Y,Grid_XY;
extern float Plane_Z2U_Conv;
extern float Plane_X2V_Conv;
extern float WRFOV;
extern Vector *WPDT;

// for the rend1 scene renderer
extern Particle *ExpPcls;
extern long NumExpPcls;

extern Vertex **ColVtx_Stack,**CVS;
extern Edge **ColEdge_Stack,**CES;

extern FILE *LogFile;

///////////////////////
/// TABLES!!!!!!!!!!!
extern DWord _t_sqrttab[4096];
extern DWord _t_rsqrttab[4096];
extern DWord _t_exptab[4096];
extern DWord _t_rexptab[4096];
extern float _t_fxfl;


////////////////////////////////////////////////////////////////////
///////////// ASSEMBLER / AUXILERY PRAGMAS SECTION /////////////////
////////////////////////////////////////////////////////////////////
#ifdef _C_WATCOM
void MatrixXVector(Matrix M,Vector *U,Vector *V);
#pragma aux MatrixXVector=\
	"FLD   DWORD PTR [ESI]"\
	"FMUL  DWORD PTR [EDX]"\
	"FLD   DWORD PTR [ESI]"\
	"FMUL  DWORD PTR [EDX+12]"\
	"FLD   DWORD PTR [ESI]"\
	"FMUL  DWORD PTR [EDX+24]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FMUL  DWORD PTR [EDX+4]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FMUL  DWORD PTR [EDX+16]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FMUL  DWORD PTR [EDX+28]"\
	"FXCH  ST(2)"\
	"FADDP ST(5),ST(0)"\
	"FADDP ST(3),ST(0)"\
	"FADDP ST(1),ST(0)"\
	"FLD   DWORD PTR [ESI+8]"\
	"FMUL  DWORD PTR [EDX+8]"\
	"FLD   DWORD PTR [ESI+8]"\
	"FMUL  DWORD PTR [EDX+20]"\
	"FLD   DWORD PTR [ESI+8]"\
	"FMUL  DWORD PTR [EDX+32]"\
	"FXCH  ST(2)"\
	"FADDP ST(5),ST(0)"\
	"FADDP ST(3),ST(0)"\
	"FADDP ST(1),ST(0)"\
	"FXCH  ST(1)"\
	"FXCH  ST(2)"\
	"FSTP  DWORD PTR [EDI]"\
	"FSTP  DWORD PTR [EDI+8]"\
	"FSTP  DWORD PTR [EDI+4]"\
	parm [EDX] [ESI] [EDI]\
modify exact [8087];


void Vector_Add(Vector *U,Vector *V,Vector *W);
#pragma aux Vector_Add=\
	"FLD   DWORD PTR [ESI]"\
	"FADD  DWORD PTR [EDX]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FADD  DWORD PTR [EDX+4]"\
	"FLD   DWORD PTR [ESI+8]"\
	"FADD  DWORD PTR [EDX+8]"\
	"FXCH  ST(2)"\
	"FSTP  DWORD PTR [EDI]"\
	"FSTP  DWORD PTR [EDI+4]"\
	"FSTP  DWORD PTR [EDI+8]"\
	parm [ESI] [EDX] [EDI]\
modify exact [8087];

void Vector_Sub(Vector *U,Vector *V,Vector *W);
#pragma aux Vector_Sub=\
	"FLD   DWORD PTR [ESI]"\
	"FSUB  DWORD PTR [EDX]"\
	"FLD   DWORD PTR [ESI+4]"\
	"FSUB  DWORD PTR [EDX+4]"\
	"FLD   DWORD PTR [ESI+8]"\
	"FSUB  DWORD PTR [EDX+8]"\
	"FXCH  ST(2)"\
	"FSTP  DWORD PTR [EDI]"\
	"FSTP  DWORD PTR [EDI+4]"\
	"FSTP  DWORD PTR [EDI+8]"\
	parm [ESI] [EDX] [EDI]\
modify exact [8087];

/*void Vector_Scale(Vector *U,float &Scale,Vector *W);
#pragma aux Vector_Scale=\
"FLD   DWORD PTR [ESI]"\
"FMUL  DWORD PTR [EDX]"\
"FLD   DWORD PTR [ESI+4]"\
"FMUL  DWORD PTR [EDX]"\
"FLD   DWORD PTR [ESI+8]"\
"FMUL  DWORD PTR [EDX]"\
"FXCH  ST(2)"\
"FSTP  DWORD PTR [EDI]"\
"FSTP  DWORD PTR [EDI+4]"\
"FSTP  DWORD PTR [EDI+8]"\
parm [ESI] [EDX] [EDI]\
modify exact [8087];
*/

void FPU_LPrecision(void);
#pragma aux FPU_LPrecision=\
	"   FNStCW  [OldFPUCW]            " \
	"   Mov     ax,[OldFPUCW]         " \
	"   And     eax,1111101111111111b " \
	"   Or      eax,0000100000000000b " \
	"   And     eax,64767             " \
	"   Mov     [FPUCW],ax        " \
	"   FLdCW   [FPUCW]           " \
modify exact [eax 8087];

void FPU_NormalState(void);
#pragma aux FPU_NormalState=\
	"   FLdCW   [OldFPUCW]"\
modify exact [8087];

void RoundToInt(long *dest, float val);
#pragma aux RoundToInt=\
	"fistp DWORD [eax]"\
	parm nomemory [eax] [8087]\
modify exact [eax 8087];

void RoundToDW(DWord *dest, float val);
#pragma aux RoundToDW=\
	"fistp DWORD [eax]"\
	parm nomemory [eax] [8087]\
modify exact [eax 8087];


float SQRT(float Value);
#pragma aux SQRT =                               \
	"     mov   ecx,eax"                          \
	"     and   eax,0xffffff"                     \
	"     shr   ecx,23"                           \
	"     xor   eax,0x800000"                     \
	"     shr   eax,12"                           \
	"     mov   edx,[eax*4+_t_sqrttab]"           \
	"     mov   ecx,[ecx*4+_t_exptab]"            \
	"     or    ecx,edx"                          \
	"     mov   _t_fxfl,ecx"                      \
	"     fld   _t_fxfl"                          \
    parm caller [eax]                            \
modify [eax ecx edx];

/*ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ*/
// 1.0 / sqrt - fast reciprocal square root (to within 5 digits)

float RSQRT(float Value);

#pragma aux RSQRT =                           \
	"     mov   ecx,eax"                           \
	"     and   eax,0xffffff"                      \
	"     shr   ecx,23"                            \
	"     xor   eax,0x800000"                      \
	"     shr   eax,12"                            \
	"     mov   edx,[eax*4+_t_rsqrttab]"           \
	"     mov   ecx,[ecx*4+_t_rexptab]"            \
	"     test  edx,edx"                           \
	"     jz    rsqrt_psqr"                        \
	"     or    ecx,edx"                           \
	"     mov   _t_fxfl,ecx"                       \
	"     fld   _t_fxfl"                           \
	"     jmp   rsqrt_exit"                        \
	"rsqrt_psqr:"                                  \
	"     add   ecx,0x800000"                      \
	"     or    ecx,edx"                           \
	"     mov   _t_fxfl,ecx"                       \
	"     fld   _t_fxfl"                           \
	"rsqrt_exit:"                                  \
    parm caller [eax]                             \
modify [eax ecx edx];

#endif

#ifdef WIN32
template <typename MATRIX, typename VECTOR>
inline void MatrixXVector(MATRIX M, VECTOR *U, VECTOR *V)
{

	V->x = M[0][0] * U->x + M[0][1] * U->y + M[0][2] * U->z;
	V->y = M[1][0] * U->x + M[1][1] * U->y + M[1][2] * U->z;
	V->z = M[2][0] * U->x + M[2][1] * U->y + M[2][2] * U->z;

	//__asm {
	//	mov	EDX, [M]
	//		mov ESI, [U]
	//		mov EDI, [V]
	//		FLD   DWORD PTR [ESI]
	//		FMUL  DWORD PTR [EDX]
	//		FLD   DWORD PTR [ESI]
	//		FMUL  DWORD PTR [EDX+12]
	//		FLD   DWORD PTR [ESI]
	//		FMUL  DWORD PTR [EDX+24]
	//		FLD   DWORD PTR [ESI+4]
	//		FMUL  DWORD PTR [EDX+4]
	//		FLD   DWORD PTR [ESI+4]
	//		FMUL  DWORD PTR [EDX+16]
	//		FLD   DWORD PTR [ESI+4]
	//		FMUL  DWORD PTR [EDX+28]
	//		FXCH  ST(2)
	//		FADDP ST(5),ST(0)
	//		FADDP ST(3),ST(0)
	//		FADDP ST(1),ST(0)
	//		FLD   DWORD PTR [ESI+8]
	//		FMUL  DWORD PTR [EDX+8]
	//		FLD   DWORD PTR [ESI+8]
	//		FMUL  DWORD PTR [EDX+20]
	//		FLD   DWORD PTR [ESI+8]
	//		FMUL  DWORD PTR [EDX+32]
	//		FXCH  ST(2)
	//		FADDP ST(5),ST(0)
	//		FADDP ST(3),ST(0)
	//		FADDP ST(1),ST(0)
	//		FXCH  ST(1)
	//		FXCH  ST(2)
	//		FSTP  DWORD PTR [EDI]
	//		FSTP  DWORD PTR [EDI+8]
	//		FSTP  DWORD PTR [EDI+4]
	//}
}

inline void Vector_Add(Vector *U,Vector *V,Vector *W)
{
	W->x = U->x + V->x;
	W->y = U->y + V->y;
	W->z = U->z + V->z;
	//__asm {
	//	mov esi, [U]
	//		mov edx, [V]
	//		mov edi, [W]
	//		FLD   DWORD PTR [ESI]
	//		FADD  DWORD PTR [EDX]
	//		FLD   DWORD PTR [ESI+4]
	//		FADD  DWORD PTR [EDX+4]
	//		FLD   DWORD PTR [ESI+8]
	//		FADD  DWORD PTR [EDX+8]
	//		FXCH  ST(2)
	//		FSTP  DWORD PTR [EDI]
	//		FSTP  DWORD PTR [EDI+4]
	//		FSTP  DWORD PTR [EDI+8]
	//}
}

inline void Vector_Sub(Vector *U,Vector *V,Vector *W)
{
	W->x = U->x - V->x;
	W->y = U->y - V->y;
	W->z = U->z - V->z;
	//__asm {
	//	mov esi, [U]
	//		mov edx, [V]
	//		mov edi, [W]
	//		FLD   DWORD PTR [ESI]
	//		FSUB  DWORD PTR [EDX]
	//		FLD   DWORD PTR [ESI+4]
	//		FSUB  DWORD PTR [EDX+4]
	//		FLD   DWORD PTR [ESI+8]
	//		FSUB  DWORD PTR [EDX+8]
	//		FXCH  ST(2)
	//		FSTP  DWORD PTR [EDI]
	//		FSTP  DWORD PTR [EDI+4]
	//		FSTP  DWORD PTR [EDI+8]
	//}
}

inline void FPU_LPrecision()
{
	__asm {
		// FPU control word
		FNStCW  [OldFPUCW]
		Mov     ax,[OldFPUCW]
		And     eax,1111101111111111b
		Or      eax,0000100000000000b
		And     eax,1111110011111111b            
		Mov     [FPUCW],ax        
		FLdCW   [FPUCW]

		// SSE control word
		STMXCSR	 [OldMXCSR];      RCMSKBIT EXPBIT
		mov eax, [OldMXCSR];  8765432109876543210
		and eax, 11111111111111110000000001000000b
		or  eax, 00000000000000001101111111000000b
		mov [CurMXCSR], eax
		LDMXCSR  [CurMXCSR]
	}
}


inline void FPU_NormalState()
{
	__asm {
		FLdCW   [OldFPUCW]
	}
}

inline void RoundToInt(long *dest, float val)
{
	*dest = static_cast<long>(val);
	//__asm {
	//	fld dword ptr [val]
	//		mov eax, [dest]
	//		fistp [eax]
	//}
}

inline void RoundToDW(DWord *dest, float val)
{
	*dest = static_cast<DWord>(val);
	//__asm {
	//	fld dword ptr [val]
	//		mov eax, [dest]
	//		fistp [eax]
	//}
}


inline float SQRT(float Value)
{
	return sqrtf(Value);
	//__asm {
	//	mov eax, dword ptr [Value]
	//		mov   ecx,eax
	//		and   eax,0xffffff
	//		shr   ecx,23
	//		xor   eax,0x800000
	//		shr   eax,12
	//		mov   edx,[eax*4+_t_sqrttab]
	//		mov   ecx,[ecx*4+_t_exptab] 
	//		or    ecx,edx
	//		mov   _t_fxfl,ecx
	//}
	//return _t_fxfl;
}

/*ħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħħ*/
// 1.0 / sqrt - fast reciprocal square root (to within 5 digits)

inline float RSQRT(float Value)
{
//	return 1.0f / sqrt(Value);
	float &val = Value;
	float retval;
	__asm
	{
		//mov edi, [val]
		rsqrtss xmm0, [val]
		movss [retval], xmm0
	}
	return retval;

//	__asm {
//		mov eax, dword ptr [Value]
//			mov   ecx,eax
//			and   eax,0xffffff
//			shr   ecx,23
//			xor   eax,0x800000
//			shr   eax,12
//			mov   edx,[eax*4+_t_rsqrttab]
//			mov   ecx,[ecx*4+_t_rexptab]
//			test  edx,edx
//			jz    rsqrt_psqr
//			or    ecx,edx
//			mov   _t_fxfl,ecx
//			jmp   rsqrt_exit
//rsqrt_psqr:
//        add   ecx,0x800000
//			or    ecx,edx
//			mov   _t_fxfl,ecx        
//rsqrt_exit:
//	}
//	return _t_fxfl;
}

inline void Fist(word &r, float x)
{	
	__asm 
	{
		mov edi, [r]
		FLD dword ptr x
		FISTP word ptr [edi]
	}
}

inline void Fist(sword &r, float x)
{	
	__asm 
	{
		mov edi, [r]
		FLD dword ptr x
		FISTP word ptr [edi]
	}
}

inline void Fist(dword &r, float x)
{	
	__asm 
	{
		mov edi, [r]
		FLD dword ptr x
		FISTP dword ptr [edi]
	}
}

inline dword Fist(float x)
{
	static dword ceiled;
	__asm 
	{
		FLD   dword ptr [x]		
		FISTP dword ptr [ceiled]
		;mov eax, [ceiled]
	}
	return ceiled;
}

// stores 'data' n times starting from address p
// (p[0...n-1] = data)
inline void FastWrite(void *p, dword data, dword n)
{
	__asm
	{
		mov edi, [p]
		mov eax, data
		mov ecx, n

		cmp ecx, 16
		jge optver
		rep stosd
		jmp afterfill
		; setup MMX register
optver:
		movd MM0, data
		movq MM1, MM0
		punpckldq MM0, MM1

		lea edi, [edi+ecx*4]
		neg ecx

		; align write
		test edi, 4
		jz afterprior

		mov [edi+ecx*4], eax
		inc ecx

afterprior:
		mov edx, ecx

		sar ecx, 1 ; # qwords
		and edx, 1 ; remainder dwords
		
quads:
		;movntq [edi+ecx*8], MM0
		__asm _emit 0x0F
		__asm _emit 0xE7
		__asm _emit 0x04
		__asm _emit 0xCF
		inc ecx
		jnz quads

		and edx,edx
		jz afterpost
		mov [edi-4], eax
afterpost:
		emms
afterfill:
	}
}

inline void rcpss(float *s)
{
	__asm
	{
		mov edi, [s]
		rcpss xmm0, [edi]
		movss [edi], xmm0
	}
}


#endif

#pragma pack (pop)
#pragma pack (pop)
#pragma pack (pop)

#endif //FDS_VARS_H_INCLUDED
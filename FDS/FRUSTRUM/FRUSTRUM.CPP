#include <stdio.h>
#include <stdlib.h>

#include <Base/FDS_VARS.H>
#include <Base/FDS_DECS.H>
#include <Base/FDS_DEFS.H>

#include <Base/Scene.h>

#define ClipAccurate
#define COUNT_POLYS
//#define NO_MIPMAPS

float FOVX,FOVY;
float C_FZP,C_rFZP, C_NZP, C_rNZP;

dword g_MipLevel;
dword g_renderedPolys;


///////////////////////////////////////////////////////////////////////////////
// Third Generation Triangle -> Convex counterclockwise NGon Frustrum clipper.
// Features:
// Based on the usual Hodgeman-Sutherland polygon clipping algorithm.
// Cuts polygons in parts so texture-mappers can assume input polygons to have 
// a constant mipmapping level across them. this also allevates for affine gouraud
// interpolation errors.
// Version 1.0
// Log
// 30.03.03 project opened
//
// Notes
// (Twosided support): Vertices may not be in counterclockwise order at first.
// Order swapping is executed after Z-clipping.
// The CW/CCW check before vertex order swapping can be removed if 
// backface culling test will store its result in the face flags.

#define ENABLE_PIXELCOUNT

struct Viewport
{
	float ClipX1, ClipX2, ClipY1, ClipY2;
	float NearZ, FarZ;
	float iNearZ, iFarZ;
};

#define CLIPPER_MAXVERTS 48

static Viewport C_VP;
static mword C_numVerts;
static Vertex C_Verts[CLIPPER_MAXVERTS];
static Vertex *C_Ptr[3][CLIPPER_MAXVERTS];
static Vertex **C_Prim, **C_Scnd, **C_Tetr;

static Vertex *_IA,*_IB;
static float _IVal;
static dword C_Flags;


#define LOGTABLEPREC 8
#define EXPTABLEPREC 8
#define EXPTABLEMASK 0x000000FF

static float LogTable[1<<LOGTABLEPREC];
static float ExpTable[(1<<EXPTABLEPREC)+1];

float fastLog2(float x)
{
//	static const float b2 = 1.0 / log(2.0);
//	return log(x)*b2;
	sdword ix = *(sdword *)&x;
	return LogTable[(ix & 0x7f8000) >> (23-LOGTABLEPREC)] + ((ix  >> 23) - 127);
}

float fastPow2(float x)
{
//	static const float b2 = log(2.0);
//	return exp(b2*x);
	static const float ETP = (1 << EXPTABLEPREC);
	// generate pow2 LUT
	long i = Fist(x * ETP);
	union {
		float e;
		long le;
	};
	e = ExpTable[i & EXPTABLEMASK];

	// 3.5 cycles faster on P-4!!!!!!!!
	//le += (i & (~((1<<EXPTABLEPREC)-1)) ) << (23-EXPTABLEPREC);

	le += (i >> EXPTABLEPREC) << 23;
	return e;
}

static float invTable[32] = {0.0,
	2.0/1.0,  2.0/2.0,  2.0/3.0,  2.0/4.0,
	2.0/5.0,  2.0/6.0,  2.0/7.0,  2.0/8.0,
	2.0/9.0,  2.0/10.0, 2.0/11.0, 2.0/12.0,
	2.0/13.0, 2.0/14.0, 2.0/15.0, 2.0/16.0,
	2.0/17.0, 2.0/18.0, 2.0/19.0, 2.0/20.0,
	2.0/21.0, 2.0/22.0, 2.0/23.0, 2.0/24.0,
	2.0/25.0, 2.0/26.0, 2.0/27.0, 2.0/28.0,
	2.0/29.0, 2.0/30.0, 2.0/31.0, 
};


void InitFrustrumClipper()
{
	dword i;
	static const float rl2 = 1.0 / log(2.0);

	memset(C_Verts, 0, sizeof(C_Verts));

	C_Prim = C_Ptr[0];
	C_Scnd = C_Ptr[1];
	C_Tetr = C_Ptr[2];

	union
	{
		float x;
		dword ix;
	};
	// generate log2 LUT
	for (i = 0; i < 1 << LOGTABLEPREC; i++)
	{
		ix = (127 << 23) + (i << (23-LOGTABLEPREC));
		LogTable[i] = log(x) * rl2;
	}

	static const float b2 = log(2.0);
	static const float rETP = 1.0f / (1 << EXPTABLEPREC);
	// generate pow2 LUT
	for (i = 0; i <= 1 << EXPTABLEPREC; i++)
	{
		x = i * rETP;
		ExpTable[i] = exp(b2*x);
	}
	
	for(i=0;i<10; i++)
	{
		x = rand() / 391.241;

		float l_acc = log(x) * rl2;
		float l_tbl = fastLog2(x);

		l_acc = exp(x * b2);
		l_tbl = fastPow2(x);
	}
}

void InitClipperViewport(Scene *Sc)
{
	C_VP.NearZ = Sc->NZP;
	C_VP.FarZ = Sc->FZP;
	C_VP.iNearZ = 1.0 / Sc->NZP;
	C_VP.iFarZ = 1.0 / Sc->FZP;

	// scene should have a flag for modifyable clipping bounds.
	C_VP.ClipX1 = 0.0;
	C_VP.ClipX2 = XRes;
	C_VP.ClipY1 = 0.0;
	C_VP.ClipY2 = YRes-1;
}
static inline void Calc_Flags(Vertex *V)
{
	if (V->PX<C_VP.ClipX1) V->Flags = Vtx_VisLeft; else V->Flags = 0;
	if (V->PX>C_VP.ClipX2) V->Flags |= Vtx_VisRight;
	if (V->PY<C_VP.ClipY1) V->Flags |= Vtx_VisUp;
	if (V->PY>C_VP.ClipY2) V->Flags |= Vtx_VisDown;
	C_Flags |= V->Flags;
}

static inline void Calc_YFlags(Vertex *V)
{
	if (V->PY<C_VP.ClipY1) V->Flags = Vtx_VisUp; else V->Flags = 0;
	if (V->PY>C_VP.ClipY2) V->Flags |= Vtx_VisDown;
	C_Flags |= V->Flags;
}

static inline void ClipperSwap()
{
	C_Scnd[C_numVerts] = C_Scnd[0];

	Vertex **Swap = C_Prim;
	C_Prim = C_Scnd;
	C_Scnd = Swap;
}

static void FClipper_Near()
{
	dword i,j = 0;
	_IB = C_Prim[0];

	float t;
	Vertex *newVert = C_Verts;
	for(i=0; i<C_numVerts;i++)
	{
		_IA = _IB;
		_IB = C_Prim[i+1];
		if  (_IA->Flags & Vtx_VisNear)
		{
			if (_IB->Flags & Vtx_VisNear) continue;
		} else {
			C_Scnd[j++] = _IA;
			if (!(_IB->Flags & Vtx_VisNear)) continue;
		}
		t = (C_VP.NearZ - _IA->TPos.z) / (_IB->TPos.z - _IA->TPos.z);
		
		newVert->TPos.x = _IA->TPos.x + t * (_IB->TPos.x - _IA->TPos.x);
		newVert->TPos.y = _IA->TPos.y + t * (_IB->TPos.y - _IA->TPos.y);
		newVert->TPos.z = C_VP.NearZ;
		newVert->LR = _IA->LR + t * (_IB->LR - _IA->LR);
		newVert->LG = _IA->LG + t * (_IB->LG - _IA->LG);
		newVert->LB = _IA->LB + t * (_IB->LB - _IA->LB);
		newVert->U = _IA->U + t * (_IB->U - _IA->U);
		newVert->V = _IA->V + t * (_IB->V - _IA->V);

		newVert->PX = newVert->TPos.x * C_VP.iNearZ;
		newVert->PY = newVert->TPos.y * C_VP.iNearZ;
		newVert->RZ = C_VP.iNearZ;
		newVert->UZ = newVert->U * C_VP.iNearZ;
		newVert->VZ = newVert->V * C_VP.iNearZ;

		Calc_Flags(newVert);
		C_Scnd[j++] = newVert++;
	}

	C_numVerts = j;
	ClipperSwap();
}

static void FClipper_Far()
{
	dword i,j = 0;
	_IB = C_Prim[0];

	float t;
	Vertex *newVert = C_Verts+2;
	for(i=0;i<C_numVerts;i++)
	{
		_IA = _IB;
		_IB = C_Prim[i+1];
		if  (_IA->Flags & Vtx_VisFar)
		{
			if (_IB->Flags & Vtx_VisFar) continue;
		} else {
			C_Scnd[j++] = _IA;
			if (!(_IB->Flags & Vtx_VisFar)) continue;
		}
		t = (C_VP.FarZ-_IA->TPos.z)/(_IB->TPos.z-_IA->TPos.z);		
		newVert->PX = C_VP.iFarZ * (_IA->TPos.x + t * (_IB->TPos.x-_IA->TPos.x) );
		newVert->PY = C_VP.iFarZ * (_IA->TPos.y + t * (_IB->TPos.y-_IA->TPos.y) );			
		newVert->LR = _IA->LR + t * (_IB->LR-_IA->LR);
		newVert->LG = _IA->LG + t * (_IB->LG-_IA->LG);
		newVert->LB = _IA->LB + t * (_IB->LB-_IA->LB);
		newVert->TPos.z = C_VP.FarZ;
		newVert->RZ = C_VP.iFarZ;
		newVert->U = _IA->U + t * (_IB->U-_IA->U);
		newVert->V = _IA->V + t * (_IB->V-_IA->V);
		newVert->UZ = C_VP.iFarZ * newVert->U;
		newVert->VZ = C_VP.iFarZ * newVert->V;
		Calc_Flags(newVert);
		C_Scnd[j++] = newVert++;
	}

	C_numVerts = j;
	ClipperSwap();
}

// one day, with the appropriate technology, this will use functions generated by code
static void FInterpolator(Vertex *V, float t)
{
	V->PX = _IA->PX + t * (_IB->PX - _IA->PX);
	V->PY = _IA->PY + t * (_IB->PY - _IA->PY);
	V->LR = _IA->LR + t * (_IB->LR - _IA->LR);
	V->LG = _IA->LG + t * (_IB->LG - _IA->LG);
	V->LB = _IA->LB + t * (_IB->LB - _IA->LB);
	V->RZ = _IA->RZ + t * (_IB->RZ - _IA->RZ);
	V->UZ = _IA->UZ + t * (_IB->UZ - _IA->UZ);
	V->VZ = _IA->VZ + t * (_IB->VZ - _IA->VZ);

	// calculate (U,V)
	V->TPos.z = 1.0 / V->RZ;
	V->U = V->UZ * V->TPos.z;
	V->V = V->VZ * V->TPos.z;
}

static void FClipper_Left()
{
	dword i, j = 0;
	_IB = C_Prim[0];

	Vertex *newVert = C_Verts+4;
	for(i=0; i<C_numVerts; i++)
	{
		_IA = _IB;
		_IB = C_Prim[i+1];
		if  (_IA->Flags & Vtx_VisLeft)
		{
			if (_IB->Flags & Vtx_VisLeft) continue;
		} else {
			C_Scnd[j++] = _IA;
			if (!(_IB->Flags & Vtx_VisLeft)) continue;
		}
		FInterpolator(newVert, (C_VP.ClipX1-_IA->PX)/(_IB->PX-_IA->PX));
		newVert->PX = C_VP.ClipX1;
		Calc_YFlags(newVert);
		C_Scnd[j++] = newVert++;
	}

	C_numVerts = j;
	ClipperSwap();
}

static void FClipper_Right()
{
	dword i, j = 0;
	_IB = C_Prim[0];

	Vertex *newVert = C_Verts+6;
	for(i=0; i<C_numVerts; i++)
	{
		_IA = _IB;
		_IB = C_Prim[i+1];
		if  (_IA->Flags & Vtx_VisRight)
		{
			if (_IB->Flags & Vtx_VisRight) continue;
		} else {
			C_Scnd[j++] = _IA;
			if (!(_IB->Flags & Vtx_VisRight)) continue;
		}
		FInterpolator(newVert, (C_VP.ClipX2-_IA->PX)/(_IB->PX-_IA->PX));
		newVert->PX = C_VP.ClipX2;
		Calc_YFlags(newVert);
		C_Scnd[j++] = newVert++;
	}

	C_numVerts = j;
	ClipperSwap();
}

static void FClipper_Up()
{
	dword i, j = 0;
	_IB = C_Prim[0];

	Vertex *newVert = C_Verts+8;
	for(i=0; i<C_numVerts; i++)
	{
		_IA = _IB;
		_IB = C_Prim[i+1];
		if  (_IA->Flags & Vtx_VisUp)
		{
			if (_IB->Flags & Vtx_VisUp) continue;
		} else {
			C_Scnd[j++] = _IA;
			if (!(_IB->Flags & Vtx_VisUp)) continue;
		}
		FInterpolator(newVert, (C_VP.ClipY1-_IA->PY)/(_IB->PY-_IA->PY));
		newVert->PY = C_VP.ClipY1;
		C_Scnd[j++] = newVert++;
	}

	C_numVerts = j;
	ClipperSwap();
}

static void FClipper_Down()
{
	dword i, j = 0;
	_IB = C_Prim[0];

	Vertex *newVert = C_Verts+10;
	for(i=0; i<C_numVerts; i++)
	{
		_IA = _IB;
		_IB = C_Prim[i+1];
		if  (_IA->Flags & Vtx_VisDown)
		{
			if (_IB->Flags & Vtx_VisDown) continue;
		} else {
			C_Scnd[j++] = _IA;
			if (!(_IB->Flags & Vtx_VisDown)) continue;
		}
		FInterpolator(newVert, (C_VP.ClipY2-_IA->PY)/(_IB->PY-_IA->PY));
		newVert->PY = C_VP.ClipY2;
		C_Scnd[j++] = newVert++;
	}

	C_numVerts = j;
	ClipperSwap();
}


static void CorrectCWOrder()
{
	Vertex *A = C_Prim[0], *B = C_Prim[1], *C = C_Prim[2];
	
	float area = (B->PX - A->PX) * (C->PY - A->PY) - (C->PX - A->PX) * (B->PY - A->PY);

	if (area > 0.0)
	{
		for(mword i=0; i<C_numVerts; i++)
		{
			C_Scnd[i] = C_Prim[C_numVerts-i];
		}
		ClipperSwap();
	}
}

static void YSort(Vertex **Prim, Vertex **Scnd, mword nVerts)
{
	// Find vertex with minimal y-value.
	mword i, j;
	float minY = Prim[0]->PY;
	mword minYIndex = 0;
	for (i=1; i<nVerts; i++)
	{
		float y = Prim[i]->PY;
		if (y < minY)
		{
			minYIndex = i;
			minY = y;
		}
	}
	
	// rotate vertices so that minimal y vertex appears first
	for(i=minYIndex; i<nVerts; i++)
	{
		Scnd[i-minYIndex] = Prim[i];
	}
	
	j = nVerts - minYIndex;
	for(i=0; i<minYIndex; i++)
	{
		Scnd[i+j] = Prim[i];
	}

}

void MiplevelClipper()
{
	const float mipBias = 0.5;

	Texture *Tx = DoFace->Txtr->Txtr;
	long L = Tx->numMipmaps-1; // maximal level	

	// polygon texel / pixel area
	float pixArea = 0.0f, texArea = 0.0f;

	Vertex *A = C_Prim[0], *B, *C;
	mword i, j;
	for(i=2; i<C_numVerts; i++)
	{
		B = C_Prim[i-1];
		C = C_Prim[i];
	
		pixArea += (C->PX - A->PX) * (B->PY - A->PY) - (B->PX - A->PX) * (C->PY - A->PY);
		texArea += (C->U  - A->U ) * (B->V  - A->V ) - (B->U  - A->U ) * (C->V  - A->V );
	}

#ifdef ENABLE_PIXELCOUNT
	FillerPixelcount += 0.5f * fabs(pixArea);
#endif
	// Simple miplevel calculation
	//if (!Keyboard[ScY])
	static float MinSize = XRes*YRes * 0.02; // update if XRes and/or YRes changes...

	if ((Tx->Flags & Txtr_Nomip) ||  pixArea < MinSize)
	{
		if (Tx->Flags & Txtr_Nomip) {
			g_MipLevel = 0;
		} else {
			long mip = 0.5 * fastLog2(fabs(texArea * Tx->SizeX * Tx->SizeY / pixArea)) + mipBias;
			if (mip < 0) mip = 0;
			if (mip > L) mip = L;
			g_MipLevel = mip;
		}

#ifdef COUNT_POLYS
		g_renderedPolys++;
#endif
#ifdef NO_MIPMAPS
		g_MipLevel = 0;
#endif
		DoFace->Filler(C_Prim, C_numVerts);

		return;
	}

	// mipmap level constant
	float q = 0;
	for(i=0; i<C_numVerts; i++)
	{
		q += fastLog2(C_Prim[i]->RZ);
	}
	
	float _C = 0.5 * fastLog2(fabs(texArea * Tx->SizeX * Tx->SizeY / pixArea)) + invTable[C_numVerts] * q + mipBias;

	// calculate range of 1/z values along polygon
	float mrz = 1E+38, Mrz = 0;
	long mlindex = 0;
	for(i=0; i<C_numVerts; i++)
	{
//		level = _C - 2 * log2(rz);
		float rz = C_Prim[i]->RZ;

		if (rz<mrz) 
		{
			mrz = rz;
		}

		if (rz>Mrz)
		{
			Mrz = rz;
			mlindex = i;
		}
	}
	
	// calculate mipmapping level range
	// assuming rounding state = ceil
	float ml = _C - 2 * fastLog2(Mrz);
	float Ml = _C - 2 * fastLog2(mrz);

	long iml = Fist(ml-0.99999);
	long iMl = Fist(Ml-0.99999);

	if (iml < 0) iml = 0;
	if (iml > L) iml = L;
	if (iMl < 0) iMl = 0;
	if (iMl > L) iMl = L;


	// simple case (uniform mipmap across polygon)
	// should also include all polygons with reletively small onscreen area, because
	// polysplitting is REALLY slow.
	if (iml == iMl)
	{
		g_MipLevel = iml;
		if (g_MipLevel < 0) g_MipLevel = 0;
#ifdef COUNT_POLYS
		g_renderedPolys++;
#endif
#ifdef NO_MIPMAPS
		g_MipLevel = 0;
#endif
		DoFace->Filler(C_Prim, C_numVerts);
		return;
	}

	// scanconversion
	long left, right, limit;

	float t;
	mword numSectionVerts;
	Vertex *newVert = C_Verts + 12;

	left = right = mlindex;

	// build initial section	
	mword numRight = 0; // notice: Right vertex #0 is always ignored
	numSectionVerts = 0;

	static const float h_log_2 = 0.5 * log(2.0);
	static const float limrz_mult = 1.0 / sqrt(2.0);
	//float limrz = exp(h_log_2*(_C - iml));
	float limrz = fastPow2(0.5*(_C - iml));

	for(limit = iml+1;limit <= iMl; limit++)
	{
		// calculate crossover 1/z
		limrz *= limrz_mult;

		if (limrz >= Mrz) continue;
		if (limrz <= mrz) break;

		// traverse left
		while(C_Prim[left]->RZ > limrz)
		{
			C_Scnd[numSectionVerts++] = C_Prim[left];
			if (left == C_numVerts) left = 0;
			left++;			
		}

		// traverse right
		while(C_Prim[right]->RZ > limrz)
		{
			C_Tetr[numRight++] = C_Prim[right];
			if (right == 0) right = C_numVerts;
			right--;
		}

		// intersect left
		t = (limrz - C_Prim[left-1]->RZ) / (C_Prim[left]->RZ - C_Prim[left-1]->RZ);

		_IA = C_Prim[left-1];
		_IB = C_Prim[left];
		FInterpolator(newVert, t);
		C_Scnd[numSectionVerts++] = newVert;
		newVert++;
		
		// intersect right
		t = (limrz - C_Prim[right+1]->RZ) / (C_Prim[right]->RZ - C_Prim[right+1]->RZ);

		_IA = C_Prim[right+1];
		_IB = C_Prim[right];
		FInterpolator(newVert, t);
		C_Scnd[numSectionVerts++] = newVert;
		newVert++;

		// write vertices from right edge traversal in reverse order
		for(j = 1; j<numRight; j++)
			C_Scnd[numSectionVerts++] = C_Tetr[numRight-j];

		g_MipLevel = limit-1;
		YSort(C_Scnd, C_Tetr, numSectionVerts);
#ifdef COUNT_POLYS
		g_renderedPolys++;
#endif
#ifdef NO_MIPMAPS
		g_MipLevel = 0;
#endif
		DoFace->Filler(C_Tetr, numSectionVerts);

		// prepare new section - add last two intersection points
		numRight = 1; // vertex #0 is ignored
		// two earlier intersect points in reverse order
		C_Scnd[0] = newVert - 1;
		C_Scnd[1] = newVert - 2;
		numSectionVerts = 2;
	}

	// generate/draw last polygon section
	g_MipLevel = iMl;

	if (right == 0) right = C_numVerts;
	if (left == mlindex)
	{
#ifdef COUNT_POLYS
		g_renderedPolys++;
#endif
#ifdef NO_MIPMAPS
		g_MipLevel = 0;
#endif
		DoFace->Filler(C_Prim, C_numVerts);
		return;
	}
	while (left != right)
	{
		C_Scnd[numSectionVerts++] = C_Prim[left];
		if (left == C_numVerts) left = 0;
		left++;			
	}
	C_Scnd[numSectionVerts++] = C_Prim[left];
	YSort(C_Scnd, C_Tetr, numSectionVerts);
#ifdef COUNT_POLYS
	g_renderedPolys++;
#endif
#ifdef NO_MIPMAPS
		g_MipLevel = 0;
#endif
	DoFace->Filler(C_Tetr, numSectionVerts);
}

float PolyArea()
{
	// polygon pixel area
	float pixArea = 0.0f;

	Vertex *A = C_Prim[0], *B, *C;
	mword i, j;
	for(i=2; i<C_numVerts; i++)
	{
		B = C_Prim[i-1];
		C = C_Prim[i];
	
		pixArea += (C->PX - A->PX) * (B->PY - A->PY) - (B->PX - A->PX) * (C->PY - A->PY);
	}

	return fabs(pixArea) * 0.5f;
}


void FrustrumClipper()
{
	mword i, j;

	// Assign polygon
	C_numVerts = 3;
	C_Prim[0] = DoFace->A;
	C_Prim[1] = DoFace->B;
	C_Prim[2] = DoFace->C;
	C_Prim[3] = DoFace->A;
		
	C_Flags = C_Prim[0]->Flags|C_Prim[1]->Flags|C_Prim[2]->Flags;

	if (C_Flags&Vtx_VisNear)  FClipper_Near(); 
	if (C_Flags&Vtx_VisFar)   FClipper_Far();

	// Z-Clipping may have placed the entire polygon outside 2D boundary
	/*j = C_Prim[0]->Flags;
	for(i=1; i<C_numVerts; i++)
		j &= C_Prim[i]->Flags;
	if (j) return;*/

	// Swap vertex order if it isn't CCW.	
	CorrectCWOrder();

	if (C_Flags&Vtx_VisLeft)  FClipper_Left();
	if (C_Flags&Vtx_VisRight) FClipper_Right();
	if (C_Flags&Vtx_VisUp)    FClipper_Up();
	if (C_Flags&Vtx_VisDown)  FClipper_Down();

	// Clipping process may have eliminated the entire polygon
	if (!C_numVerts) return;

	YSort(C_Prim, C_Scnd, C_numVerts);
	
	// triangulate result NGon and sort triangle vertices by Y
	// to conform to legacy rasterizer convension

	if (DoFace->Txtr->Txtr)
	{
		ClipperSwap();
		MiplevelClipper();
	} else {
#ifdef ENABLE_PIXELCOUNT
		FillerPixelcount += PolyArea();
#endif
#ifdef COUNT_POLYS
		g_renderedPolys++;
#endif		
		DoFace->Filler(C_Scnd, C_numVerts);
	}
	/*for(i=2; i<C_numVerts; i++)
	{
		if (C_Scnd[i-1]->PY < C_Scnd[i]->PY)
			DoFace->Filler(C_Scnd[0], C_Scnd[i-1], C_Scnd[i]);
		else
			DoFace->Filler(C_Scnd[0], C_Scnd[i], C_Scnd[i-1]);
	}*/
}

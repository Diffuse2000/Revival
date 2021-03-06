#include "SkyCube.h"
#include "Base/Object.h"
#include "Base/TriMesh.h"
#include "Base/Scene.h"

static void GenerateSkyTexture(Texture *Tx, long numStars)
{
/*	Vector V;

	dword *Txd = (dword *)Tx->Data;

	const float K = 0.01;
	float y =-PI/4.0f;
	dword i, size;

	// f'(x) = 1 / cos f(x).

	for(i=0;y<PI/4.0f;i++)
	{
		y += K / cos(y);
	}

	size = i;
	float *F = new float [size+1];
	y =-PI/4.0f;
	for(i=0;i<=size; i++)
	{
		F[i] = y;
		y += K / cos(y);
	}

	while (numStars--)
	{
		float x = rand() * (size-1) / 32768.0;

		int ix = x;
		float fx = x-ix;
		float beta = F[ix] * (1.0-fx) + F[ix+1] * fx;

		Vector V;
		V.x = 1.0;
		V.y = tan(beta);
		V.z = (rand()-16384) / 16384.0;

		// texture is placed at X=1 plane
		if (fabs(V.y) >= V.x || fabs(V.z) >= V.x)
		{
			int kaka = 1;
		}
		V.y /= V.x;
		V.z /= V.x;

		long TX = V.z * 127.99 + 128.0;
		long TY = V.y * 127.99 + 128.0; 
		Txd[TX + TY*256] = 0x00FFFFFF;

	}

	delete [] F;
	int banana = 1;*/
	Image Im;
	New_Image(&Im, 256, 256);
	memset(Im.Data, 0, 256*256*sizeof(dword));
	Generate_Plasma(&Im, 1000, 3851);
	Convert_Image2Texture(&Im, Tx);
}

Scene * CreateSkyCube(dword skyType)
{
	Scene *Sc = (Scene *)getAlignedBlock(sizeof(Scene), 16);
	memset(Sc, 0, sizeof(Scene));
	Sc->FZP = 200.0;

	TriMesh *T = (TriMesh *)getAlignedBlock(sizeof(TriMesh), 16);
	memset(T,0,sizeof(TriMesh));

	Object *Obj = new Object;
	memset(Obj, 0, sizeof(Object));
	Obj->Name = strdup("Skycube");

	Obj->Next = Sc->ObjectHead;
	if (Sc->ObjectHead) Sc->ObjectHead->Prev = Obj;
	Sc->ObjectHead = Obj;

	Obj->Data = T;
	Obj->Type = Obj_TriMesh;

	T->Next = Sc->TriMeshHead;
	if (Sc->TriMeshHead) Sc->TriMeshHead->Prev = T;
	Sc->TriMeshHead = T;

	// set rotation/position ptrs
	Obj->Rot =&T->RotMat;
	Obj->Pos =&T->IPos;
	Vector_Form(&Obj->Pivot, 0, 0, 0);

	T->VIndex = 24;
	T->Verts = new Vertex[T->VIndex];
	memset(T->Verts, 0, sizeof(Vertex) * T->VIndex);

	T->FIndex = 12;
	T->Faces = new Face [T->FIndex];
	memset(T->Faces, 0, sizeof(Face) * T->FIndex);

	// the normals to each of the cube's faces
	Vector N[6] = {
		Vector( 0.0f, 0.0f, 1.0f),
		Vector(-1.0f, 0.0f, 0.0f),
		Vector( 0.0f, 0.0f,-1.0f),
		Vector( 1.0f, 0.0f, 0.0f),
		Vector( 0.0f, 1.0f, 0.0f),
		Vector( 0.0f,-1.0f, 0.0f)
	};

	// Vertex positions
	Vector P[8] = {
		Vector(-1.0f, 0.0f,-1.0f),
		Vector( 1.0f, 0.0f,-1.0f),
		Vector(-1.0f, 1.0f,-1.0f),
		Vector( 1.0f, 1.0f,-1.0f),
		Vector(-1.0f, 0.0f, 1.0f),
		Vector( 1.0f, 0.0f, 1.0f),
		Vector(-1.0f, 1.0f, 1.0f),
		Vector( 1.0f, 1.0f, 1.0f),
	};

	dword Order[4*6] = {
		7,6,4,5,		
		3,7,5,1,
		2,3,1,0,
		6,2,0,4,
		4,0,1,5,
		6,7,3,2
	};

	Vertex *SV = T->Verts;
	dword *O = Order,o;
	dword i;
	for(i=0;i<6;i++)
	{
		float ratio = 0.5;
		if (((O[0]>>1)&1) ==
			((O[1]>>1)&1) ==
			((O[2]>>1)&1) ==
			((O[3]>>1)&1)) ratio = 1.0;

		o = *O++;
		SV->LR = SV->LG = SV->LB = 127.0;
		SV->Pos.x = P[o].x;
		SV->Pos.y = P[o].y;
		SV->Pos.z = P[o].z;
		SV->U = 0.5f;//256.0f;
		SV->V = 0.5f;//256.0f;
		SV++;
		o = *O++;
		SV->LR = SV->LG = SV->LB = 127.0;
		SV->Pos.x = P[o].x;
		SV->Pos.y = P[o].y;
		SV->Pos.z = P[o].z;
		SV->U = 255.5f;//256.0f;
		SV->V = 0.5f;//256.0f;
		SV++;
		o = *O++;
		SV->LR = SV->LG = SV->LB = 127.0;
		SV->Pos.x = P[o].x;
		SV->Pos.y = P[o].y;
		SV->Pos.z = P[o].z;
		SV->U = 255.5f;//256.0f;
		SV->V = 255.5f*ratio;//256.0f;
		SV++;
		o = *O++;
		SV->LR = SV->LG = SV->LB = 127.0;
		SV->Pos.x = P[o].x;
		SV->Pos.y = P[o].y;
		SV->Pos.z = P[o].z;
		SV->U = 0.5f;//256.0f;
		SV->V = 255.5f*ratio;//256.0f;
		SV++;
	}

	Material *M[6];
	Texture *Tx[6];
	for(i=0; i<6; i++)
	{
		M[i] = (Material *)getAlignedBlock(sizeof(Material), 16);
		memset(M[i], 0, sizeof(Material));
		Tx[i] = new Texture;
		memset(Tx[i], 0, sizeof(Texture));
		M[i]->Flags = Mat_TwoSided;
		M[i]->Txtr = Tx[i];	

		Tx[i]->BPP = 32;
		dword *data = new dword [256*256];
		Tx[i]->Data = (byte *)data;
		memset(Tx[i]->Data, 0, 256*256*4);
		GenerateSkyTexture(Tx[i], 200);
		
		Tx[i]->OptClass = 0;
		Tx[i]->SizeX = 256;
		Tx[i]->SizeY = 256;
	}

	
	T->FIndex = 12;
	Face *F = T->Faces;
	for(i=0; i<6; i++)
	{
		F->A = T->Verts + i*4;
		F->B = T->Verts + i*4 + 1;
		F->C = T->Verts + i*4 + 2;
		F->Txtr = M[i];
//		F->Filler = The_BiTrue;
		//F->Clipper = Frust_Set_UV;
		F->U1 = F->A->U;
		F->U2 = F->B->U;
		F->U3 = F->C->U;
		F->V1 = F->A->V;
		F->V2 = F->B->V;
		F->V3 = F->C->V;
		F++;

		F->A = T->Verts + i*4;
		F->B = T->Verts + i*4 + 2;
		F->C = T->Verts + i*4 + 3;
		F->Txtr = M[i];
//		F->Filler = The_BiTrue;
		//F->Clipper = Frust_Set_UV;
		F->U1 = F->A->U;
		F->U2 = F->B->U;
		F->U3 = F->C->U;
		F->V1 = F->A->V;
		F->V2 = F->B->V;
		F->V3 = F->C->V;
		F++;
	}

	T->Pos.CurKey = 0;
	T->Pos.NumKeys = 1;
	T->Pos.Keys = new SplineKey[1];
	memset(T->Pos.Keys, 0, sizeof(SplineKey));
	Quaternion_Form(&T->Pos.Keys->Pos,0.0,0.0,0.0,0.0f);
	//T->IPos = Vortex_Center;
	T->Scale.CurKey = 0;
	T->Scale.NumKeys = 1;
	T->Scale.Keys = new SplineKey[1];
	Quaternion_Form(&T->Scale.Keys->Pos,100.0f,100.0f,100.0f,0.0f);
	Vector_Form(&T->IScale, 100.0,100.0, 100.0);
	T->Rotate.CurKey = 0;
	T->Rotate.NumKeys = 1;
	T->Rotate.Keys = new SplineKey[1];
	Quaternion_Form(&T->Rotate.Keys->Pos,0.0f,0.0f,0.0f,1.0f);
	Vector_Form(&T->BSphereCtr,0.0f,0.0f,0.0f);
	T->BSphereRad = 10000.0f;

	Matrix_Identity(T->RotMat);

	//Preprocess_Scene(Sc);
	T->Flags = HTrack_Visible;

	return Sc;
}

void RenderSkyCube(Scene *Sc, Camera *Cm)
{
	Scene *PrevCurScene = CurScene;
	Camera *PrevView = View;
	View = Cm;
	CurScene = Sc;

	C_FZP = CurScene->FZP;
	C_rFZP = 1.0f/C_FZP;

	Animate_Objects(Sc);

	Vector PrevViewPos = View->ISource;
	Vector_Zero(&View->ISource);

	Transform_Objects(Sc);
	if (CAll)
	{
		Radix_SortingASM(FList,SList,CAll);
		Render();
	}
	View->ISource = PrevViewPos;

	CurScene = PrevCurScene;
	View = PrevView;
	C_FZP = CurScene->FZP;
	C_rFZP = 1.0f/C_FZP;
}
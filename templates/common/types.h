
#ifndef TYPES_H
#define TYPES_H

typedef char bool;
typedef char s8;
typedef uchar u8;
typedef int16 s16;
typedef uint16 u16;
typedef int16 s16;
typedef int32 s32;
typedef uint32 u32;
typedef int64 s64;
typedef uint64 u64;
typedef hfloat f16;
typedef float f32;
typedef double f64;

typedef struct
{
	u8 A;
	u8 R;
	u8 G;
	u8 B;
}TColor8 <read=TColor8ToString>;

typedef struct
{
	u8 A;
	u8 B;
	u8 G;
	u8 R;
}TColorInverted8 <read=TColorInverted8ToString>;

string TColor8ToString(TColor8& color)
{
	local char buffer[255];
	SPrintf(buffer, "[A: %d R: %d G: %d B: %d]", color.A, color.R, color.G, color.B);
	return buffer;
}

string TColorInverted8ToString(TColorInverted8& color)
{
	local char buffer[255];
	SPrintf(buffer, "[A: %d R: %d G: %d B: %d]", color.A, color.R, color.G, color.B);
	return buffer;
}

typedef struct
{
    f32 X;
    f32 Y;
} TVector2 <read=TVector2ToString>;

string TVector2ToString( TVector2& value )
{
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f]", value.X, value.Y );

    return buffer;
}

typedef struct
{
    f16 X;
    f16 Y;
} TVector2Half <read=TVector2HalfToString>;

string TVector2HalfToString( TVector2Half& value )
{
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f]", value.X, value.Y );

    return buffer;
}

typedef struct
{
    f32 X;
    f32 Y;
    f32 Z;
} TVector3 <read=TVector3ToString>;

string TVector3ToString( TVector3& value )
{
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f, %.6f]", value.X, value.Y, value.Z );

    return buffer;
}

typedef struct
{
    f16 X;
    f16 Y;
	f16 Z;
} TVector3Half <read=TVector3HalfToString>;

string TVector3HalfToString( TVector3Half& value )
{
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f, %.6f]", value.X, value.Y, value.Z );

    return buffer;
}

typedef struct
{
    f32 X;
    f32 Y;
    f32 Z;
	f32 W;
} TVector4 <read=TVector4ToString>;

string TVector4ToString( TVector4& value )
{
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f, %.6f, %.6f]", value.X, value.Y, value.Z, value.W );

    return buffer;
}

typedef TVector4 TQuaternion;

typedef struct
{
    f16 X;
    f16 Y;
    f16 Z;
	f16 W;
} TVector4Half <read=TVector4HalfToString>;

string TVector4HalfToString( TVector4Half& value )
{
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f, %.6f, %.6f]", value.X, value.Y, value.Z, value.W );

    return buffer;
}

typedef TVector4Half TQuaternionHalf;

typedef struct
{
	TVector3 Min;
	TVector3 Max;
} TExtents3D <read=TExtents3DToString>;

string TExtents3DToString( TExtents3D& value )
{
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f, %.6f] [%.6f, %.6f, %.6f]", value.Min.X, value.Min.Y, value.Min.Z, value.Max.X, value.Max.Y, value.Max.Z );

    return buffer;
}

typedef TExtents3D TBoundingBox3D;

typedef struct
{
	TVector3 Center;
	f32 Radius;
} TSphere3D <read=TSphere3DToString>;

string TSphere3DToString( TSphere3D& value )
{
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f, %.6f] %.6f]", value.Center.X, value.Center.Y, value.Center.Z, value.Radius );

    return buffer;
}

typedef struct
{
	u32 Value;
} TNormal11_11_11 <read=TNormal11_11_10ToString>;

string TNormal11_11_10ToString( TNormal11_11_11& value )
{
	const int FULL_WIDTH = 32;
	
	const int X_POS = 0;
	const int X_WIDTH = 11;
	const int X_LSHIFT = FULL_WIDTH - X_POS - X_WIDTH;
	const int X_RSHIFT = FULL_WIDTH - X_WIDTH;
	const int X_DOT = (1 << (X_WIDTH - 1)) - 1;
	
	const int Y_POS = X_POS + X_WIDTH;
	const int Y_WIDTH = 11;
	const int Y_LSHIFT = FULL_WIDTH - Y_POS - Y_WIDTH;
	const int Y_RSHIFT = FULL_WIDTH - Y_WIDTH;
	const int Y_DOT = (1 << (Y_WIDTH - 1)) - 1;
	
	const int Z_POS = Y_POS + Y_WIDTH;
	const int Z_WIDTH = 10;	
	const int Z_LSHIFT = FULL_WIDTH - Z_POS - Z_WIDTH;
	const int Z_RSHIFT = FULL_WIDTH - Z_WIDTH;
	const int Z_DOT = (1 << (Z_WIDTH - 1)) - 1;
	
	u32 packed = value.Value;

	int xInt = ((int)(packed << X_LSHIFT)) >> X_RSHIFT;
	int yInt = ((int)(packed << Y_LSHIFT)) >> Y_RSHIFT;
	int zInt = ((int)(packed << Z_LSHIFT)) >> Z_RSHIFT;

	f32 x = (float)xInt / (float)X_DOT;
	f32 y = (float)yInt / (float)Y_DOT;
	f32 z = (float)zInt / (float)Z_DOT;
	
    local char buffer[255];
    SPrintf( buffer, "[%.6f, %.6f, %.6f]", x, y, z );

	return buffer;
}

typedef struct( u32 base )
{
	u32 Offset;
	
	if ( Offset != 0 )
	{
		local u32 returnPos = FTell();
		FSeek( base + Offset );
		string Value;
		FSeek( returnPos );
	}
} TStringOffset <read=TStringOffsetToString>;

string TStringOffsetToString( TStringOffset& value )
{
	if ( value.Offset == 0 ) return "";
    return value.Value;
}

typedef struct
{
    struct
    {
        f32 Column[4];
    } Row[3];
} TMatrix4x3;

typedef struct
{
    struct
    {
        f32 Column[4];
    } Row[4];
} TMatrix4x4;

#endif // #ifndef TYPES_H

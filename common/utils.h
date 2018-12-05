//---------------------------------------------
// Random color
//---------------------------------------------
local u32 __RandomSeed = 0xDEADBABE;
local u32 __RandomBit = 0;
local u32 __RandomCount = 0;
 
u32 MyRandom( u32 to )
{
    ++__RandomCount;
    __RandomBit  = ( (__RandomSeed >> 0 ) ^ ( __RandomSeed >> 2 ) ^ ( __RandomSeed >> 3 ) ^ ( __RandomSeed >> 5 ) ) & 1;
    __RandomSeed = ( ( ( ( __RandomBit << 15 ) | ( __RandomSeed >> 1 ) ) + ( 0xBABE / __RandomCount ) ) % to );

    while( __RandomSeed < 0 )
        __RandomSeed += to;

    return __RandomSeed;
}

u32 RandomColor()
{
    return MyRandom( 0xFFFFFFFF );
}

void SetRandomBackColor()
{
    SetBackColor( RandomColor() );
}

// Generate u32 from FourCC in string format
u32 FourCC( string str )
{
    return (u32)str[0] << 24 | (u32)str[1] << 16 | (u32)str[2] << 8 | (u32)str[3];
}

u32 MakeFourCC( string str )
{
    if ( IsLittleEndian() )
        return (u32)str[0] | (u32)str[1] << 8 | (u32)str[2] << 16 | (u32)str[3] << 24;
    else
        return (u32)str[3] | (u32)str[2] << 8 | (u32)str[1] << 16 | (u32)str[0] << 24;
}

u32 Align( u32 value, u32 alignment )
{
	return (value + (alignment - 1)) & ~(alignment - 1);
}

void FAlign( u32 alignment )
{
	FSeek( Align( FTell(), alignment ) );
}
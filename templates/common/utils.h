
#ifndef UTILS_H
#define UTILS_H

#include "common/types.h"


void PrintValueUInt( string name, u64 value )
{
    PrintValueUIntEx( name, value, true );
}

void PrintValueUIntEx( string name, u64 value, bool newline )
{
    Printf( "%s: %8d (%08X) ", name, value, value );
    if ( newline )
        PrintNl();
}

void PrintValueFloat( string name, f32 value )
{
    PrintValueFloatEx( name, value, true );
}

void PrintValueFloatEx( string name, f32 value, bool newline )
{
    Printf( "%s: %8f ", name, value );
    if ( newline )
        PrintNl();
}

void PrintValueString( string name, string value )
{
    PrintValueStringEx( name, value, true );
}

void PrintValueStringEx( string name, string value, bool newline )
{
    Printf( "%s: %s ", name, value );
    if ( newline )
        PrintNl();
}

void PrintNl()
{
    Printf( "\n" );
}

void PrintArrayUInt( string name, u32 array[], u32 count )
{
    local s32 i;
    local string elementName = "";
    for ( i = 0; i < count; ++i )
    {     
        SPrintf( elementName, "%s[%d]", name, i );
        PrintValueUInt( elementName, array[i] ); 
    }
}

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

local u32 __LastRandomColor = 0;
u32 RandomColor()
{
    return __LastRandomColor = MyRandom( 0xFFFFFFFF );
}

u32 SetRandomBackColor()
{
    local u32 color = RandomColor();
    SetBackColor( color );
    return color;
}

u32 GetShadeOfLastColor(s32 amount)
{    
    local u32 r = (__LastRandomColor & 0x000000FFu) + amount; 
    if (r > 0xFF) r = 0xFF;

    local u32 g = ((__LastRandomColor & 0x0000FF00u) >> 8u) + amount; 
    if (g > 0xFF) g = 0xFF;

    local u32 b = ((__LastRandomColor & 0x00FF0000u) >> 16u) + amount; 
    if (b > 0xFF) b = 0xFF;

    __LastRandomColor = r | g << 8u | b << 16u;  
    return __LastRandomColor;
}

u32 GetLighterShadeOfLastColor()
{
    return GetShadeOfLastColor( 0x10 );
}

u32 SetBackColorToShadeOfLastColor(s32 amount)
{
    local u32 color = GetShadeOfLastColor(amount);
    SetBackColor( color );
    return color;
}

u32 SetBackColorToLighterShadeOfLastColor()
{
    local u32 color = GetShadeOfLastColor(0x10);
    SetBackColor( color );
    return color;
}

// Generate u32 from FourCC in string format
u32 ReadFourCC()
{
    local bool isLittle = IsLittleEndian();

    if (isLittle)
        BigEndian();

    local u32 value = ReadUInt();

    if (isLittle)
        LittleEndian();

    return value;
}

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

local u64 gBasePositionStack[32];
local u32 gBasePositionStackIndex = 0;
local u64 gBasePosition = 0;

local u64 gPositionStack[32];
local u32 gPositionStackIndex = 0;

u64 FTellBase()
{
    return gBasePosition;
}

void FPushBase()
{
    gBasePosition = gBasePositionStack[ ++gBasePositionStackIndex ] = FTell();
    //PrintValueUInt( "Push: gBasePosition", gBasePosition ); 
}

void FPopBase()
{
    gBasePosition = gBasePositionStack[ --gBasePositionStackIndex ];
    //PrintValueUInt( "Pop: gBasePosition", gBasePosition ); 
}

void FPush()
{
    gPositionStack[ ++gPositionStackIndex ] = FTell();
}

void FPushRel( u64 pos )
{
    gPositionStack[ ++gPositionStackIndex ] = FTell();
    FSeekRel( pos );
}

void FPushAbs( u64 pos )
{
    gPositionStack[ ++gPositionStackIndex ] = FTell();
    FSeek( pos );
}

void FPop()
{
    FSeek( gPositionStack[ gPositionStackIndex-- ] );
}

void FSeekRel( u64 pos )
{
    FSeek( gBasePosition + pos );
}

s32 ArrayIndexOfUInt( u32 array[], u32 count, u32 value )
{
    local s32 i;
    for ( i = 0; i < count; ++i )
        if ( array[i] == value )
            return i;

    return -1;
}

bool ArrayContainsUInt( u32 array[], u32 count, u32 value )
{
    return ArrayIndexOfUInt( array, count, value ) != -1;
}

void ArrayFillUInt( u32 array[], u32 count, u32 value )
{
    local u32 i;
    for ( i = 0; i < count; ++i )
        array[i] = value;
}

void ArrayMinUInt( u32 array[], u32 count )
{
    local u32 i;
    local u32 min = 0xFFFFFFFF;

    for ( i = 0; i < count; ++i )
    {
        if ( array[i] < min ) 
            min = array[i];
    }

    return max;
}

void ArrayMaxUInt( u32 array[], u32 count )
{
    local u32 i;
    local u32 max = 0;

    for ( i = 0; i < count; ++i )
    {
        if ( array[i] > max ) 
            max = array[i];
    }

    return max;
}


#endif // #ifndef UTILS_H
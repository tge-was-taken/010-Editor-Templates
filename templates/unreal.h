//------------------------------------------------
//--- 010 Editor v13.0.2 Binary Template
//
//      File: 
//   Authors: 
//   Version: 
//   Purpose: 
//  Category: 
// File Mask: 
//  ID Bytes: 
//   History: 
//------------------------------------------------

local enum EUnrealEngineObjectUE4Version GPackageFileUE4Version = VER_UE4_FIX_WIDE_STRING_CRC;
local enum EUnrealEngineObjectLicenseeUE4Version GPackageFileLicenseeUE4Version = VER_LIC_AUTOMATIC_VERSION;

enum EUnrealEngineObjectUE4Version UE4Ver()
{
    return GPackageFileUE4Version;
}

enum EUnrealEngineObjectLicenseeUE4Version LicenseeUE4Ver()
{
    return GPackageFileLicenseeUE4Version;
}

typedef byte int8;
typedef ubyte uint8;

typedef enum {
    False,    
    True,
} bool;

// CoreMiscDefines.h
enum {INDEX_NONE	= -1				};
enum {UNICODE_BOM   = 0xfeff			};

struct FGuid
{
	uint32 A;
	uint32 B;
	uint32 C;
	uint32 D;
    local bool IsValid = ((A | B | C | D) != 0);
};

typedef struct 
{
    int32 SizeNum;
    local bool bLoadUnicodeChar = SizeNum < 0;
    local int32 ActualSizeNum = SizeNum;
    if (bLoadUnicodeChar) 
    {
        ActualSizeNum = -SizeNum;
    }
    if (ActualSizeNum) 
    {
        if (bLoadUnicodeChar)
        {
            wchar_t Data[ActualSizeNum];
        }
        else
        {
            char Data[ActualSizeNum];
        }
    }
} FString <read=FString_ToString>;

string FString_ToString(FString& x)
{
    if (x.ActualSizeNum) return x.Data;
    return "";
}

// Generic type argument
typedef enum {
    T_Int32 = 1<<0,
    T_FString = 1<<1,
    T_FCustomVersion= 1<<2,
    T_FGenerationInfo= 1<<3,
    T_FCompressedChunk= 1<<4,
    T_FPackageIndex= 1<<5,
    T_FAssetDataTag= 1<<6,
    T_TSetElement = 1<<7,
    T_TPair = 1<<8,
    T_TArray = 1<<9,
    T_FName = 1<<10,
    T_FThumbnailAssetData = 1<<11,
} TType;

// TArray<TElement>
typedef struct(TType TElement) {
    uint32 Num;
    if (Num > 0) {
        switch (TElement) {
            case T_Int32:                   int32                       Data[Num] <optimize=false>; break;
            case T_FString:                 struct FString              Data[Num] <optimize=false>; break;
            case T_FGenerationInfo:         struct FGenerationInfo      Data[Num] <optimize=false>; break;
            case T_FCompressedChunk:        struct FCompressedChunk     Data[Num] <optimize=false>; break;
            case T_FPackageIndex:           struct FPackageIndex        Data[Num] <optimize=false>; break;
            case T_FAssetDataTag:           struct FAssetDataTag        Data[Num] <optimize=false>; break;
            case T_FThumbnailAssetData:     struct FThumbnailAssetData  Data[Num] <optimize=false>; break;
            default: Assert(false); break;
        }
    }
} TArray <optimize=false>;

uint32 DivideAndRoundUp(uint32 Dividend, uint32 Divisor)
{
	return (Dividend + Divisor - 1) / Divisor;
}

typedef struct
{
    int32 NumBits;
    local uint32 NumWords = DivideAndRoundUp(NumBits, 32);
    uint32 Data[NumWords];
} TBitArray;

typedef struct
{
    int32 Index;
} FSetElementId;

// TPair<TLeft,TRight>
typedef struct(TType TLeft, TType TRight)
{
    switch (TLeft)
    {
        case T_FPackageIndex: struct FPackageIndex Left; break;
        default: Assert(false);
    }
   
    if (TRight & T_TArray) {
        TArray Right(TRight & ~T_TArray);
    }
    else
    {
        switch (TRight)
        {
            default: Assert(false);
        }
    }
} TPair;

// TSetElement<TElementType>
typedef struct(TType TElementType)
{
	/** The element's value. */
    switch (TElementType)
    {
        case (T_TPair|T_FPackageIndex|T_TArray|T_FName):
            TPair Value(T_FPackageIndex,T_TArray|T_FName);  
            break;
        default:
            Assert(false);
            break;
    }

	/** The id of the next element in the same hash bucket. */
	FSetElementId HashNextId;

	/** The hash bucket that the element is currently linked to. */
	int32 HashIndex;
} TSetElement <optimize=false>;

// TSparseArray<TElement>
typedef struct(TType TElement)
{
    int32 NumElements;
    if (TElement & T_TSetElement) {
        TSetElement Array(TElement & ~T_TSetElement)[1];
    }
    else
    {
        switch (TElement)
        {
            default:
                Assert(false); 
                break;
        } 
    }
} TSparseArray;

// TSet<TElement>
typedef struct(TType TElement)
{
    TSparseArray Elements(T_TSetElement | TElement);
} TSet;

// TMap<TKey,TValue>
typedef struct(TType TKey, TType TValue)
{
    TSet Pairs(T_TPair | TKey | TValue);
} TMap;

/**
 * Flags describing an object instance
 */
enum EObjectFlags
{
	// Do not add new flags unless they truly belong here. There are alternatives.
	// if you change any the bit of any of the RF_Load flags, then you will need legacy serialization
	RF_NoFlags						= 0x00000000,	///< No flags, used to avoid a cast

	// This first group of flags mostly has to do with what kind of object it is. Other than transient, these are the persistent object flags.
	// The garbage collector also tends to look at these.
	RF_Public					=0x00000001,	///< Object is visible outside its package.
	RF_Standalone				=0x00000002,	///< Keep object around for editing even if unreferenced.
	RF_MarkAsNative					=0x00000004,	///< Object (UField) will be marked as native on construction (DO NOT USE THIS FLAG in HasAnyFlags() etc)
	RF_Transactional			=0x00000008,	///< Object is transactional.
	RF_ClassDefaultObject		=0x00000010,	///< This object is its class's default object
	RF_ArchetypeObject			=0x00000020,	///< This object is a template for another object - treat like a class default object
	RF_Transient				=0x00000040,	///< Don't save object.

	// This group of flags is primarily concerned with garbage collection.
	RF_MarkAsRootSet					=0x00000080,	///< Object will be marked as root set on construction and not be garbage collected, even if unreferenced (DO NOT USE THIS FLAG in HasAnyFlags() etc)
	RF_TagGarbageTemp			=0x00000100,	///< This is a temp user flag for various utilities that need to use the garbage collector. The garbage collector itself does not interpret it.

	// The group of flags tracks the stages of the lifetime of a uobject
	RF_NeedInitialization		=0x00000200,	///< This object has not completed its initialization process. Cleared when ~FObjectInitializer completes
	RF_NeedLoad					=0x00000400,	///< During load, indicates object needs loading.
	RF_KeepForCooker			=0x00000800,	///< Keep this object during garbage collection because it's still being used by the cooker
	RF_NeedPostLoad				=0x00001000,	///< Object needs to be postloaded.
	RF_NeedPostLoadSubobjects	=0x00002000,	///< During load, indicates that the object still needs to instance subobjects and fixup serialized component references
	RF_NewerVersionExists		=0x00004000,	///< Object has been consigned to oblivion due to its owner package being reloaded, and a newer version currently exists
	RF_BeginDestroyed			=0x00008000,	///< BeginDestroy has been called on the object.
	RF_FinishDestroyed			=0x00010000,	///< FinishDestroy has been called on the object.

	// Misc. Flags
	RF_BeingRegenerated			=0x00020000,	///< Flagged on UObjects that are used to create UClasses (e.g. Blueprints) while they are regenerating their UClass on load (See FLinkerLoad::CreateExport()), as well as UClass objects in the midst of being created
	RF_DefaultSubObject			=0x00040000,	///< Flagged on subobjects that are defaults
	RF_WasLoaded				=0x00080000,	///< Flagged on UObjects that were loaded
	RF_TextExportTransient		=0x00100000,	///< Do not export object to text form (e.g. copy/paste). Generally used for sub-objects that can be regenerated from data in their parent object.
	RF_LoadCompleted			=0x00200000,	///< Object has been completely serialized by linkerload at least once. DO NOT USE THIS FLAG, It should be replaced with RF_WasLoaded.
	RF_InheritableComponentTemplate = 0x00400000, ///< Archetype of the object can be in its super class
	RF_DuplicateTransient		=0x00800000,	///< Object should not be included in any type of duplication (copy/paste, binary duplication, etc.)
	RF_StrongRefOnFrame			=0x01000000,	///< References to this object from persistent function frame are handled as strong ones.
	RF_NonPIEDuplicateTransient	=0x02000000,	///< Object should not be included for duplication unless it's being duplicated for a PIE session
	RF_Dynamic					=0x04000000,	///< Field Only. Dynamic field - doesn't get constructed during static initialization, can be constructed multiple times
	RF_WillBeLoaded				=0x08000000,	///< This object was constructed during load and will be loaded shortly
	RF_HasExternalPackage		=0x10000000,	///< This object has an external package assigned and should look it up when getting the outermost package
};


/**
* Flags controlling [de]compression
* Make sure to update VerifyCompressionFlagsValid after changing these values.
*/
enum ECompressionFlags
{
	/** No compression																*/
	COMPRESS_None = 0x00,
	/** Compress with ZLIB - DEPRECATED, USE FNAME									*/
	COMPRESS_ZLIB = 0x01,
	/** Compress with GZIP - DEPRECATED, USE FNAME									*/
	COMPRESS_GZIP = 0x02,
	/** Compress with user defined callbacks - DEPRECATED, USE FNAME                */
	COMPRESS_Custom = 0x04,
	/** Joint of the previous ones to determine if old flags are being used			*/
	COMPRESS_DeprecatedFormatFlagsMask = 0xF,


	/** No flags specified /														*/
	COMPRESS_NoFlags = 0x00,
	/** Prefer compression that compresses smaller (ONLY VALID FOR COMPRESSION)		*/
	COMPRESS_BiasMemory = 0x10,
	/** Prefer compression that compresses faster (ONLY VALID FOR COMPRESSION)		*/
	COMPRESS_BiasSpeed = 0x20,
	/** Is the source buffer padded out	(ONLY VALID FOR UNCOMPRESS)					*/
	COMPRESS_SourceIsPadded = 0x80,

	/** Set of flags that are options are still allowed								*/
	COMPRESS_OptionsFlagsMask = 0xF0,
};

/** Package flags, passed into UPackage::SetPackageFlags and related functions */
enum<uint32> EPackageFlags
{
	PKG_None						= 0x00000000,	///< No flags
	PKG_NewlyCreated				= 0x00000001,	///< Newly created package, not saved yet. In editor only.
	PKG_ClientOptional				= 0x00000002,	///< Purely optional for clients.
	PKG_ServerSideOnly				= 0x00000004,   ///< Only needed on the server side.
	PKG_CompiledIn					= 0x00000010,   ///< This package is from "compiled in" classes.
	PKG_ForDiffing					= 0x00000020,	///< This package was loaded just for the purposes of diffing
	PKG_EditorOnly					= 0x00000040,	///< This is editor-only package (for example: editor module script package)
	PKG_Developer					= 0x00000080,	///< Developer module
	PKG_UncookedOnly				= 0x00000100,	///< Loaded only in uncooked builds (i.e. runtime in editor)
	PKG_Cooked						= 0x00000200,	///< Package is cooked
	PKG_ContainsNoAsset				= 0x00000400,	///< Package doesn't contain any asset object (although asset tags can be present)
//	PKG_Unused						= 0x00000800,
//	PKG_Unused						= 0x00001000,
	PKG_UnversionedProperties		= 0x00002000,   ///< Uses unversioned property serialization instead of versioned tagged property serialization
	PKG_ContainsMapData				= 0x00004000,   ///< Contains map data (UObjects only referenced by a single ULevel) but is stored in a different package
//	PKG_Unused						= 0x00008000,
	PKG_Compiling					= 0x00010000,	///< package is currently being compiled
	PKG_ContainsMap					= 0x00020000,	///< Set if the package contains a ULevel/ UWorld object
	PKG_RequiresLocalizationGather	= 0x00040000,	///< Set if the package contains any data to be gathered by localization
//	PKG_Unused						= 0x00080000,
	PKG_PlayInEditor				= 0x00100000,	///< Set if the package was created for the purpose of PIE
	PKG_ContainsScript				= 0x00200000,	///< Package is allowed to contain UClass objects
	PKG_DisallowExport				= 0x00400000,	///< Editor should not export asset in this package
//	PKG_Unused						= 0x00800000,
//	PKG_Unused						= 0x01000000,	
//	PKG_Unused						= 0x02000000,	
//	PKG_Unused						= 0x04000000,
//	PKG_Unused						= 0x08000000,	
	PKG_DynamicImports				= 0x10000000,	///< This package should resolve dynamic imports from its export at runtime.
	PKG_RuntimeGenerated			= 0x20000000,	///< This package contains elements that are runtime generated, and may not follow standard loading order rules
	PKG_ReloadingForCooker			= 0x40000000,   ///< This package is reloading in the cooker, try to avoid getting data we will never need. We won't save this package.
	PKG_FilterEditorOnly			= 0x80000000,	///< Package has editor-only data filtered out
};

#define PACKAGE_FILE_TAG			0x9E2A83C1
#define PACKAGE_FILE_TAG_SWAPPED	0xC1832A9E

enum<int32> EUnrealEngineObjectUE4Version
{
	VER_UE4_OLDEST_LOADABLE_PACKAGE = 214,

	// Removed restriction on blueprint-exposed variables from being read-only
	VER_UE4_BLUEPRINT_VARS_NOT_READ_ONLY,
	// Added manually serialized element to UStaticMesh (precalculated nav collision)
	VER_UE4_STATIC_MESH_STORE_NAV_COLLISION,
	// Changed property name for atmospheric fog
	VER_UE4_ATMOSPHERIC_FOG_DECAY_NAME_CHANGE,
	// Change many properties/functions from Translation to Location
	VER_UE4_SCENECOMP_TRANSLATION_TO_LOCATION,
	// Material attributes reordering
	VER_UE4_MATERIAL_ATTRIBUTES_REORDERING,
	// Collision Profile setting has been added, and all components that exists has to be properly upgraded
	VER_UE4_COLLISION_PROFILE_SETTING,
	// Making the blueprint's skeleton class transient
	VER_UE4_BLUEPRINT_SKEL_TEMPORARY_TRANSIENT,
	// Making the blueprint's skeleton class serialized again
	VER_UE4_BLUEPRINT_SKEL_SERIALIZED_AGAIN,
	// Blueprint now controls replication settings again
	VER_UE4_BLUEPRINT_SETS_REPLICATION,
	// Added level info used by World browser
	VER_UE4_WORLD_LEVEL_INFO,
	// Changed capsule height to capsule half-height (afterwards)
	VER_UE4_AFTER_CAPSULE_HALF_HEIGHT_CHANGE,
	// Added Namepace, GUID (Key) and Flags to FText
	VER_UE4_ADDED_NAMESPACE_AND_KEY_DATA_TO_FTEXT,
	// Attenuation shapes
	VER_UE4_ATTENUATION_SHAPES,
	// Use IES texture multiplier even when IES brightness is not being used
	VER_UE4_LIGHTCOMPONENT_USE_IES_TEXTURE_MULTIPLIER_ON_NON_IES_BRIGHTNESS,
	// Removed InputComponent as a blueprint addable component
	VER_UE4_REMOVE_INPUT_COMPONENTS_FROM_BLUEPRINTS,
	// Use an FMemberReference struct in UK2Node_Variable
	VER_UE4_VARK2NODE_USE_MEMBERREFSTRUCT,
	// Refactored material expression inputs for UMaterialExpressionSceneColor and UMaterialExpressionSceneDepth
	VER_UE4_REFACTOR_MATERIAL_EXPRESSION_SCENECOLOR_AND_SCENEDEPTH_INPUTS,
	// Spline meshes changed from Z forwards to configurable
	VER_UE4_SPLINE_MESH_ORIENTATION,
	// Added ReverbEffect asset type
	VER_UE4_REVERB_EFFECT_ASSET_TYPE,
	// changed max texcoords from 4 to 8
	VER_UE4_MAX_TEXCOORD_INCREASED,
	// static meshes changed to support SpeedTrees
	VER_UE4_SPEEDTREE_STATICMESH,
	// Landscape component reference between landscape component and collision component
	VER_UE4_LANDSCAPE_COMPONENT_LAZY_REFERENCES,
	// Refactored UK2Node_CallFunction to use FMemberReference
	VER_UE4_SWITCH_CALL_NODE_TO_USE_MEMBER_REFERENCE,
	// Added fixup step to remove skeleton class references from blueprint objects
	VER_UE4_ADDED_SKELETON_ARCHIVER_REMOVAL,
	// See above, take 2.
	VER_UE4_ADDED_SKELETON_ARCHIVER_REMOVAL_SECOND_TIME,
	// Making the skeleton class on blueprints transient
	VER_UE4_BLUEPRINT_SKEL_CLASS_TRANSIENT_AGAIN,
	// UClass knows if it's been cooked
	VER_UE4_ADD_COOKED_TO_UCLASS,
	// Deprecated static mesh thumbnail properties were removed
	VER_UE4_DEPRECATED_STATIC_MESH_THUMBNAIL_PROPERTIES_REMOVED,
	// Added collections in material shader map ids
	VER_UE4_COLLECTIONS_IN_SHADERMAPID,
	// Renamed some Movement Component properties, added PawnMovementComponent
	VER_UE4_REFACTOR_MOVEMENT_COMPONENT_HIERARCHY,
	// Swap UMaterialExpressionTerrainLayerSwitch::LayerUsed/LayerNotUsed the correct way round
	VER_UE4_FIX_TERRAIN_LAYER_SWITCH_ORDER,
	// Remove URB_ConstraintSetup
	VER_UE4_ALL_PROPS_TO_CONSTRAINTINSTANCE,
	// Low quality directional lightmaps
	VER_UE4_LOW_QUALITY_DIRECTIONAL_LIGHTMAPS,
	// Added NoiseEmitterComponent and removed related Pawn properties.
	VER_UE4_ADDED_NOISE_EMITTER_COMPONENT,
	// Add text component vertical alignment
	VER_UE4_ADD_TEXT_COMPONENT_VERTICAL_ALIGNMENT,
	// Added AssetImportData for FBX asset types, deprecating SourceFilePath and SourceFileTimestamp
	VER_UE4_ADDED_FBX_ASSET_IMPORT_DATA,
	// Remove LevelBodySetup from ULevel
	VER_UE4_REMOVE_LEVELBODYSETUP,
	// Refactor character crouching
	VER_UE4_REFACTOR_CHARACTER_CROUCH,	
	// Trimmed down material shader debug information.
	VER_UE4_SMALLER_DEBUG_MATERIALSHADER_UNIFORM_EXPRESSIONS,
	// APEX Clothing
	VER_UE4_APEX_CLOTH,
	// Change Collision Channel to save only modified ones than all of them
	// @note!!! once we pass this CL, we can rename FCollisionResponseContainer enum values
	// we should rename to match ECollisionChannel
	VER_UE4_SAVE_COLLISIONRESPONSE_PER_CHANNEL,
	// Added Landscape Spline editor meshes
	VER_UE4_ADDED_LANDSCAPE_SPLINE_EDITOR_MESH,
	// Fixup input expressions for reading from refraction material attributes.
	VER_UE4_CHANGED_MATERIAL_REFACTION_TYPE,
	// Refactor projectile movement, along with some other movement component work.
	VER_UE4_REFACTOR_PROJECTILE_MOVEMENT,
	// Remove PhysicalMaterialProperty and replace with user defined enum
	VER_UE4_REMOVE_PHYSICALMATERIALPROPERTY,
	// Removed all compile outputs from FMaterial
	VER_UE4_PURGED_FMATERIAL_COMPILE_OUTPUTS,
	// Ability to save cooked PhysX meshes to Landscape
	VER_UE4_ADD_COOKED_TO_LANDSCAPE,
	// Change how input component consumption works
	VER_UE4_CONSUME_INPUT_PER_BIND,
	// Added new Graph based SoundClass Editor
	VER_UE4_SOUND_CLASS_GRAPH_EDITOR,
	// Fixed terrain layer node guids which was causing artifacts
	VER_UE4_FIXUP_TERRAIN_LAYER_NODES,
	// Added clamp min/max swap check to catch older materials
	VER_UE4_RETROFIT_CLAMP_EXPRESSIONS_SWAP,
	// Remove static/movable/stationary light classes
	VER_UE4_REMOVE_LIGHT_MOBILITY_CLASSES,
	// Refactor the way physics blending works to allow partial blending
	VER_UE4_REFACTOR_PHYSICS_BLENDING,
	// WorldLevelInfo: Added reference to parent level and streaming distance
	VER_UE4_WORLD_LEVEL_INFO_UPDATED,
	// Fixed cooking of skeletal/static meshes due to bad serialization logic
	VER_UE4_STATIC_SKELETAL_MESH_SERIALIZATION_FIX,
	// Removal of InterpActor and PhysicsActor
	VER_UE4_REMOVE_STATICMESH_MOBILITY_CLASSES,
	// Refactor physics transforms
	VER_UE4_REFACTOR_PHYSICS_TRANSFORMS,
	// Remove zero triangle sections from static meshes and compact material indices.
	VER_UE4_REMOVE_ZERO_TRIANGLE_SECTIONS,
	// Add param for deceleration in character movement instead of using acceleration.
	VER_UE4_CHARACTER_MOVEMENT_DECELERATION,
	// Made ACameraActor use a UCameraComponent for parameter storage, etc...
	VER_UE4_CAMERA_ACTOR_USING_CAMERA_COMPONENT,
	// Deprecated some pitch/roll properties in CharacterMovementComponent
	VER_UE4_CHARACTER_MOVEMENT_DEPRECATE_PITCH_ROLL,
	// Rebuild texture streaming data on load for uncooked builds
	VER_UE4_REBUILD_TEXTURE_STREAMING_DATA_ON_LOAD,
	// Add support for 32 bit index buffers for static meshes.
	VER_UE4_SUPPORT_32BIT_STATIC_MESH_INDICES,
	// Added streaming install ChunkID to AssetData and UPackage
	VER_UE4_ADDED_CHUNKID_TO_ASSETDATA_AND_UPACKAGE,
	// Add flag to control whether Character blueprints receive default movement bindings.
	VER_UE4_CHARACTER_DEFAULT_MOVEMENT_BINDINGS,
	// APEX Clothing LOD Info
	VER_UE4_APEX_CLOTH_LOD,
	// Added atmospheric fog texture data to be general
	VER_UE4_ATMOSPHERIC_FOG_CACHE_DATA,
	// Arrays serialize their inner's tags
	VAR_UE4_ARRAY_PROPERTY_INNER_TAGS,
	// Skeletal mesh index data is kept in memory in game to support mesh merging.
	VER_UE4_KEEP_SKEL_MESH_INDEX_DATA,
	// Added compatibility for the body instance collision change
	VER_UE4_BODYSETUP_COLLISION_CONVERSION,
	// Reflection capture cooking
	VER_UE4_REFLECTION_CAPTURE_COOKING,
	// Removal of DynamicTriggerVolume, DynamicBlockingVolume, DynamicPhysicsVolume
	VER_UE4_REMOVE_DYNAMIC_VOLUME_CLASSES,
	// Store an additional flag in the BodySetup to indicate whether there is any cooked data to load
	VER_UE4_STORE_HASCOOKEDDATA_FOR_BODYSETUP,
	// Changed name of RefractionBias to RefractionDepthBias.
	VER_UE4_REFRACTION_BIAS_TO_REFRACTION_DEPTH_BIAS,
	// Removal of SkeletalPhysicsActor
	VER_UE4_REMOVE_SKELETALPHYSICSACTOR,
	// PlayerController rotation input refactor
	VER_UE4_PC_ROTATION_INPUT_REFACTOR,
	// Landscape Platform Data cooking
	VER_UE4_LANDSCAPE_PLATFORMDATA_COOKING,
	// Added call for linking classes in CreateExport to ensure memory is initialized properly
	VER_UE4_CREATEEXPORTS_CLASS_LINKING_FOR_BLUEPRINTS,
	// Remove native component nodes from the blueprint SimpleConstructionScript
	VER_UE4_REMOVE_NATIVE_COMPONENTS_FROM_BLUEPRINT_SCS,
	// Removal of Single Node Instance
	VER_UE4_REMOVE_SINGLENODEINSTANCE, 
	// Character movement braking changes
	VER_UE4_CHARACTER_BRAKING_REFACTOR,
	// Supported low quality lightmaps in volume samples
	VER_UE4_VOLUME_SAMPLE_LOW_QUALITY_SUPPORT,
	// Split bEnableTouchEvents out from bEnableClickEvents
	VER_UE4_SPLIT_TOUCH_AND_CLICK_ENABLES,
	// Health/Death refactor
	VER_UE4_HEALTH_DEATH_REFACTOR,
	// Moving USoundNodeEnveloper from UDistributionFloatConstantCurve to FRichCurve
	VER_UE4_SOUND_NODE_ENVELOPER_CURVE_CHANGE,
	// Moved SourceRadius to UPointLightComponent
	VER_UE4_POINT_LIGHT_SOURCE_RADIUS,
	// Scene capture actors based on camera actors.
	VER_UE4_SCENE_CAPTURE_CAMERA_CHANGE,
	// Moving SkeletalMesh shadow casting flag from LoD details to material
	VER_UE4_MOVE_SKELETALMESH_SHADOWCASTING,
	// Changing bytecode operators for creating arrays
	VER_UE4_CHANGE_SETARRAY_BYTECODE,
	// Material Instances overriding base material properties.
	VER_UE4_MATERIAL_INSTANCE_BASE_PROPERTY_OVERRIDES,
	// Combined top/bottom lightmap textures
	VER_UE4_COMBINED_LIGHTMAP_TEXTURES,
	// Forced material lightmass guids to be regenerated
	VER_UE4_BUMPED_MATERIAL_EXPORT_GUIDS,
	// Allow overriding of parent class input bindings
	VER_UE4_BLUEPRINT_INPUT_BINDING_OVERRIDES,
	// Fix up convex invalid transform
	VER_UE4_FIXUP_BODYSETUP_INVALID_CONVEX_TRANSFORM, 
	// Fix up scale of physics stiffness and damping value
	VER_UE4_FIXUP_STIFFNESS_AND_DAMPING_SCALE, 
	// Convert USkeleton and FBoneContrainer to using FReferenceSkeleton.
	VER_UE4_REFERENCE_SKELETON_REFACTOR,
	// Adding references to variable, function, and macro nodes to be able to update to renamed values
	VER_UE4_K2NODE_REFERENCEGUIDS,
	// Fix up the 0th bone's parent bone index.
	VER_UE4_FIXUP_ROOTBONE_PARENT,
	//Allow setting of TextRenderComponents size in world space.
	VER_UE4_TEXT_RENDER_COMPONENTS_WORLD_SPACE_SIZING,
	// Material Instances overriding base material properties #2.
	VER_UE4_MATERIAL_INSTANCE_BASE_PROPERTY_OVERRIDES_PHASE_2,
	// CLASS_Placeable becomes CLASS_NotPlaceable
	VER_UE4_CLASS_NOTPLACEABLE_ADDED,
	// Added LOD info list to a world tile description
	VER_UE4_WORLD_LEVEL_INFO_LOD_LIST,
	// CharacterMovement variable naming refactor
	VER_UE4_CHARACTER_MOVEMENT_VARIABLE_RENAMING_1,
	// FName properties containing sound names converted to FSlateSound properties
	VER_UE4_FSLATESOUND_CONVERSION,
	// Added ZOrder to a world tile description
	VER_UE4_WORLD_LEVEL_INFO_ZORDER,
	// Added flagging of localization gather requirement to packages
	VER_UE4_PACKAGE_REQUIRES_LOCALIZATION_GATHER_FLAGGING,
	// Preventing Blueprint Actor variables from having default values
	VER_UE4_BP_ACTOR_VARIABLE_DEFAULT_PREVENTING,
	// Preventing Blueprint Actor variables from having default values
	VER_UE4_TEST_ANIMCOMP_CHANGE,
	// Class as primary asset, name convention changed
	VER_UE4_EDITORONLY_BLUEPRINTS,
	// Custom serialization for FEdGraphPinType
	VER_UE4_EDGRAPHPINTYPE_SERIALIZATION,
	// Stop generating 'mirrored' cooked mesh for Brush and Model components
	VER_UE4_NO_MIRROR_BRUSH_MODEL_COLLISION,
	// Changed ChunkID to be an array of IDs.
	VER_UE4_CHANGED_CHUNKID_TO_BE_AN_ARRAY_OF_CHUNKIDS,
	// Worlds have been renamed from "TheWorld" to be named after the package containing them
	VER_UE4_WORLD_NAMED_AFTER_PACKAGE,
	// Added sky light component
	VER_UE4_SKY_LIGHT_COMPONENT,
	// Added Enable distance streaming flag to FWorldTileLayer
	VER_UE4_WORLD_LAYER_ENABLE_DISTANCE_STREAMING,
	// Remove visibility/zone information from UModel
	VER_UE4_REMOVE_ZONES_FROM_MODEL,
	// Fix base pose serialization 
	VER_UE4_FIX_ANIMATIONBASEPOSE_SERIALIZATION,
	// Support for up to 8 skinning influences per vertex on skeletal meshes (on non-gpu vertices)
	VER_UE4_SUPPORT_8_BONE_INFLUENCES_SKELETAL_MESHES,
	// Add explicit bOverrideGravity to world settings
	VER_UE4_ADD_OVERRIDE_GRAVITY_FLAG,
	// Support for up to 8 skinning influences per vertex on skeletal meshes (on gpu vertices)
	VER_UE4_SUPPORT_GPUSKINNING_8_BONE_INFLUENCES,
	// Supporting nonuniform scale animation
	VER_UE4_ANIM_SUPPORT_NONUNIFORM_SCALE_ANIMATION,
	// Engine version is stored as a FEngineVersion object rather than changelist number
	VER_UE4_ENGINE_VERSION_OBJECT,
	// World assets now have RF_Public
	VER_UE4_PUBLIC_WORLDS,
	// Skeleton Guid
	VER_UE4_SKELETON_GUID_SERIALIZATION,
	// Character movement WalkableFloor refactor
	VER_UE4_CHARACTER_MOVEMENT_WALKABLE_FLOOR_REFACTOR,
	// Lights default to inverse squared
	VER_UE4_INVERSE_SQUARED_LIGHTS_DEFAULT,
	// Disabled SCRIPT_LIMIT_BYTECODE_TO_64KB
	VER_UE4_DISABLED_SCRIPT_LIMIT_BYTECODE,
	// Made remote role private, exposed bReplicates
	VER_UE4_PRIVATE_REMOTE_ROLE,
	// Fix up old foliage components to have static mobility (superseded by VER_UE4_FOLIAGE_MOVABLE_MOBILITY)
	VER_UE4_FOLIAGE_STATIC_MOBILITY,
	// Change BuildScale from a float to a vector
	VER_UE4_BUILD_SCALE_VECTOR,
	// After implementing foliage collision, need to disable collision on old foliage instances
	VER_UE4_FOLIAGE_COLLISION,
	// Added sky bent normal to indirect lighting cache
	VER_UE4_SKY_BENT_NORMAL,
	// Added cooking for landscape collision data
	VER_UE4_LANDSCAPE_COLLISION_DATA_COOKING,
	// Convert CPU tangent Z delta to vector from PackedNormal since we don't get any benefit other than memory
	// we still convert all to FVector in CPU time whenever any calculation
	VER_UE4_MORPHTARGET_CPU_TANGENTZDELTA_FORMATCHANGE,
	// Soft constraint limits will implicitly use the mass of the bodies
	VER_UE4_SOFT_CONSTRAINTS_USE_MASS,
	// Reflection capture data saved in packages
	VER_UE4_REFLECTION_DATA_IN_PACKAGES,
	// Fix up old foliage components to have movable mobility (superseded by VER_UE4_FOLIAGE_STATIC_LIGHTING_SUPPORT)
	VER_UE4_FOLIAGE_MOVABLE_MOBILITY,
	// Undo BreakMaterialAttributes changes as it broke old content
	VER_UE4_UNDO_BREAK_MATERIALATTRIBUTES_CHANGE,
	// Now Default custom profile name isn't NONE anymore due to copy/paste not working properly with it
	VER_UE4_ADD_CUSTOMPROFILENAME_CHANGE, 
	// Permanently flip and scale material expression coordinates
	VER_UE4_FLIP_MATERIAL_COORDS,
	// PinSubCategoryMemberReference added to FEdGraphPinType
	VER_UE4_MEMBERREFERENCE_IN_PINTYPE,
	// Vehicles use Nm for Torque instead of cm and RPM instead of rad/s
	VER_UE4_VEHICLES_UNIT_CHANGE,
	// removes NANs from all animations when loaded
	// now importing should detect NaNs, so we should not have NaNs in source data
	VER_UE4_ANIMATION_REMOVE_NANS,
	// Change skeleton preview attached assets property type
	VER_UE4_SKELETON_ASSET_PROPERTY_TYPE_CHANGE,
	// Fix some blueprint variables that have the CPF_DisableEditOnTemplate flag set
	// when they shouldn't
	VER_UE4_FIX_BLUEPRINT_VARIABLE_FLAGS,
	// Vehicles use Nm for Torque instead of cm and RPM instead of rad/s part two (missed conversion for some variables
	VER_UE4_VEHICLES_UNIT_CHANGE2,
	// Changed order of interface class serialization
	VER_UE4_UCLASS_SERIALIZE_INTERFACES_AFTER_LINKING,
	// Change from LOD distances to display factors
	VER_UE4_STATIC_MESH_SCREEN_SIZE_LODS,
	// Requires test of material coords to ensure they're saved correctly
	VER_UE4_FIX_MATERIAL_COORDS,
	// Changed SpeedTree wind presets to v7
	VER_UE4_SPEEDTREE_WIND_V7,
	// NeedsLoadForEditorGame added
	VER_UE4_LOAD_FOR_EDITOR_GAME,
	// Manual serialization of FRichCurveKey to save space
	VER_UE4_SERIALIZE_RICH_CURVE_KEY,
	// Change the outer of ULandscapeMaterialInstanceConstants and Landscape-related textures to the level in which they reside
	VER_UE4_MOVE_LANDSCAPE_MICS_AND_TEXTURES_WITHIN_LEVEL,
	// FTexts have creation history data, removed Key, Namespaces, and SourceString
	VER_UE4_FTEXT_HISTORY,
	// Shift comments to the left to contain expressions properly
	VER_UE4_FIX_MATERIAL_COMMENTS,
	// Bone names stored as FName means that we can't guarantee the correct case on export, now we store a separate string for export purposes only
	VER_UE4_STORE_BONE_EXPORT_NAMES,
	// changed mesh emitter initial orientation to distribution
	VER_UE4_MESH_EMITTER_INITIAL_ORIENTATION_DISTRIBUTION,
	// Foliage on blueprints causes crashes
	VER_UE4_DISALLOW_FOLIAGE_ON_BLUEPRINTS,
	// change motors to use revolutions per second instead of rads/second
	VER_UE4_FIXUP_MOTOR_UNITS,
	// deprecated MovementComponent functions including "ModifiedMaxSpeed" et al
	VER_UE4_DEPRECATED_MOVEMENTCOMPONENT_MODIFIED_SPEEDS,
	// rename CanBeCharacterBase
	VER_UE4_RENAME_CANBECHARACTERBASE,
	// Change GameplayTagContainers to have FGameplayTags instead of FNames; Required to fix-up native serialization
	VER_UE4_GAMEPLAY_TAG_CONTAINER_TAG_TYPE_CHANGE,
	// Change from UInstancedFoliageSettings to UFoliageType, and change the api from being keyed on UStaticMesh* to UFoliageType*
	VER_UE4_FOLIAGE_SETTINGS_TYPE,
	// Lights serialize static shadow depth maps
	VER_UE4_STATIC_SHADOW_DEPTH_MAPS,
	// Add RF_Transactional to data assets, fixing undo problems when editing them
	VER_UE4_ADD_TRANSACTIONAL_TO_DATA_ASSETS,
	// Change LB_AlphaBlend to LB_WeightBlend in ELandscapeLayerBlendType
	VER_UE4_ADD_LB_WEIGHTBLEND,
	// Add root component to an foliage actor, all foliage cluster components will be attached to a root
	VER_UE4_ADD_ROOTCOMPONENT_TO_FOLIAGEACTOR,
	// FMaterialInstanceBasePropertyOverrides didn't use proper UObject serialize
	VER_UE4_FIX_MATERIAL_PROPERTY_OVERRIDE_SERIALIZE,
	// Addition of linear color sampler. color sample type is changed to linear sampler if source texture !sRGB
	VER_UE4_ADD_LINEAR_COLOR_SAMPLER,
	// Added StringAssetReferencesMap to support renames of FStringAssetReference properties.
	VER_UE4_ADD_STRING_ASSET_REFERENCES_MAP,
	// Apply scale from SCS RootComponent details in the Blueprint Editor to new actor instances at construction time
	VER_UE4_BLUEPRINT_USE_SCS_ROOTCOMPONENT_SCALE,
	// Changed level streaming to have a linear color since the visualization doesn't gamma correct.
	VER_UE4_LEVEL_STREAMING_DRAW_COLOR_TYPE_CHANGE,
	// Cleared end triggers from non-state anim notifies
	VER_UE4_CLEAR_NOTIFY_TRIGGERS,
	// Convert old curve names stored in anim assets into skeleton smartnames
	VER_UE4_SKELETON_ADD_SMARTNAMES,
	// Added the currency code field to FTextHistory_AsCurrency
	VER_UE4_ADDED_CURRENCY_CODE_TO_FTEXT,
	// Added support for C++11 enum classes
	VER_UE4_ENUM_CLASS_SUPPORT,
	// Fixup widget animation class
	VER_UE4_FIXUP_WIDGET_ANIMATION_CLASS,
	// USoundWave objects now contain details about compression scheme used.
	VER_UE4_SOUND_COMPRESSION_TYPE_ADDED,
	// Bodies will automatically weld when attached
	VER_UE4_AUTO_WELDING,
	// Rename UCharacterMovementComponent::bCrouchMovesCharacterDown
	VER_UE4_RENAME_CROUCHMOVESCHARACTERDOWN,
	// Lightmap parameters in FMeshBuildSettings
	VER_UE4_LIGHTMAP_MESH_BUILD_SETTINGS,
	// Rename SM3 to ES3_1 and updates featurelevel material node selector
	VER_UE4_RENAME_SM3_TO_ES3_1,
	// Deprecated separate style assets for use in UMG
	VER_UE4_DEPRECATE_UMG_STYLE_ASSETS,
	// Duplicating Blueprints will regenerate NodeGuids after this version
	VER_UE4_POST_DUPLICATE_NODE_GUID,
	// Rename USpringArmComponent::bUseControllerViewRotation to bUsePawnViewRotation,
	// Rename UCameraComponent::bUseControllerViewRotation to bUsePawnViewRotation (and change the default value)
	VER_UE4_RENAME_CAMERA_COMPONENT_VIEW_ROTATION,
	// Changed FName to be case preserving
	VER_UE4_CASE_PRESERVING_FNAME,
	// Rename USpringArmComponent::bUsePawnViewRotation to bUsePawnControlRotation
	// Rename UCameraComponent::bUsePawnViewRotation to bUsePawnControlRotation
	VER_UE4_RENAME_CAMERA_COMPONENT_CONTROL_ROTATION,
	// Fix bad refraction material attribute masks
	VER_UE4_FIX_REFRACTION_INPUT_MASKING,
	// A global spawn rate for emitters.
	VER_UE4_GLOBAL_EMITTER_SPAWN_RATE_SCALE,
	// Cleanup destructible mesh settings
	VER_UE4_CLEAN_DESTRUCTIBLE_SETTINGS,
	// CharacterMovementComponent refactor of AdjustUpperHemisphereImpact and deprecation of some associated vars.
	VER_UE4_CHARACTER_MOVEMENT_UPPER_IMPACT_BEHAVIOR,
	// Changed Blueprint math equality functions for vectors and rotators to operate as a "nearly" equals rather than "exact"
	VER_UE4_BP_MATH_VECTOR_EQUALITY_USES_EPSILON,
	// Static lighting support was re-added to foliage, and mobility was returned to static
	VER_UE4_FOLIAGE_STATIC_LIGHTING_SUPPORT,
	// Added composite fonts to Slate font info
	VER_UE4_SLATE_COMPOSITE_FONTS,
	// Remove UDEPRECATED_SaveGameSummary, required for UWorld::Serialize
	VER_UE4_REMOVE_SAVEGAMESUMMARY,

	//Remove bodyseutp serialization from skeletal mesh component
	VER_UE4_REMOVE_SKELETALMESH_COMPONENT_BODYSETUP_SERIALIZATION,
	// Made Slate font data use bulk data to store the embedded font data
	VER_UE4_SLATE_BULK_FONT_DATA,
	// Add new friction behavior in ProjectileMovementComponent.
	VER_UE4_ADD_PROJECTILE_FRICTION_BEHAVIOR,
	// Add axis settings enum to MovementComponent.
	VER_UE4_MOVEMENTCOMPONENT_AXIS_SETTINGS,
	// Switch to new interactive comments, requires boundry conversion to preserve previous states
	VER_UE4_GRAPH_INTERACTIVE_COMMENTBUBBLES,
	// Landscape serializes physical materials for collision objects 
	VER_UE4_LANDSCAPE_SERIALIZE_PHYSICS_MATERIALS,
	// Rename Visiblity on widgets to Visibility
	VER_UE4_RENAME_WIDGET_VISIBILITY,
	// add track curves for animation
	VER_UE4_ANIMATION_ADD_TRACKCURVES,
	// Removed BranchingPoints from AnimMontages and converted them to regular AnimNotifies.
	VER_UE4_MONTAGE_BRANCHING_POINT_REMOVAL,
	// Enforce const-correctness in Blueprint implementations of native C++ const class methods
	VER_UE4_BLUEPRINT_ENFORCE_CONST_IN_FUNCTION_OVERRIDES,
	// Added pivot to widget components, need to load old versions as a 0,0 pivot, new default is 0.5,0.5
	VER_UE4_ADD_PIVOT_TO_WIDGET_COMPONENT,
	// Added finer control over when AI Pawns are automatically possessed. Also renamed Pawn.AutoPossess to Pawn.AutoPossessPlayer indicate this was a setting for players and not AI.
	VER_UE4_PAWN_AUTO_POSSESS_AI,
	// Added serialization of timezone to FTextHistory for AsDate operations.
	VER_UE4_FTEXT_HISTORY_DATE_TIMEZONE,
	// Sort ActiveBoneIndices on lods so that we can avoid doing it at run time
	VER_UE4_SORT_ACTIVE_BONE_INDICES,
	// Added per-frame material uniform expressions
	VER_UE4_PERFRAME_MATERIAL_UNIFORM_EXPRESSIONS,
	// Make MikkTSpace the default tangent space calculation method for static meshes.
	VER_UE4_MIKKTSPACE_IS_DEFAULT,
	// Only applies to cooked files, grass cooking support.
	VER_UE4_LANDSCAPE_GRASS_COOKING,
	// Fixed code for using the bOrientMeshEmitters property.
	VER_UE4_FIX_SKEL_VERT_ORIENT_MESH_PARTICLES,
	// Do not change landscape section offset on load under world composition
	VER_UE4_LANDSCAPE_STATIC_SECTION_OFFSET,
	// New options for navigation data runtime generation (static, modifiers only, dynamic)
	VER_UE4_ADD_MODIFIERS_RUNTIME_GENERATION,
	// Tidied up material's handling of masked blend mode.
	VER_UE4_MATERIAL_MASKED_BLENDMODE_TIDY,
	// Original version of VER_UE4_MERGED_ADD_MODIFIERS_RUNTIME_GENERATION_TO_4_7; renumbered to prevent blocking promotion in main.
	VER_UE4_MERGED_ADD_MODIFIERS_RUNTIME_GENERATION_TO_4_7_DEPRECATED,
	// Original version of VER_UE4_AFTER_MERGED_ADD_MODIFIERS_RUNTIME_GENERATION_TO_4_7; renumbered to prevent blocking promotion in main.
	VER_UE4_AFTER_MERGED_ADD_MODIFIERS_RUNTIME_GENERATION_TO_4_7_DEPRECATED,
	// After merging VER_UE4_ADD_MODIFIERS_RUNTIME_GENERATION into 4.7 branch
	VER_UE4_MERGED_ADD_MODIFIERS_RUNTIME_GENERATION_TO_4_7,
	// After merging VER_UE4_ADD_MODIFIERS_RUNTIME_GENERATION into 4.7 branch
	VER_UE4_AFTER_MERGING_ADD_MODIFIERS_RUNTIME_GENERATION_TO_4_7,
	// Landscape grass weightmap data is now generated in the editor and serialized.
	VER_UE4_SERIALIZE_LANDSCAPE_GRASS_DATA,
	// New property to optionally prevent gpu emitters clearing existing particles on Init().
	VER_UE4_OPTIONALLY_CLEAR_GPU_EMITTERS_ON_INIT,
	// Also store the Material guid with the landscape grass data
	VER_UE4_SERIALIZE_LANDSCAPE_GRASS_DATA_MATERIAL_GUID,
	// Make sure that all template components from blueprint generated classes are flagged as public
	VER_UE4_BLUEPRINT_GENERATED_CLASS_COMPONENT_TEMPLATES_PUBLIC,
	// Split out creation method on ActorComponents to distinguish between native, instance, and simple or user construction script
	VER_UE4_ACTOR_COMPONENT_CREATION_METHOD,
	// K2Node_Event now uses FMemberReference for handling references
	VER_UE4_K2NODE_EVENT_MEMBER_REFERENCE,
	// FPropertyTag stores GUID of struct
	VER_UE4_STRUCT_GUID_IN_PROPERTY_TAG,
	// Remove unused UPolys from UModel cooked content
	VER_UE4_REMOVE_UNUSED_UPOLYS_FROM_UMODEL,
	// This doesn't do anything except trigger a rebuild on HISMC cluster trees, in this case to get a good "occlusion query" level
	VER_UE4_REBUILD_HIERARCHICAL_INSTANCE_TREES,
	// Package summary includes an CompatibleWithEngineVersion field, separately to the version it's saved with
	VER_UE4_PACKAGE_SUMMARY_HAS_COMPATIBLE_ENGINE_VERSION,
	// Track UCS modified properties on Actor Components
	VER_UE4_TRACK_UCS_MODIFIED_PROPERTIES,
	// Allowed landscape spline meshes to be stored into landscape streaming levels rather than the spline's level
	VER_UE4_LANDSCAPE_SPLINE_CROSS_LEVEL_MESHES,
	// Deprecate the variables used for sizing in the designer on UUserWidget
	VER_UE4_DEPRECATE_USER_WIDGET_DESIGN_SIZE,
	// Make the editor views array dynamically sized
	VER_UE4_ADD_EDITOR_VIEWS,
	// Updated foliage to work with either FoliageType assets or blueprint classes
	VER_UE4_FOLIAGE_WITH_ASSET_OR_CLASS,
	// Allows PhysicsSerializer to serialize shapes and actors for faster load times
	VER_UE4_BODYINSTANCE_BINARY_SERIALIZATION,
	// Added fastcall data serialization directly in UFunction
	VER_UE4_SERIALIZE_BLUEPRINT_EVENTGRAPH_FASTCALLS_IN_UFUNCTION,
	// Changes to USplineComponent and FInterpCurve
	VER_UE4_INTERPCURVE_SUPPORTS_LOOPING,
	// Material Instances overriding base material LOD transitions
	VER_UE4_MATERIAL_INSTANCE_BASE_PROPERTY_OVERRIDES_DITHERED_LOD_TRANSITION,
	// Serialize ES2 textures separately rather than overwriting the properties used on other platforms
	VER_UE4_SERIALIZE_LANDSCAPE_ES2_TEXTURES,
	// Constraint motor velocity is broken into per-component
	VER_UE4_CONSTRAINT_INSTANCE_MOTOR_FLAGS,
	// Serialize bIsConst in FEdGraphPinType
	VER_UE4_SERIALIZE_PINTYPE_CONST,
	// Change UMaterialFunction::LibraryCategories to LibraryCategoriesText (old assets were saved before auto-conversion of FArrayProperty was possible)
	VER_UE4_LIBRARY_CATEGORIES_AS_FTEXT,
	// Check for duplicate exports while saving packages.
	VER_UE4_SKIP_DUPLICATE_EXPORTS_ON_SAVE_PACKAGE,
	// Pre-gathering of gatherable, localizable text in packages to optimize text gathering operation times
	VER_UE4_SERIALIZE_TEXT_IN_PACKAGES,
	// Added pivot to widget components, need to load old versions as a 0,0 pivot, new default is 0.5,0.5
	VER_UE4_ADD_BLEND_MODE_TO_WIDGET_COMPONENT,
	// Added lightmass primitive setting
	VER_UE4_NEW_LIGHTMASS_PRIMITIVE_SETTING,
	// Deprecate NoZSpring property on spring nodes to be replaced with TranslateZ property
	VER_UE4_REPLACE_SPRING_NOZ_PROPERTY,
	// Keep enums tight and serialize their values as pairs of FName and value. Don't insert dummy values.
	VER_UE4_TIGHTLY_PACKED_ENUMS,
	// Changed Asset import data to serialize file meta data as JSON
	VER_UE4_ASSET_IMPORT_DATA_AS_JSON,
	// Legacy gamma support for textures.
	VER_UE4_TEXTURE_LEGACY_GAMMA,
	// Added WithSerializer for basic native structures like FVector, FColor etc to improve serialization performance
	VER_UE4_ADDED_NATIVE_SERIALIZATION_FOR_IMMUTABLE_STRUCTURES,
	// Deprecated attributes that override the style on UMG widgets
	VER_UE4_DEPRECATE_UMG_STYLE_OVERRIDES,
	// Shadowmap penumbra size stored
	VER_UE4_STATIC_SHADOWMAP_PENUMBRA_SIZE,
	// Fix BC on Niagara effects from the data object and dev UI changes.
	VER_UE4_NIAGARA_DATA_OBJECT_DEV_UI_FIX,
	// Fixed the default orientation of widget component so it faces down +x
	VER_UE4_FIXED_DEFAULT_ORIENTATION_OF_WIDGET_COMPONENT,
	// Removed bUsedWithUI flag from UMaterial and replaced it with a new material domain for UI
	VER_UE4_REMOVED_MATERIAL_USED_WITH_UI_FLAG,
	// Added braking friction separate from turning friction.
	VER_UE4_CHARACTER_MOVEMENT_ADD_BRAKING_FRICTION,
	// Removed TTransArrays from UModel
	VER_UE4_BSP_UNDO_FIX,
	// Added default value to dynamic parameter.
	VER_UE4_DYNAMIC_PARAMETER_DEFAULT_VALUE,
	// Added ExtendedBounds to StaticMesh
	VER_UE4_STATIC_MESH_EXTENDED_BOUNDS,
	// Added non-linear blending to anim transitions, deprecating old types
	VER_UE4_ADDED_NON_LINEAR_TRANSITION_BLENDS,
	// AO Material Mask texture
	VER_UE4_AO_MATERIAL_MASK,
	// Replaced navigation agents selection with single structure
	VER_UE4_NAVIGATION_AGENT_SELECTOR,
	// Mesh particle collisions consider particle size.
	VER_UE4_MESH_PARTICLE_COLLISIONS_CONSIDER_PARTICLE_SIZE,
	// Adjacency buffer building no longer automatically handled based on triangle count, user-controlled
	VER_UE4_BUILD_MESH_ADJ_BUFFER_FLAG_EXPOSED,
	// Change the default max angular velocity
	VER_UE4_MAX_ANGULAR_VELOCITY_DEFAULT,
	// Build Adjacency index buffer for clothing tessellation
	VER_UE4_APEX_CLOTH_TESSELLATION,
	// Added DecalSize member, solved backward compatibility
	VER_UE4_DECAL_SIZE,
	// Keep only package names in StringAssetReferencesMap
	VER_UE4_KEEP_ONLY_PACKAGE_NAMES_IN_STRING_ASSET_REFERENCES_MAP,
	// Support sound cue not saving out editor only data
	VER_UE4_COOKED_ASSETS_IN_EDITOR_SUPPORT,
	// Updated dialogue wave localization gathering logic.
	VER_UE4_DIALOGUE_WAVE_NAMESPACE_AND_CONTEXT_CHANGES,
	// Renamed MakeRot MakeRotator and rearranged parameters.
	VER_UE4_MAKE_ROT_RENAME_AND_REORDER,
	// K2Node_Variable will properly have the VariableReference Guid set if available
	VER_UE4_K2NODE_VAR_REFERENCEGUIDS,
	// Added support for sound concurrency settings structure and overrides
	VER_UE4_SOUND_CONCURRENCY_PACKAGE,
	// Changing the default value for focusable user widgets to false
	VER_UE4_USERWIDGET_DEFAULT_FOCUSABLE_FALSE,
	// Custom event nodes implicitly set 'const' on array and non-array pass-by-reference input params
	VER_UE4_BLUEPRINT_CUSTOM_EVENT_CONST_INPUT,
	// Renamed HighFrequencyGain to LowPassFilterFrequency
	VER_UE4_USE_LOW_PASS_FILTER_FREQ,
	// UAnimBlueprintGeneratedClass can be replaced by a dynamic class. Use TSubclassOf<UAnimInstance> instead.
	VER_UE4_NO_ANIM_BP_CLASS_IN_GAMEPLAY_CODE,
	// The SCS keeps a list of all nodes in its hierarchy rather than recursively building it each time it is requested
	VER_UE4_SCS_STORES_ALLNODES_ARRAY,
	// Moved StartRange and EndRange in UFbxAnimSequenceImportData to use FInt32Interval
	VER_UE4_FBX_IMPORT_DATA_RANGE_ENCAPSULATION,
	// Adding a new root scene component to camera component
	VER_UE4_CAMERA_COMPONENT_ATTACH_TO_ROOT,
	// Updating custom material expression nodes for instanced stereo implementation
	VER_UE4_INSTANCED_STEREO_UNIFORM_UPDATE,
	// Texture streaming min and max distance to handle HLOD
	VER_UE4_STREAMABLE_TEXTURE_MIN_MAX_DISTANCE,	
	// Fixing up invalid struct-to-struct pin connections by injecting available conversion nodes
	VER_UE4_INJECT_BLUEPRINT_STRUCT_PIN_CONVERSION_NODES,
	// Saving tag data for Array Property's inner property
	VER_UE4_INNER_ARRAY_TAG_INFO,
	// Fixed duplicating slot node names in skeleton due to skeleton preload on compile
	VER_UE4_FIX_SLOT_NAME_DUPLICATION,
	// Texture streaming using AABBs instead of Spheres
	VER_UE4_STREAMABLE_TEXTURE_AABB,
	// FPropertyTag stores GUID of property
	VER_UE4_PROPERTY_GUID_IN_PROPERTY_TAG,
	// Name table hashes are calculated and saved out rather than at load time
	VER_UE4_NAME_HASHES_SERIALIZED,
	// Updating custom material expression nodes for instanced stereo implementation refactor
	VER_UE4_INSTANCED_STEREO_UNIFORM_REFACTOR,
	// Added compression to the shader resource for memory savings
	VER_UE4_COMPRESSED_SHADER_RESOURCES,
	// Cooked files contain the dependency graph for the event driven loader (the serialization is largely independent of the use of the new loader)
	VER_UE4_PRELOAD_DEPENDENCIES_IN_COOKED_EXPORTS,
	// Cooked files contain the TemplateIndex used by the event driven loader (the serialization is largely independent of the use of the new loader, i.e. this will be null if cooking for the old loader)
	VER_UE4_TemplateIndex_IN_COOKED_EXPORTS,
	// FPropertyTag includes contained type(s) for Set and Map properties:
	VER_UE4_PROPERTY_TAG_SET_MAP_SUPPORT,
	// Added SearchableNames to the package summary and asset registry
	VER_UE4_ADDED_SEARCHABLE_NAMES,
	// Increased size of SerialSize and SerialOffset in export map entries to 64 bit, allow support for bigger files
	VER_UE4_64BIT_EXPORTMAP_SERIALSIZES,
	// Sky light stores IrradianceMap for mobile renderer.
	VER_UE4_SKYLIGHT_MOBILE_IRRADIANCE_MAP,
	// Added flag to control sweep behavior while walking in UCharacterMovementComponent.
	VER_UE4_ADDED_SWEEP_WHILE_WALKING_FLAG,
	// StringAssetReference changed to SoftObjectPath and swapped to serialize as a name+string instead of a string
	VER_UE4_ADDED_SOFT_OBJECT_PATH,
	// Changed the source orientation of point lights to match spot lights (z axis)
	VER_UE4_POINTLIGHT_SOURCE_ORIENTATION,
	// LocalizationId has been added to the package summary (editor-only)
	VER_UE4_ADDED_PACKAGE_SUMMARY_LOCALIZATION_ID,
	// Fixed case insensitive hashes of wide strings containing character values from 128-255
	VER_UE4_FIX_WIDE_STRING_CRC,
	// Added package owner to allow private references
	VER_UE4_ADDED_PACKAGE_OWNER,
	// Changed the data layout for skin weight profile data
	VER_UE4_SKINWEIGHT_PROFILE_DATA_LAYOUT_CHANGES,
	// Added import that can have package different than their outer
	VER_UE4_NON_OUTER_PACKAGE_IMPORT,
	// Added DependencyFlags to AssetRegistry
	VER_UE4_ASSETREGISTRY_DEPENDENCYFLAGS,
	// Fixed corrupt licensee flag in 4.26 assets
	VER_UE4_CORRECT_LICENSEE_FLAG,

	// -----<new versions can be added before this line>-------------------------------------------------
	// - this needs to be the last line (see note below)
	VER_UE4_AUTOMATIC_VERSION_PLUS_ONE,
	VER_UE4_AUTOMATIC_VERSION = VER_UE4_AUTOMATIC_VERSION_PLUS_ONE - 1
};

enum<int32> EUnrealEngineObjectLicenseeUE4Version
{
	VER_LIC_NONE = 0,
	// - this needs to be the last line (see note below)
	VER_LIC_AUTOMATIC_VERSION_PLUS_ONE,
	VER_LIC_AUTOMATIC_VERSION = VER_LIC_AUTOMATIC_VERSION_PLUS_ONE - 1
};

// Custom serialization version for changes made in Dev-Framework stream
enum FFrameworkObjectVersion
{
	// Before any version changes were made
	FFrameworkObjectVersion_BeforeCustomVersionWasAdded = 0,

	// BodySetup's default instance collision profile is used by default when creating a new instance.
	FFrameworkObjectVersion_UseBodySetupCollisionProfile,

	// Regenerate subgraph arrays correctly in animation blueprints to remove duplicates and add
	// missing graphs that appear read only when edited
	FFrameworkObjectVersion_AnimBlueprintSubgraphFix,

	// Static and skeletal mesh sockets now use the specified scale
	FFrameworkObjectVersion_MeshSocketScaleUtilization,

	// Attachment rules are now explicit in how they affect location, rotation and scale
	FFrameworkObjectVersion_ExplicitAttachmentRules,

	// Moved compressed anim data from uasset to the DDC
	FFrameworkObjectVersion_MoveCompressedAnimDataToTheDDC,

	// Some graph pins created using legacy code seem to have lost the RF_Transactional flag,
	// which causes issues with undo. Restore the flag at this version
	FFrameworkObjectVersion_FixNonTransactionalPins,

	// Create new struct for SmartName, and use that for CurveName
	FFrameworkObjectVersion_SmartNameRefactor,
	
	// Add Reference Skeleton to Rig
	FFrameworkObjectVersion_AddSourceReferenceSkeletonToRig, 

	// Refactor ConstraintInstance so that we have an easy way to swap behavior paramters
	FFrameworkObjectVersion_ConstraintInstanceBehaviorParameters,

	// Pose Asset support mask per bone
	FFrameworkObjectVersion_PoseAssetSupportPerBoneMask,

	// Physics Assets now use SkeletalBodySetup instead of BodySetup
	FFrameworkObjectVersion_PhysAssetUseSkeletalBodySetup,

	// Remove SoundWave CompressionName
	FFrameworkObjectVersion_RemoveSoundWaveCompressionName,

	// Switched render data for clothing over to unreal data, reskinned to the simulation mesh
	FFrameworkObjectVersion_AddInternalClothingGraphicalSkinning,

	// Wheel force offset is now applied at the wheel instead of vehicle COM
	FFrameworkObjectVersion_WheelOffsetIsFromWheel,

	// Move curve metadata to be saved in skeleton
	// Individual asset still saves some flag - i.e. disabled curve and editable or not, but 
	// major flag - i.e. material types - moves to skeleton and handle in one place
	FFrameworkObjectVersion_MoveCurveTypesToSkeleton,
	
	// Cache destructible overlaps on save
	FFrameworkObjectVersion_CacheDestructibleOverlaps,

	// Added serialization of materials applied to geometry cache objects
	FFrameworkObjectVersion_GeometryCacheMissingMaterials,

	// Switch static & skeletal meshes to calculate LODs based on resolution-independent screen size
	FFrameworkObjectVersion_LODsUseResolutionIndependentScreenSize,

	// Blend space post load verification
	FFrameworkObjectVersion_BlendSpacePostLoadSnapToGrid,

	// Addition of rate scales to blend space samples
	FFrameworkObjectVersion_SupportBlendSpaceRateScale,

	// LOD hysteresis also needs conversion from the LODsUseResolutionIndependentScreenSize version
	FFrameworkObjectVersion_LODHysteresisUseResolutionIndependentScreenSize,

	// AudioComponent override subtitle priority default change
	FFrameworkObjectVersion_ChangeAudioComponentOverrideSubtitlePriorityDefault,

	// Serialize hard references to sound files when possible
	FFrameworkObjectVersion_HardSoundReferences,

	// Enforce const correctness in Animation Blueprint function graphs
	FFrameworkObjectVersion_EnforceConstInAnimBlueprintFunctionGraphs,
	
	// Upgrade the InputKeySelector to use a text style
	FFrameworkObjectVersion_InputKeySelectorTextStyle,

	// Represent a pins container type as an enum not 3 independent booleans
	FFrameworkObjectVersion_EdGraphPinContainerType,

	// Switch asset pins to store as string instead of hard object reference
	FFrameworkObjectVersion_ChangeAssetPinsToString,

	// Fix Local Variables so that the properties are correctly flagged as blueprint visible
	FFrameworkObjectVersion_LocalVariablesBlueprintVisible,

	// Stopped serializing UField_Next so that UFunctions could be serialized in dependently of a UClass
	// in order to allow us to do all UFunction loading in a single pass (after classes and CDOs are created):
	FFrameworkObjectVersion_RemoveUField_Next,
	
	// Fix User Defined structs so that all members are correct flagged blueprint visible 
	FFrameworkObjectVersion_UserDefinedStructsBlueprintVisible,

	// FMaterialInput and FEdGraphPin store their name as FName instead of FString
	FFrameworkObjectVersion_PinsStoreFName,

	// User defined structs store their default instance, which is used for initializing instances
	FFrameworkObjectVersion_UserDefinedStructsStoreDefaultInstance,

	// Function terminator nodes serialize an FMemberReference rather than a name/class pair
	FFrameworkObjectVersion_FunctionTerminatorNodesUseMemberReference,

	// Custom event and non-native interface event implementations add 'const' to reference parameters
	FFrameworkObjectVersion_EditableEventsUseConstRefParameters,

	// No longer serialize the legacy flag that indicates this state, as it is now implied since we don't serialize the skeleton CDO
	FFrameworkObjectVersion_BlueprintGeneratedClassIsAlwaysAuthoritative,

	// Enforce visibility of blueprint functions - e.g. raise an error if calling a private function from another blueprint:
	FFrameworkObjectVersion_EnforceBlueprintFunctionVisibility,

	// ActorComponents now store their serialization index
	FFrameworkObjectVersion_StoringUCSSerializationIndex,

	// -----<new versions can be added above this line>-------------------------------------------------
	FFrameworkObjectVersion_VersionPlusOne,
	FFrameworkObjectVersion_LatestVersion = FFrameworkObjectVersion_VersionPlusOne - 1
};

enum FCoreObjectVersion
{
    // Before any version changes were made
    FCoreObjectVersion_BeforeCustomVersionWasAdded = 0,
    FCoreObjectVersion_MaterialInputNativeSerialize,
    FCoreObjectVersion_EnumProperties,
    FCoreObjectVersion_SkeletalMaterialEditorDataStripping,
    FCoreObjectVersion_FProperties,
    
    // -----<new versions can be added above this line>-------------------------------------------------
    FCoreObjectVersion_VersionPlusOne,
    FCoreObjectVersion_LatestVersion = FCoreObjectVersion_VersionPlusOne - 1
};

#define VER_MIN_SCRIPTVM_UE4									(VER_UE4_CASE_PRESERVING_FNAME)
#define VER_MIN_SCRIPTVM_LICENSEEUE4							(VER_LIC_NONE)

/**
 * Wrapper for index into a ULnker's ImportMap or ExportMap.
 * Values greater than zero indicate that this is an index into the ExportMap.  The
 * actual array index will be (FPackageIndex - 1).
 *
 * Values less than zero indicate that this is an index into the ImportMap. The actual
 * array index will be (-FPackageIndex - 1)
 */
typedef struct
{
	/**
	 * Values greater than zero indicate that this is an index into the ExportMap.  The
	 * actual array index will be (FPackageIndex - 1).
	 *
	 * Values less than zero indicate that this is an index into the ImportMap. The actual
	 * array index will be (-FPackageIndex - 1)
	 */
	int32 Index;

    local bool IsImport = Index < 0;
    local bool IsExport = Index > 0;
    local bool IsNull = Index == 0;
    local int32 ImportIndex = IsImport ? -Index - 1 : -1;
    local int32 ExportIndex = IsExport ? Index - 1 : -1;
} FPackageIndex <read=FPackageIndex_ToString>;

typedef struct
{
	uint32 Value;
} FNameEntryId <read=FNameEntryId_ToString>;

/**
 * Public name, available to the world.  Names are stored as a combination of
 * an index into a table of unique strings and an instance number.
 * Names are case-insensitive, but case-preserving (when WITH_CASE_PRESERVING_NAME is 1)
 */
typedef struct
{
	/** Index into the Names array (used to find String portion of the string/number pair used for comparison) */
	FNameEntryId	ComparisonIndex;
#ifdef WITH_CASE_PRESERVING_NAME
	/** Index into the Names array (used to find String portion of the string/number pair used for display) */
	FNameEntryId	DisplayIndex;
#endif // WITH_CASE_PRESERVING_NAME
	/** Number portion of the string/number pair (stored internally as 1 more than actual, so zero'd memory will be the default, no-instance case) */
	uint32			Number;
} FName <read=FName_ToString>;

string FName_ToString(FName& x)
{
    return FNameEntryId_ToString(x.ComparisonIndex);
}

bool FName_Equals(FName& x, string y)
{
    return Strcmp(FName_ToString(x), y) == 0;
}

/**
 *  A tag describing a class property, to aid in serialization.
 */
typedef struct
{
	FName	Name;		// Name of property.
    if (!FName_Equals(Name, "None")) 
    {
	    FName	Type;		// Type of property
        int32	Size;   // Property size.
	    int32	ArrayIndex; // Index if an array; else 0.
    
        if (Type.Number == 0)
        {
            if (FName_Equals(Type, "StructProperty"))
            {
   	            FName	StructName;	// Struct name if FStructProperty.  
                if (GPackageFileUE4Version >= VER_UE4_STRUCT_GUID_IN_PROPERTY_TAG)
                {
        	        FGuid	StructGuid;        
                }       
            }
            else if (FName_Equals(Type, "BoolProperty"))
            {
 	            uint8	BoolVal;// a boolean property's value (never need to serialize data for bool properties except here)           
            }
            else if (FName_Equals(Type, "ByteProperty") ||
                     FName_Equals(Type, "EnumProperty"))
            {
    	        FName	EnumName;      
            }
            else if (FName_Equals(Type, "ArrayProperty"))
            {
                if (GPackageFileUE4Version >= VAR_UE4_ARRAY_PROPERTY_INNER_TAGS)
                {
               	    FName	InnerType;
                }
            }
            else if (GPackageFileUE4Version >= VER_UE4_PROPERTY_TAG_SET_MAP_SUPPORT)
            {
                if (FName_Equals(Type, "SetProperty"))
                {
                    FName	InnerType;
                }
                else if (FName_Equals(Type, "MapProperty"))
                {
                    FName	InnerType;
                    FName	ValueType;
                }
            }
        }
    
	    if (GPackageFileUE4Version >= VER_UE4_PROPERTY_GUID_IN_PROPERTY_TAG)
        {
            int8 HasPropertyGuid;
    
            if (HasPropertyGuid) 
            {
	            FGuid	PropertyGuid;
            }
        }

        byte Data[Size];
    }
} FPropertyTag <read=FPropertyTag_ToString>;

string FPropertyTag_ToString(FPropertyTag& x)
{
    return FName_ToString(x.Name);
}

local bool HasUnversionedProperties = false;

typedef struct
{
    local bool IsStaticClass = false;
} UClass;

typedef struct
{
    const bool IsLoading = true;
    const bool IsSaving = false;
    const bool IsObjectReferenceCollector = false;
    const bool IsIgnoringOuterRef = false;
    const bool IsTransacting = false;
    const bool IsTextFormat = false;
    const bool WantBinaryPropertySerialization = true;
    const bool UseVersionedPropertySerialization = false;
    const int PortFlags = 0;
    const bool ArUseCustomPropertyList = false;
        
    

    if (!IsLoading && !IsSaving && !IsObjectReferenceCollector)
    {
        FName LoadName;
        struct UObject LoadOuter;
        struct UClass ObjClass;
    }
    else if (IsTransacting && !IsIgnoringOuterRef)
    {
        FName LoadName;
        struct UObject LoadOuter;
        struct UClass LoadPackage;
    }
    else
    {
        local struct UClass ObjClass;
    }

    if (!ObjClass.IsStaticClass) 
    {
        // SerializeScriptProperties()
        if(IsTextFormat || ((IsLoading || IsSaving) && WantBinaryPropertySerialization))
        {
            // ObjClass->SerializeTaggedProperties()
            if (UseVersionedPropertySerialization)
            {
                // SerializeUnversionedProperties
            }
            else
            {
                // SerializeVersionedTaggedProperties
                while (true)
                {
                    FPropertyTag Tag;
                    if (FName_Equals(Tag.Name, "None"))
                        break;

                    // SerializeTaggedProperty
                    byte Data[Tag.Size];
                }
            }
            
        }
        else if (PortFlags != 0 && ArUseCustomPropertyList)
        {
            // ObjClass->SerializeBinEx
        }
        else
        {
            // ObjClass->SerializeBin
        }
    }

    if (IsTransacting)
    {
        bool WasKill;
    }

    //PossiblySerializeObjectGuid();
} UObject;

typedef struct
{
    UObject Base;
    local FFrameworkObjectVersion FrameworkObjectVersion = FFrameworkObjectVersion_LatestVersion;
    if (FrameworkObjectVersion < FFrameworkObjectVersion_RemoveUField_Next)
    {
        struct UField Next;
    }
} UField;

typedef struct
{
    UField Base;
    struct UStruct SuperStruct;
    local FFrameworkObjectVersion FrameworkObjectVersion = FFrameworkObjectVersion_LatestVersion;
    local FCoreObjectVersion CoreObjectVersion = FCoreObjectVersion_LatestVersion;
    if (FrameworkObjectVersion < FFrameworkObjectVersion_RemoveUField_Next)
    {
        struct UField Children;
    }
    else
    {
        TArray Children(T_UField);
    }

    if (CoreObjectVersion >= FCoreObjectVersion_FProperties)
    {
        // SerializeProperties
    }

    int32 ScriptBytecodeSize;
    int32 ScriptStorageSize;
    local uint64 ScriptStartOffset = FTell();

	//const bool bIsLinkerLoader = Ar.IsPersistent() && (Ar.GetLinker() != nullptr);
	const uint64 ScriptEndOffset = ScriptStartOffset + ScriptBytecodeSize;
    local bool bSkipScriptSerialization = (UE4Ver() < VER_MIN_SCRIPTVM_UE4) || (LicenseeUE4Ver() < VER_MIN_SCRIPTVM_LICENSEEUE4);
    if (bSkipScriptSerialization)
    {
        //
    }
} UStruct;
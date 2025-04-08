// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetData.h"

bool FSTGenericGameplayAbilityTargetData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	
	enum
	{
		TagFlag = 1 <<0,
		ObjectsFlag = 1 << 1,
		VectorsFlag = 1 << 3,
		FloatsFlag = 1 << 4,
		EnumsFlag = 1 <<5,
		IntsFlag = 1 <<6,
		NamesFlag = 1 <<7
	};

	uint8 Flag = 0;
	if (Ar.IsSaving())
	{
		if (!TagContainer.IsEmpty())
		{
			Flag |= TagFlag;
		}
		if (!Objects.IsEmpty())
		{
			Flag |= ObjectsFlag;
		}
		if (!Vectors.IsEmpty())
		{
			Flag |= VectorsFlag;
		}
		if (!Floats.IsEmpty())
		{
			Flag |= FloatsFlag;
		}
		if (!Enums.IsEmpty())
		{
			Flag |= EnumsFlag;
		}
		if (!Ints.IsEmpty())
		{
			Flag |= IntsFlag;
		}
		if (!Names.IsEmpty())
		{
			Flag |= NamesFlag;
		}
	}
	Ar << Flag;
	
	if (Flag & TagFlag)
	{
		TagContainer.NetSerialize(Ar, Map, bOutSuccess);
	}
	else
	{
		TagContainer.Reset();
	}

	if (Flag & ObjectsFlag)
	{
		Ar << Objects;
	}
	else
	{
		Objects.Empty();
	}

	if (Flag & VectorsFlag)
	{
		Ar << Vectors;
	}
	else
	{
		Vectors.Empty();
	}

	if (Flag & FloatsFlag)
	{
		Ar << Floats;
	}
	else
	{
		Floats.Empty();
	}

	if (Flag & EnumsFlag)
	{
		Ar << Enums;
	}
	else
	{
		Enums.Empty();
	}

	if (Flag & IntsFlag)
	{
		Ar << Ints;
	}
	else
	{
		Ints.Empty();
	}

	if (Flag & NamesFlag)
	{
		Ar << Names;
	}
	else
	{
		Names.Empty();
	}

	bOutSuccess = !Ar.IsError();
	return bOutSuccess;
}

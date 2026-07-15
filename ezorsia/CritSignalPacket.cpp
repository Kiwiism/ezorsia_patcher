#include "stdafx.h"
#include "CritSignalPacket.h"

namespace
{
	constexpr DWORD kMeleeEncodeSite = 0x00952B28;
	constexpr DWORD kShootEncodeSite = 0x00955428;
	constexpr DWORD kMagicEncodeSite = 0x0095705E;
	constexpr DWORD kEncode4Address = 0x004065A6;
	constexpr int kCriticalFlagOffset = 0x3C;

	DWORD g_encode4Address = kEncode4Address;

	bool CallTargets(DWORD address, DWORD expectedTarget)
	{
		__try
		{
			if (*reinterpret_cast<const unsigned char*>(address) != 0xE8)
			{
				return false;
			}

			const int relative = *reinterpret_cast<const int*>(address + 1);
			return address + 5 + relative == expectedTarget;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	void WriteRelativeCall(DWORD address, void* destination)
	{
		Memory::WriteByte(address, 0xE8);
		Memory::WriteInt(address + 1,
			static_cast<unsigned int>(reinterpret_cast<uintptr_t>(destination) - address - 5));
	}

	__declspec(naked) void MeleeCritEncode4()
	{
		__asm
		{
			mov edx, [eax + kCriticalFlagOffset]
			test edx, edx
			jz no_crit
			mov edx, [esp + 4]
			add edx, 0x80000001
			mov[esp + 4], edx

		no_crit:
			jmp dword ptr[g_encode4Address]
		}
	}

	__declspec(naked) void RangedCritEncode4()
	{
		__asm
		{
			mov edx, [edi + kCriticalFlagOffset]
			test edx, edx
			jz no_crit
			mov edx, [esp + 4]
			add edx, 0x80000001
			mov[esp + 4], edx

		no_crit:
			jmp dword ptr[g_encode4Address]
		}
	}

	bool ValidateHookSites()
	{
		return CallTargets(kMeleeEncodeSite, kEncode4Address)
			&& CallTargets(kShootEncodeSite, kEncode4Address)
			&& CallTargets(kMagicEncodeSite, kEncode4Address);
	}
}

bool CritSignalPacket::Apply()
{
	if (!ValidateHookSites())
	{
		std::cout << "Crit signal packet patch disabled: runtime signature mismatch" << std::endl;
		return false;
	}

	WriteRelativeCall(kMeleeEncodeSite, MeleeCritEncode4);
	WriteRelativeCall(kShootEncodeSite, RangedCritEncode4);
	WriteRelativeCall(kMagicEncodeSite, RangedCritEncode4);

	std::cout << "Crit signal packet patch enabled" << std::endl;
	return true;
}

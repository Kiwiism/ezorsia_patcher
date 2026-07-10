#include "stdafx.h"
#include "ShadowPartnerAttacks.h"

namespace
{
	constexpr DWORD kMeleeLineCountAddress = 0x00951C00;
	constexpr DWORD kMeleeLineCountReturn = 0x00951C08;
	constexpr DWORD kPhysicalDamageCallAddress = 0x00951E42;
	constexpr DWORD kPhysicalDamageAddress = 0x0078DF87;
	constexpr DWORD kMagicLineCountAddress = 0x00955B8C;
	constexpr DWORD kMagicLineCountReturn = 0x00955B94;
	constexpr DWORD kMagicDamageLoopInitAddress = 0x00791864;
	constexpr DWORD kMagicDamageLoopBody = 0x0079186A;
	constexpr DWORD kMagicDamageAdvanceAddress = 0x00791FA8;
	constexpr DWORD kMagicDamageLoopContinue = 0x00791FAF;
	constexpr DWORD kWvsContextAddress = 0x00BE7918;
	constexpr DWORD kZtlSecureFuseLongAddress = 0x00416563;
	constexpr int kMaxBaseLinesWithPartner = 7;

	DWORD g_meleeLineCountReturn = kMeleeLineCountReturn;
	DWORD g_magicLineCountReturn = kMagicLineCountReturn;
	DWORD g_magicDamageLoopBody = kMagicDamageLoopBody;
	DWORD g_magicDamageLoopContinue = kMagicDamageLoopContinue;
	volatile LONG g_magicBaseLineCount = 0;
	volatile LONG g_magicTotalLineCount = 0;
	volatile LONG g_shadowPartnerPercent = 0;

	using ZtlSecureFuseLong = long(__cdecl*)(long*, unsigned int);
	using PhysicalDamage = int(__thiscall*)(
		void*, void*, void*, void*, unsigned int, void*, void*, int, int, int, int, int,
		int, void*, int, int*, int*, int, int, int, int);

	auto ZtlSecureFuse = reinterpret_cast<ZtlSecureFuseLong>(kZtlSecureFuseLongAddress);
	auto CalculatePhysicalDamage = reinterpret_cast<PhysicalDamage>(kPhysicalDamageAddress);

	int __stdcall GetShadowPartnerPercent()
	{
		__try
		{
			void* context = *reinterpret_cast<void**>(kWvsContextAddress);
			if (context == nullptr)
			{
				return 0;
			}

			const uintptr_t secondaryStat = reinterpret_cast<uintptr_t>(context) + 0x2134;
			const long value = ZtlSecureFuse(
				reinterpret_cast<long*>(secondaryStat + 0x42C),
				*reinterpret_cast<unsigned int*>(secondaryStat + 0x434));
			return value > 0 && value <= 100 ? value : 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return 0;
		}
	}

	bool BytesMatch(DWORD address, const unsigned char* expected, size_t size)
	{
		__try
		{
			return std::memcmp(reinterpret_cast<const void*>(address), expected, size) == 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

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

	__declspec(naked) void MeleeLineCountHook()
	{
		__asm
		{
			pushad
			call GetShadowPartnerPercent
			test eax, eax
			popad
			jz no_shadow_partner

			cmp eax, kMaxBaseLinesWithPartner
			jle double_lines
			mov eax, kMaxBaseLinesWithPartner

		double_lines:
			add eax, eax

		no_shadow_partner:
			mov[ebp - 0x58], eax
			jmp dword ptr[g_meleeLineCountReturn]
		}
	}

	int __fastcall PhysicalDamageHook(
		void* instance, void*, void* characterData, void* basicStat, void* secondaryStat,
		unsigned int mobId, void* mobStat, void* mobTemplate, int damagePerMob,
		int weaponItemId, int bulletItemId, int attackType, int action, int shadowPartner,
		void* skill, int skillLevel, int* damage, int* critical, int keyDown,
		int darkForce, int advancedChargeDamage, int invincible)
	{
		shadowPartner = GetShadowPartnerPercent() > 0 ? 1 : 0;
		return CalculatePhysicalDamage(
			instance, characterData, basicStat, secondaryStat, mobId, mobStat, mobTemplate,
			damagePerMob, weaponItemId, bulletItemId, attackType, action, shadowPartner,
			skill, skillLevel, damage, critical, keyDown, darkForce, advancedChargeDamage,
			invincible);
	}

	__declspec(naked) void MagicLineCountHook()
	{
		__asm
		{
			push eax
			pushad
			call GetShadowPartnerPercent
			mov g_shadowPartnerPercent, eax
			popad
			pop eax

			cmp g_shadowPartnerPercent, 0
			jz no_shadow_partner

			cmp eax, kMaxBaseLinesWithPartner
			jle save_base_lines
			mov eax, kMaxBaseLinesWithPartner

		save_base_lines:
			mov g_magicBaseLineCount, eax
			add eax, eax
			jmp store_line_count

		no_shadow_partner:
			mov g_magicBaseLineCount, 0
			cmp eax, 15
			jle store_line_count
			mov eax, 15

		store_line_count:
			mov[ebp - 0x64], eax
			jmp dword ptr[g_magicLineCountReturn]
		}
	}

	__declspec(naked) void MagicDamageLoopInitHook()
	{
		__asm
		{
			mov[ebp + 0x34], eax
			mov g_magicTotalLineCount, ecx
			mov[ebp - 0x34], ecx
			jmp dword ptr[g_magicDamageLoopBody]
		}
	}

	__declspec(naked) void MagicDamageAdvanceHook()
	{
		__asm
		{
			push ebx
			push esi
			push edi

			mov ebx, g_magicBaseLineCount
			test ebx, ebx
			jz advance

			mov edi, g_magicTotalLineCount
			mov eax, ebx
			add eax, eax
			cmp edi, eax
			jne advance

			mov eax, edi
			sub eax, [ebp - 0x34]
			cmp eax, ebx
			jb advance

			mov esi, [ebp + 0x40]
			mov ecx, ebx
			shl ecx, 2
			sub esi, ecx
			mov eax, [esi]
			test eax, eax
			jle advance

			mov ecx, g_shadowPartnerPercent
			test ecx, ecx
			jle advance
			imul eax, ecx
			mov ecx, 100
			cdq
			idiv ecx
			cmp eax, 1
			jge write_partner_damage
			mov eax, 1

		write_partner_damage:
			mov esi, [ebp + 0x40]
			mov[esi], eax

		advance:
			pop edi
			pop esi
			pop ebx
			add dword ptr[ebp + 0x40], 4
			dec dword ptr[ebp - 0x34]
			jmp dword ptr[g_magicDamageLoopContinue]
		}
	}

	bool ValidateHookSites()
	{
		const unsigned char meleeLineCount[] = { 0x89, 0x45, 0xA8 };
		const unsigned char magicLineCount[] = { 0x89, 0x45, 0x9C };
		const unsigned char magicLoopInit[] = { 0x89, 0x45, 0x34, 0x89, 0x4D, 0xCC };
		const unsigned char magicAdvance[] = { 0x83, 0x45, 0x40, 0x04, 0xFF, 0x4D, 0xCC };

		return BytesMatch(kMeleeLineCountAddress, meleeLineCount, sizeof(meleeLineCount))
			&& CallTargets(kPhysicalDamageCallAddress, kPhysicalDamageAddress)
			&& BytesMatch(kMagicLineCountAddress, magicLineCount, sizeof(magicLineCount))
			&& BytesMatch(kMagicDamageLoopInitAddress, magicLoopInit, sizeof(magicLoopInit))
			&& BytesMatch(kMagicDamageAdvanceAddress, magicAdvance, sizeof(magicAdvance));
	}
}

bool ShadowPartnerAttacks::Apply()
{
	if (!ValidateHookSites())
	{
		std::cout << "Shadow Partner melee/magic patch disabled: runtime signature mismatch" << std::endl;
		return false;
	}

	Memory::CodeCave(MeleeLineCountHook, kMeleeLineCountAddress, 8);
	WriteRelativeCall(kPhysicalDamageCallAddress, PhysicalDamageHook);
	Memory::CodeCave(MagicLineCountHook, kMagicLineCountAddress, 8);
	Memory::CodeCave(MagicDamageLoopInitHook, kMagicDamageLoopInitAddress, 6);
	Memory::CodeCave(MagicDamageAdvanceHook, kMagicDamageAdvanceAddress, 7);

	std::cout << "Shadow Partner melee/magic attacks enabled" << std::endl;
	return true;
}

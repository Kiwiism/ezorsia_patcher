#pragma once

#pragma region AdventureMS Gameplay Changes

#pragma region Custom Skill Work

// Custom skill addresses - Gwen
int doActiveJmpBack = 0x0096793B; // Jump Location
int aBuff = 0x00969284; // Works for Buffs
int aMelee = 0x009690AE; // Works for LTRB Handling
int aMagic = 0x0096928B;
int aShoot = 0x009690E9;
int aPrepare = 0x00969229; // Untested
int aTeleport = 0x00969146; // Untested
int aFlying = 0x009683ED; // Untested
int aRecovery = 0x00969217; // Untested
int aBoundJump = 0x0096897A; // Untested
int aSummon = 0x009689DF; // Untested
int aCombatStep = 0x00969026; // Untested

// Code cave for custom skills - Gwen
void __declspec(naked) doActiveSkills()
{
	__asm
	{
		// Add a Custom Skill
		// mov eax, yourSkillId
		// cmp esi, eax
		// je[yourSkillType]

		// List of Custom Skills
		mov eax, 1101008
		cmp esi, eax
		je[melee]
		mov eax, 4201006
		cmp esi, eax
		je[melee]
		mov eax, 1201008
		cmp esi, eax
		je[melee]
		mov eax, 1301008
		cmp esi, eax
		je[melee]
		mov eax, 5001008
		cmp esi, eax
		je[melee]
		mov eax, 5101008
		cmp esi, eax
		je[melee]
		mov eax, 4101006
		cmp esi, eax
		je[shoot]
		mov eax, 1301009
		cmp esi, eax
		je[buff]
		mov eax, 1201009
		cmp esi, eax
		je[buff]
		mov eax, 3101006
		cmp esi, eax
		je[buff]
		mov eax, 3201006
		cmp esi, eax
		je[buff]
		mov eax, 5101009
		cmp esi, eax
		je[buff]
		mov eax, 5201009
		cmp esi, eax
		je[buff]

		// Jump back to the original location.
		mov eax, 2301005
		jmp[doActiveJmpBack]

		// Skill-type jump locations.
	buff:
		jmp[aBuff]
	melee:
		jmp[aMelee]
	magic:
		jmp[aMagic]
	shoot:
		jmp[aShoot]
	prep:
		jmp[aPrepare]
	tele:
		jmp[aTeleport]
	fly:
		jmp[aFlying]
	recover:
		jmp[aRecovery]
	jump:
		jmp[aBoundJump]
	summon:
		jmp[aSummon]
	combat:
		jmp[aCombatStep]
	}
}

// Add custom skill IDs here.
bool isSkillIDMatched(int nSkillID)
{
	const int skillIDs[] =
	{
		1101008, 1201008, 1301008, 1301009, 1201009, 3201006, 3101006, 4101006, 4201006, 5101009, 5201009, 5001008, 5101008
	};

	return std::find(std::begin(skillIDs), std::end(skillIDs), nSkillID) != std::end(skillIDs);
}

// Route matching custom skills through the custom skill code cave.
auto pDoActiveSkill = (int(__thiscall*)(int, int, int, int))0x00966F7A;
int(__fastcall CUserLocal__DoActiveSkill_t)(int _This, void* edx, int nSkillID, unsigned int nScanCode, int pnConsumeCheck)
{
	if (isSkillIDMatched(nSkillID))
	{
		Memory::CodeCave((void*)doActiveSkills, 0x0096792A, 0);
	}
	else
	{
		Memory::WriteByte(0x0096792A, 0x0F);
		Memory::WriteByte(0x0096792A + 1, 0x8F);
		Memory::WriteByte(0x0096792A + 2, 0x71);
		Memory::WriteByte(0x0096792A + 3, 0x09);
		Memory::WriteByte(0x0096792A + 4, 0x00);
	}
	return pDoActiveSkill(_This, nSkillID, nScanCode, pnConsumeCheck);
}

#pragma endregion

#pragma region Existing Skill Modifications

// LTRB handling for non-bullet skills.
typedef int(__cdecl* is_attack_area_set_by_data_t)(int nSkillID);
is_attack_area_set_by_data_t _is_attack_area_set_by_data;
int __cdecl is_attack_area_set_by_data(int nSkillID)
{
	// Add skills requiring LTRB handling here.
	if (nSkillID == 4001002 || nSkillID == 3201003 || nSkillID == 4101006 || nSkillID == 3201005)
	{
		return 1;
	}
	return _is_attack_area_set_by_data(nSkillID);
}

// LTRB handling for custom shoot skills.
typedef int(__cdecl* is_rect_attack_shoot_skill_t)(int nSkillID);
is_rect_attack_shoot_skill_t _is_rect_attack_shoot_skill;
int __cdecl is_rect_attack_shoot_skill(int nSkillID)
{
	// Add shoot skills requiring LTRB handling here.
	if (nSkillID == 4101006 || nSkillID == 3201005)
	{
		return 1;
	}
	return _is_rect_attack_shoot_skill(nSkillID);
}

// LTRB Handling for Bullet Skills
DWORD dwFireArrow = 0x00955DA8;
DWORD dwFireArrowRet = 0x00955DAD;
DWORD dwFireSucc = 0x00956372;
__declspec(naked) void FireArrow() {
	__asm {

		cmp eax, 2101005 // Poison Breath
		je success
		cmp eax, 2301005 // Holy Ray
		je success
		cmp eax, 4101006 // Shuriken MAX
		je success
		cmp eax, 3201005 // Iron Arrow
		je success
		cmp eax, 0x0021E3CB
		jmp dwFireArrowRet
		success :
		jmp dwFireSucc
	}
}

// LTRB Handling for Bullet Skills
DWORD dwFireBulletAdd = 0x00956445;
DWORD dwFireBulletSucc = 0x0095645B;
DWORD dwFireBulletRet = 0x0095644E;
__declspec(naked) void FireArrowBullet() {
	__asm {
		cmp dword ptr[ebp - 0x14], 2101005 // Poison Breath
		je success
		cmp dword ptr[ebp - 0x14], 2301005 // Holy Ray
		je success
		cmp dword ptr[ebp - 0x14], 4101006 // Shuriken MAX
		je success
		cmp dword ptr[ebp - 0x14], 3201005 // Iron Arrow
		je success
		jmp dwFireBulletRet
		success :
		jmp dwFireBulletSucc
	}
}

// Reduced CD: Recoil Shot
DWORD dwRecoilShot = 0x00953646;
DWORD dwRecoilShotRet = 0x0095364D;
DWORD dwRecoilShotSucc = 0x00953669;
DWORD recoilShotCooldownMs = PatchSettings::RecoilShotCooldownMs;
__declspec(naked) void RecoilShotLowerCD() {
	__asm {
		cmp eax, 5201006
		je success
		jmp dwRecoilShotRet
		success :
		mov eax, dword ptr[recoilShotCooldownMs]
		jmp dwRecoilShotSucc
	}
}


// Remove Charge: Big Bang Bishop
DWORD dwBB = 0x00967ECC;
DWORD dwBBReturn = 0x00967ED2;
DWORD dwBBS = 0x0096928B;
__declspec(naked) void BigBang() {
	__asm
	{
		je BBS
		jmp dword ptr[dwBBReturn]
		BBS:
		jmp dword ptr[dwBBS]
	}
}


// Remove Charge: Big Bang IL
DWORD dwBB2 = 0x00967DEE;
DWORD dwBB2Return = 0x00967DF4;
DWORD dwBBS2 = 0x0096928B;
__declspec(naked) void BigBang2() {
	__asm
	{
		je BBS2
		jmp dword ptr[dwBB2Return]
		BBS2:
		jmp dword ptr[dwBBS2]
	}
}

// Remove Charge: Big Bang FP
DWORD dwBB3 = 0x00967D10;
DWORD dwBB3Return = 0x00967D16;
DWORD dwBBS3 = 0x0096928B;
__declspec(naked) void BigBang3() {
	__asm
	{
		je BBS3
		jmp dword ptr[dwBB3Return]
		BBS3:
		jmp dword ptr[dwBBS3]
	}
}

// Remove Charge: Piercing Arrow
DWORD dwPA = 0x00968048;
DWORD dwPAReturn = 0x00967B8B;
DWORD dwPAS = 0x009690E9;
__declspec(naked) void PA() {
	__asm {
		je PAS
		jmp dword ptr[dwPAReturn]
		PAS:
		jmp dword ptr[dwPAS]
	}
}

// Remove Charge: Corkscrew Blow
DWORD dwCB = 0x00968278;
DWORD dwCBReturn = 0x00967B8B;
DWORD dwCBS = 0x009690AE;
__declspec(naked) void CorkscrewBlow() {
	__asm {
		je CBS
		jmp dword ptr[dwCBReturn]
		CBS:
		jmp dword ptr[dwCBS]
	}
}

#pragma endregion

#pragma endregion

#pragma region AdventureMS QoL

// Rechargeable Arrows - Gwen
void CreateCodecaveAt(DWORD patchAddress, DWORD jmpto, DWORD jmpback) {
	BYTE* cave = (BYTE*)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!cave) return;

	BYTE* p = cave;

	// cmp eax, 206
	*p++ = 0x3D; *p++ = 0xCE; *p++ = 0x00; *p++ = 0x00; *p++ = 0x00; // cmp eax, 206
	*p++ = 0x0F; *p++ = 0x84;                                        // je
	DWORD reljmp = jmpto - ((DWORD)p + 4);
	memcpy(p, &reljmp, 4); p += 4;

	// cmp eax, 207
	*p++ = 0x3D; *p++ = 0xCF; *p++ = 0x00; *p++ = 0x00; *p++ = 0x00;
	*p++ = 0x0F; *p++ = 0x84;
	reljmp = jmpto - ((DWORD)p + 4);
	memcpy(p, &reljmp, 4); p += 4;

	// cmp eax, 233
	*p++ = 0x3D; *p++ = 0xE9; *p++ = 0x00; *p++ = 0x00; *p++ = 0x00;
	*p++ = 0x0F; *p++ = 0x84;
	reljmp = jmpto - ((DWORD)p + 4);
	memcpy(p, &reljmp, 4); p += 4;

	// jmp back
	*p++ = 0xE9;
	reljmp = jmpback - ((DWORD)p + 4);
	memcpy(p, &reljmp, 4);

	// Final jump from original address to this codecave
	DWORD relToCave = (DWORD)cave - patchAddress - 5;
	BYTE patch[5] = { 0xE9 };
	memcpy(patch + 1, &relToCave, 4);
	WriteProcessMemory(GetCurrentProcess(), (void*)patchAddress, patch, 5, nullptr);
}

DWORD GetNearJumpTarget(DWORD instrAddr) {
	BYTE* bytes = reinterpret_cast<BYTE*>(instrAddr);
	if ((bytes[0] == 0x0F && (bytes[1] == 0x84 || bytes[1] == 0x85))) { // JE or JNZ
		int32_t relOffset = *reinterpret_cast<int32_t*>(bytes + 2);
		return instrAddr + 6 + relOffset;
	}
	return 0;
}

BYTE ReadByteAt(DWORD address) {
	return *(BYTE*)address;
}

void replace() {
	const char* searchAoB = "3D CF 00 00 00";
	unsigned int address;
	int skipCount = 0;
	std::vector<DWORD> foundAddresses;


	while ((address = Memory::FindAoB(searchAoB, 0x00400000, 0x007AAAAA, skipCount)) != 0) {
		foundAddresses.push_back((DWORD)address);
		skipCount++;
	}

	for (DWORD addr : foundAddresses) {
		DWORD jmpto = addr + 7 + ReadByteAt(addr + 6);
		DWORD returnAfter = addr + 7;
		CreateCodecaveAt(addr, jmpto, returnAfter);
	}
}
DWORD Bypass3 = 0x00755A24;
DWORD Bypass2 = 0x00755B4E;
DWORD Bypass4 = 0x00756C8B;

void RechargeArrows() {
	replace(); // Include arrows in bullet and throwing-star shop checks.
	Memory::PatchNop(Bypass2, 6); // Draw the recharge button for all ammunition types.
	Memory::PatchNop(Bypass3, 6); // Draws the recharge costs
	Memory::PatchNop(Bypass4, 6); // Skip the redundant full-ammunition check.
}

// Maker Categories fix - Gwen
DWORD use = 0x0082313A;
DWORD setup = 0x00823136;
DWORD etc = 0x0082312F;
DWORD cash = 0x0082314D;

__declspec(naked) void other()
{
	__asm
	{
		push 5467
		jmp cash
	}
}

__declspec(naked) void makerStuff()
{
	__asm
	{
		sub eax, 200
		je used

		sub    eax, ecx
		je setupd

		sub eax, ecx
		je etcd

		jmp other

		used : jmp  use
		setupd : jmp setup
		etcd : jmp etc
	}
}

// Disable Auto Assign Button
DWORD autoAssign_ret = 0x008C5926;
__declspec(naked) void autoAssignRemove()
{
	_asm {
		lea ecx, [eax + 4]
		mov eax, [ecx]
		push 0
		jmp autoAssign_ret
	}
}

// World Map to middle of screen
int wordMapX, wordMapY;
DWORD wordMapUIccRtn = 0x009EB5A1;
__declspec(naked) void wordMapUIcc()
{
	__asm {
		push 20Ch
		push 29Ah
		push wordMapY
		push wordMapX
		jmp wordMapUIccRtn
	}
}

#pragma endregion

#pragma region AdventureMS Fixes

// Fix Mouse Scroll Issue
DWORD fixMouseWheelAddr = 0x009E8090;
DWORD fixMouseWheelRetJmpAddr = 0x009E809F;
DWORD SetCursorVectorPos = 0x0059A0CB;
__declspec(naked) void fixMouseWheelHook() {
	__asm {
		// is mouse wheel
		cmp eax, 522
		je[halo3]
		mov eax, dword ptr[edi]
		shr eax, 0x10
		push eax
		movzx eax, word ptr[edi]
		push eax
		call SetCursorVectorPos
		halo3 :
		jmp[fixMouseWheelRetJmpAddr]
	}
}

// Align Chat Box Text
DWORD chat_Y_offset_retn = 0x008DD6BE;
DWORD call_func_chat_cave = 0x00403382;
__declspec(naked) void chat_Y_offset_cave()
{
	_asm {
		call call_func_chat_cave
		push eax
		mov eax, [ebp - 28h]
		dec eax
		mov[ebp - 28h], eax
		pop eax
		jmp chat_Y_offset_retn
	}
}

// Align Monster Book Text
DWORD pos1Rtn = 0x0086437D;
DWORD pos2Rtn = 0x0086449A;
DWORD pos3Rtn = 0x008645B9;
__declspec(naked) void pos1()
{
	__asm {
		push 146
		sub eax, edi
		push eax
		jmp pos1Rtn
	}
}
__declspec(naked) void pos2()
{
	__asm {
		push 180
		sub eax, edi
		push eax
		jmp pos2Rtn
	}
}
__declspec(naked) void pos3()
{
	__asm {
		push 200
		sub eax, edi
		push eax
		jmp pos3Rtn
	}
}

#pragma endregion

#pragma region AdventureMS Unused

/*
//Increase face limit
DWORD dwFace = 0x005C94F3;
DWORD dwFaceRet = 0x005C9505;
DWORD dwFaceAddy = 0x005C95BF;
DWORD dwAddyIDK = 0x005C958D;
Cave(FaceCave) {
	cmp eax, 0x02
		je addyFace
		cmp eax, 0x05
		je addyFace
		cmp eax, 0x07
		je addyFace
		cmp eax, 0x08
		jle addyIDK
		jmp dword ptr[dwFaceRet]

		addyFace:
	jmp dword ptr[dwFaceAddy]

		addyIDK :
		jmp dword ptr[dwAddyIDK]
}
_EndCave
//Increase face limit (npc)
DWORD dwFaceNPC = 0x009ACA99;
DWORD dwFaceNPCret = 0x009ACAA4;
DWORD dwFaceAddy2 = 0x009ACAA0;
Cave(FaceNPC) {
	idiv ecx
		cmp eax, 0x02
		je addyFace2
		cmp eax, 0x05
		je addyFace2
		cmp eax, 0x07
		je addyFace2
		cmp eax, 0x08
		je addyFace2
		jmp dword ptr[dwFaceNPCret]

		addyFace2:
	jmp dword ptr[dwFaceAddy2]
}
_EndCave
*/

/*
// Trinket Slot (Shoulder)
int equipGridMapX[] = { 5, 38, 71, 104, 137 };
int equipGridMapY[] = { 35, 68, 100, 132, 166 , 197 , 233 };
int equipGridMap[7][5] = { {0, 1, 0, 53, 54}, {49, 2, 0, 15, 16}, {0, 0, 3, 4, 51}, {9, 5, 17, 11, 10}, {8, 6, 50, 12, 13}, {0, 0, 7, 0, 0}, {18, 19, 20, 0, 0} };
int getEquipGridPosition(int cursorX, int cursorY)
{
	const int padding = 0;
	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			int topLeftX = equipGridMapX[x];
			int topLeftY = equipGridMapY[y];
			int bottomRightX = topLeftX + 32 + (padding * 2);
			int bottomRightY = topLeftY + 32 + (padding * 2);
			if (cursorX >= topLeftX && cursorX < bottomRightX && cursorY >=
				topLeftY && cursorY < bottomRightY)
			{
				return equipGridMap[y][x];
			}
		}
	}

	return 0;
}

std::pair<int, int> getPixelPositionFromNumber(int number)
{
	for (int y = 0; y < 7; ++y)
	{
		for (int x = 0; x < 5; ++x)
		{
			if (equipGridMap[y][x] == number)
			{
				// Scrape our array for the raw base coord data
				int pixelX = equipGridMapX[x];
				int pixelY = equipGridMapY[y] + 32;
				return { pixelX, pixelY };
			}
		}
	}

	return { -1, -1 }; // Invalid req, dump it off-screen
}

int getPixelPositionYFromNumber(int number) { return getPixelPositionFromNumber(number).second; }
int getPixelPositionXFromNumber(int number) { return getPixelPositionFromNumber(number).first; }

DWORD putEquipPosition_Hook_Ret = 0x007FEF58;
_declspec(naked) void putEquipPosition_Hook()
{
	_asm {
		mov eax, [ebp + 0x8];
		push eax;
		call getPixelPositionYFromNumber;
		mov edi, eax;
		pop eax;
		mov eax, [ebp + 0x8];
		push edi;
		push eax;
		call getPixelPositionXFromNumber;
		mov edi, eax;
		pop eax;
		push edi;
		jmp[putEquipPosition_Hook_Ret];
	}
}

End Trinket Changes(Shoulder)

*/

/*
// Set Base Attack Speed to 0 // Doesn't Work
DWORD attSpdCap = 0x00765086;
__declspec(naked) void setAttSpdCap()
{
	__asm {
		add[ebp + 0x08], edx
		xor eax, eax // set to 0
		cmp[ebp + 0x08], eax
		jmp attSpdCap
	}
}

// High Version Hair and Face
DWORD faceRtn = 0x005C95BF;
DWORD hairRtn = 0x005C958D;
DWORD faceHairCaveRtn = 0x005C9505;
DWORD faceHairCave2Rtn = 0x009ACAA0;
DWORD faceHairCave2Jmp = 0x009ACAA4;
DWORD faceHairCave3Rtn = 0x009ACAAC;
__declspec(naked) void faceHairCave()
{
	__asm {
		cmp eax, 2
		jz label_face
		cmp eax, 3
		jz label_hair
		cmp eax, 4
		jz label_hair
		cmp eax, 5
		jz label_face
		cmp eax, 6
		jz label_hair

		jmp faceHairCaveRtn

		label_face :
		jmp faceRtn

			label_hair :
		jmp hairRtn
	}
}
__declspec(naked) void faceHairCave2()
{
	__asm {
		cmp eax, 2
		jz label
		cmp eax, 5
		jz label
		jmp faceHairCave2Jmp

		label :
		jmp faceHairCave2Rtn
	}
}
__declspec(naked) void faceHairCave3()
{
	__asm {
		cmp eax, 3
		jz label
		cmp eax, 4
		jz label
		cmp eax, 6

		label:
		setnz cl
			jmp faceHairCave3Rtn
	}

// Hair / Face Uncap
DWORD dwHairFaceUncap1 = 0x005C94F3;
DWORD dwHairFaceUncap2 = 0x009ACA9B;
DWORD dwHairFaceUncapCapRetn = 0x005C9505;
DWORD dwHairFaceUncapFaceRetn = 0x005C95BF;
DWORD dwHairFaceUncapHairRetn = 0x005C958D;
DWORD dwHairFaceUncapRetn = 0x009ACAAD;
__declspec(naked) void HairFaceIdUncap1()
{
	__asm {
		cmp eax, 0x2
		je FACE_RET
		cmp eax, 0x5
		je FACE_RET
		cmp eax, 0x3
		je HAIR_RET
		cmp eax, 0x4
		je HAIR_RET
		cmp eax, 0x6
		je HAIR_RET
		jmp CAP_RET
		FACE_RET :
		jmp dword ptr[dwHairFaceUncapFaceRetn]
			HAIR_RET :
			jmp dword ptr[dwHairFaceUncapHairRetn]
			CAP_RET :
			jmp dword ptr[dwHairFaceUncapCapRetn]
	}
}

__declspec(naked) void HairFaceIdUncap2()
{
	__asm {
		cmp eax, 0x2
		je FACE_RET
		cmp eax, 0x5
		je FACE_RET
		cmp eax, 0x3
		je HAIR_RET
		cmp eax, 0x4
		je HAIR_RET
		cmp eax, 0x6
		je HAIR_RET
		jmp SKIN_RET
		FACE_RET :
		mov eax, 0x0
			mov ecx, 0x0
			jmp JMP_RET
			HAIR_RET :
		mov eax, 0x1
			mov ecx, 0x1
			jmp JMP_RET
			SKIN_RET :
		mov eax, 0x2
			mov ecx, 0x2
			jmp JMP_RET
			JMP_RET :
		jmp dword ptr[dwHairFaceUncapRetn]
	}
}

// Expand Cash Effect Id's
DWORD dwCashEffExpansion1 = 0x0093C144;
DWORD dwCashEffExpansion1CheckRtm = 0x0093C156;
DWORD dwCashEffExpansion1Rtm = 0x0093C163;
__declspec(naked) void cash_effect_expansion_1()
{
	__asm {
		mov eax, [ebp + 8] // item ID
		cdq
		mov ecx, 0x2710 // 10000
		idiv ecx
		cmp eax, 0x1F5 // 501
		je enable_loop

		mov eax, [ebp + 8] // item ID
		cdq
		mov ecx, 0x3E8 // 1000
		idiv ecx
		jmp dword ptr[dwCashEffExpansion1CheckRtm] // back to check for 4290xxx

		enable_loop:
		jmp dword ptr[dwCashEffExpansion1Rtm] // set ani type to 32
	}
}

DWORD dwCashEffExpansion2 = 0x0093C67A;
DWORD dwCashEffExpansion2CheckRtm = 0x0093C690;
DWORD dwCashEffExpansion2Rtm = 0x0093C69A;
__declspec(naked) void cash_effect_expansion_2()
{
	__asm {
		mov eax, [ebp + 8] // item ID
		cdq
		mov ecx, 0x2710 // 10000
		idiv ecx
		cmp eax, 0x1F5 // 501
		je enable_loop

		mov eax, [ebp + 8] // item ID
		cdq
		mov ecx, 0x3E8 // 1000
		idiv ecx
		jmp dword ptr[dwCashEffExpansion2CheckRtm] // back to check for 4290xxx

		enable_loop:
		jmp dword ptr[dwCashEffExpansion2Rtm] // set ani type to 32
	}
}

*/

#pragma endregion

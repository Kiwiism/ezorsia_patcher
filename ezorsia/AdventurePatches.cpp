#include "stdafx.h"
#include "AddyLocations.h"
#include "AdventureCodecaves.h"
#include "AdventurePatches.h"
#include "CritSignalPacket.h"
#include "PatchToggles.h"
#include "ShadowPartnerAttacks.h"

static_assert(PatchSettings::KnockbackDistance <= 0xFFFF, "KnockbackDistance must fit in 16 bits.");
static_assert(PatchSettings::RushCooldownMs <= 0xFFFF, "RushCooldownMs must fit in 16 bits.");
static_assert(PatchSettings::DamageCap >= 0.0 && PatchSettings::DamageCap <= 2147483647.0,
	"DamageCap must fit in a signed 32-bit integer.");
static_assert(PatchSettings::AranComboDurationMs >= 0, "AranComboDurationMs cannot be negative.");
static_assert(PatchSettings::ChainLightningHorizontalRange >= 0,
	"ChainLightningHorizontalRange cannot be negative.");
static_assert(PatchSettings::ChainLightningVerticalRange >= 0,
	"ChainLightningVerticalRange cannot be negative.");
static_assert(PatchSettings::PasswordLength <= 0xFF, "PasswordLength must fit in 8 bits.");
static_assert(PatchSettings::MakerDropdownSize <= 0xFF, "MakerDropdownSize must fit in 8 bits.");
static_assert(PatchSettings::MakerPurpleStatThreshold <= 0xFF, "MakerPurpleStatThreshold must fit in 8 bits.");
static_assert(PatchSettings::MakerYellowStatThreshold <= 0xFF, "MakerYellowStatThreshold must fit in 8 bits.");

void AdventurePatches::Apply()
{
	// Gameplay: custom skills

#if PATCH_SHADOW_PARTNER_MELEE_MAGIC
	ShadowPartnerAttacks::Apply();
#endif

#if PATCH_CRIT_SIGNAL_PACKET
	CritSignalPacket::Apply();
#endif

#if PATCH_CUSTOM_ACTIVE_SKILLS
	// Enable Custom Active Skills - Gwen
	Memory::SetHook(true, reinterpret_cast<void**>(&pDoActiveSkill), CUserLocal__DoActiveSkill_t);

#endif

#if PATCH_LTRB_NON_BULLET_SKILLS
	// LTRB Handling - Gwen
	_is_attack_area_set_by_data = reinterpret_cast<is_attack_area_set_by_data_t>(0x007666CB);
	Memory::SetHook(TRUE, reinterpret_cast<PVOID*>(&_is_attack_area_set_by_data), is_attack_area_set_by_data);

#endif

#if PATCH_LTRB_CUSTOM_SHOOT_SKILLS
	// LTRB Handling - Custom Shoot Skills - Pepa / Teto
	_is_rect_attack_shoot_skill = reinterpret_cast<is_rect_attack_shoot_skill_t>(0x00766722);
	Memory::SetHook(TRUE, reinterpret_cast<PVOID*>(&_is_rect_attack_shoot_skill), is_rect_attack_shoot_skill);
#endif

	// Gameplay: existing skills
#if PATCH_ARAN_COMBO_UPTIME
	// Set Aran combo duration - Crabo
	Memory::WriteInt(0x0094BDC8 + 1, PatchSettings::AranComboDurationMs);
	Memory::WriteInt(0x00960708 + 1, PatchSettings::AranComboDurationMs);
	Memory::WriteInt(0x0096095B + 1, PatchSettings::AranComboDurationMs);

#endif

#if PATCH_ARAN_BUFF_MODIFIERS
	// Aran Buff Modifiers - PDD / MDD - Pepa
	Memory::WriteByte(0x007669D6 + 2, 0x09);

#endif

#if PATCH_POWER_GUARD_CHANGES
	// PowerGuard Changes - Pepa
	Memory::PatchNop(0x00958803, 2);
	Memory::PatchNop(0x00958805, 3);
	Memory::PatchNop(0x00958816, 11); // Remove Boss Halving
	Memory::PatchNop(0x009593AE, 3); // Remove Healing Effect

#endif

#if PATCH_INSTANT_FINAL_ATTACK
	// Instant FA - General
	Memory::WriteByte(0x0095795E, 0x83);
	Memory::WriteByte(0x0095795E + 1, 0xC0);
	Memory::WriteByte(0x0095795E + 2, 0x00);

#endif

#if PATCH_KNOCKBACK_DISTANCE
	// Change knockback distance for all skills (300 default) - Pepa / Teto
	Memory::WriteShort(0x0066BA5E + 1, static_cast<unsigned short>(PatchSettings::KnockbackDistance));

#endif

#if PATCH_RUSH_COOLDOWN
	// Rush cooldown in milliseconds - Crabo
	Memory::WriteShort(0x00952EF9 + 2, static_cast<unsigned short>(PatchSettings::RushCooldownMs));

#endif

#if PATCH_RUSH_IN_AIR
	// Cast rush in air - Crabo
	Memory::PatchNop(0x00950B58, 6);
	Memory::PatchNop(0x00950B63, 6);
	Memory::PatchNop(0x00950B6E, 6);
#endif

#if PATCH_STUN_MASTERY_CRIT_PUNCH
	// Stun Mastery Crit Punch - Xervous
	Memory::PatchNop(0x0078E1DC, 2);

#endif

#if PATCH_ASSAULTER_WITHOUT_TARGET
	// Assaulter no target - Crabo
	Memory::WriteByte(0x00951347 + 1, 0x1C);

#endif

#if PATCH_TRY_SHOOT_ATTACK_LTRB
	// TryShootAttack CodeCaving for Ranged Skills to be LTRB Handled
	Memory::SetHook(true, reinterpret_cast<void**>(&ltrbshoothook), ltrb);
	Memory::CodeCave(LtRb_Eval, 0x00953E2C, 6); // CodeCave to move skills into tryShootAttack

#endif

#if PATCH_MULTI_MOB_MAGIC_ARROWS
	// Multimob for Fire Arrow / Angel Ray / Holy Arrow - Crabo
	Memory::CodeCave(FireArrow, dwFireArrow, 5);
	Memory::CodeCave(FireArrowBullet, dwFireBulletAdd, 5);
	Memory::FillBytes(0x0095644A, 0x90, 2);

#endif

#if PATCH_BATTLESHIP_ANIMATION_SPEED
	// Battleship Mount Animation Speed - Rulax
	Memory::WriteValue(0x00936B2D + 6, PatchSettings::BattleshipAnimationSpeed);

#endif

#if PATCH_BATTLESHIP_ROPE_CLIMB
	// Battleship Rope Climb - Rulax
	Memory::WriteByte(0x009CC11F, 0xEB);

#endif

#if PATCH_CHAIN_LIGHTNING_RANGE
	// Chain Lightning Bounce Range - Slimy
	Memory::WriteInt(0x00955EF0 + 1, PatchSettings::ChainLightningHorizontalRange);
	Memory::WriteInt(0x006788EE + 2, PatchSettings::ChainLightningVerticalRange);
	Memory::WriteInt(0x006788F6 + 2, -PatchSettings::ChainLightningVerticalRange);

#endif

#if PATCH_CHAIN_LIGHTNING_NO_DAMAGE_FALLOFF
	// Chain Lightning / Piercing Arrow No Damage Falloff
	Memory::WriteByte(0x0075C004, 0xEB);
	Memory::WriteByte(0x0075C004 + 1, 0x00);

#endif

#if PATCH_POTIONS_IN_DARKSIGHT
	// Allow Pots While in Darksight - Angel
	Memory::FillBytes(0x0094F6AB, 0x90, 6);

#endif

#if PATCH_BOOMERANG_STEP_IN_AIR
	// Boomerang Step in air - Root Leak
	Memory::WriteValue(0x00950B4D + 2, 0x00950C53 - (0x00950B4D + 6));

#endif

#if PATCH_BOOMERANG_STEP_TERRAIN
	// Boomerang Step on slopes, edges, and small platforms
	Memory::WriteByte(0x00950D96, 0x2A);

#endif

#if PATCH_JUMP_ATTACK_MAGE_SHOOT
	// Jump Attack Mage / Shoot - Teto
	Memory::WriteByte(0x009539FA, 0xEB);
	Memory::WriteByte(0x009559E5, 0xEB);

#endif

#if PATCH_RECOIL_SHOT_COOLDOWN
	// Recoil Shot CD - Root Leak
	Memory::CodeCave(RecoilShotLowerCD, dwRecoilShot, 5);
	Memory::FillBytes(0x0095364B, 0x90, 2);

#endif

#if PATCH_MID_AIR_TELEPORT
	// Mid Air Teleport - Root Leak
	Memory::PatchNop(0x00957C2D, 6);

#endif

#if PATCH_NO_CHARGE_BIG_BANG
	// Remove Charge: Big Bang - Root Leak
	Memory::CodeCave(BigBang, dwBB, 5);
	Memory::CodeCave(BigBang2, dwBB2, 5);//I/L
	Memory::CodeCave(BigBang3, dwBB3, 5);//F/P

#endif

#if PATCH_NO_CHARGE_PIERCING_ARROW
	// Remove Charge: Piercing Arrow - Root Leak
	Memory::CodeCave(PA, dwPA, 5);

#endif

#if PATCH_NO_CHARGE_CORKSCREW_BLOW
	// Remove Charge: Corkscrew Blow - Root Leak
	Memory::CodeCave(CorkscrewBlow, dwCB, 5);

#endif

#if PATCH_MOVE_AND_ATTACK
	// Move & Attack Simultaneously - Root Leak
	Memory::WriteByte(0x0095F97A, 0xEB);
	Memory::WriteByte(0x0095F97A + 1, 0x59);
	Memory::WriteByte(0x009CBFB0, 0xEB);
	Memory::PatchNop(0x0094C3BB, 6);
#endif

	// Gameplay: Maker
#if PATCH_MAKER_ITEM_COLORS
	// Update ItemCalcValues - Pepa
	Memory::WriteByte(0x005CA1B8 + 2, static_cast<unsigned char>(PatchSettings::MakerPurpleStatThreshold));
	Memory::WriteByte(0x005CA5FA + 2, static_cast<unsigned char>(PatchSettings::MakerYellowStatThreshold));

#endif

#if PATCH_MAKER_CATEGORIES
	// Update Maker Categories - Gwen
	Memory::CodeCave((void*)makerStuff, 0x008230FA, 0);
	Memory::CodeCave((void*)Replace, 0x005C9F7A, 0);

#endif

#if PATCH_MAKER_HIDE_MONSTER_CRYSTAL_CATEGORY
	// Skip Drawing Dropdown Categories - Pepa
	Memory::PatchNop(0x0082354F, 64); // Monster Etc > Crystal
#if PATCH_MAKER_HIDE_DISASSEMBLE_CATEGORY
	Memory::PatchNop(0x0082350F, 64); // Disassemble Equipment
#endif
#if PATCH_MAKER_HIDE_UNKNOWN_CATEGORY
	Memory::PatchNop(0x00823818, 21); // ???
#endif

#endif

#if PATCH_MAKER_ANY_STIMULATOR
	// Allow any type of Stimulator for any equip in Maker - Pepa
	Memory::WriteByte(0x0082660B, 0xEB);
	Memory::WriteByte(0x0082660B + 1, 0x4D);

#endif

#if PATCH_MAKER_MESO_POSITION
	// Change the Meso Draw Location - Pepa
	Memory::WriteByte(0x008251D3 + 1, 0xE1);

#endif

#if PATCH_MAKER_DROPDOWN_SIZE
	// Expand the Maker dropdown menu - Pepa
	Memory::WriteByte(0x008223BD + 6, static_cast<unsigned char>(PatchSettings::MakerDropdownSize));

#endif

#if PATCH_MAKER_DRAW_FUNCTIONS
	// Draw Functions - Pepa
	Memory::WriteByte(0x008258DC + 2, 0x06); // Handle up to 6 Gem Slots for disabled icons
	Memory::WriteByte(0x008258E8 + 2, 0xE); // Change Starting Draw Gem Slot Location
	Memory::PatchNop(0x00824D1C, 31); // Skip Drawing Catalyst

#endif

#if PATCH_MAKER_SLOT_RECTS
	// SetItemSlotRect - Pepa
	Memory::WriteByte(0x00822A1C + 1, 0xE); // Change Starting Set Gem Location
	Memory::PatchNop(0x00822A67, 27); // Skip setting RecipeSlot
	Memory::PatchNop(0x00822A82, 29); // Skip setting CatalystSlot
	Memory::PatchNop(0x00822A54, 19); // Skip setting CatalystDropSlot
	Memory::PatchNop(0x00823B10, 5); // Skip setting Catalyst
	Memory::PatchNop(0x00823B17, 5); // Skip setting Recipe
#endif

	// Gameplay: general systems
#if PATCH_RECHARGEABLE_ARROWS
	// Rechargeable Arrows - Gwen
	RechargeArrows();

#endif

#if PATCH_DISABLE_AUTO_ASSIGN
	// Disable AutoAssign Button - Pepa
	Memory::CodeCave(autoAssignRemove, 0x008C5920, 5);
#endif

#if PATCH_ONE_HANDED_AXE_BW_MULTIPLIER
	// 1H Axe/BW multiplier - Crabo
	constexpr DWORD oneHandedMultiplierAddress = 0x00AFE868;
	Memory::WriteDouble(oneHandedMultiplierAddress, PatchSettings::OneHandedAxeBwMultiplier);
	Memory::WriteInt(0x0078F6B0 + 2, oneHandedMultiplierAddress); // CalcDamage
	Memory::WriteInt(0x008C2DFD + 2, oneHandedMultiplierAddress); // Character sheet

#endif

#if PATCH_TWO_HANDED_AXE_BW_MULTIPLIER
	// 2H Axe/BW multiplier - Crabo
	constexpr DWORD twoHandedMultiplierAddress = 0x00AFE848;
	Memory::WriteDouble(twoHandedMultiplierAddress, PatchSettings::TwoHandedAxeBwMultiplier);
	Memory::WriteInt(0x008C2C56 + 2, twoHandedMultiplierAddress); // Character sheet
	Memory::WriteInt(0x0078F24A + 2, twoHandedMultiplierAddress); // CalcDamage

#endif

#if PATCH_UNCAPPED_PLAYER_STATS
	// Uncapped Player Stats - Root Leak
	Memory::WriteDouble(0x00AFE8A0, PatchSettings::DamageCap);
	const int damageCap = static_cast<int>(PatchSettings::DamageCap);
	Memory::WriteValue(0x008C3304 + 1, damageCap);
#if !PATCH_UNCAPPED_MOB_STATS
	Memory::WriteByteArray(0x00780620 + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1));
#endif
	Memory::WriteByteArray(0x0077E055 + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1));
	Memory::WriteByteArray(0x0077E12F + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1));
#if !PATCH_UNCAPPED_MOB_STATS
	Memory::WriteByteArray(0x0077E215 + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1));
#endif
	Memory::WriteByteArray(0x0078FF5F + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1));
	Memory::WriteByteArray(0x0079166C + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1));
	Memory::WriteByteArray(0x00791CD5 + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1));
	Memory::WriteByteArray(0x007806D0 + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1)); // Accuracy
	Memory::WriteByteArray(0x00780702 + 1, Uncap_Stat_Arr_1, sizeof(Uncap_Stat_Arr_1)); // Avoidability

#endif

#if PATCH_UNCAPPED_MOB_STATS
	// Uncapped Mob Stats - Root Leak
	Memory::WriteValue(0x0067DD1D + 1, PatchSettings::MobStatCap);
	Memory::WriteValue(0x00793499 + 1, PatchSettings::MobStatCap);
	Memory::WriteValue(0x00793107 + 1, PatchSettings::MobStatCap);
	Memory::WriteValue(0x007926DD + 1, PatchSettings::MobStatCap);
	Memory::WriteValue(0x0077E215 + 1, PatchSettings::MobStatCap);
	Memory::WriteValue(0x00780620 + 1, PatchSettings::MobStatCap);
#if PATCH_FORCE_MOB_ATTACKS_TO_HIT
	Memory::WriteByte(0x007930C5, 0xEB); // Check for Physical DMG Miss (these essentially force the player to ALWAYS be hit)
	Memory::WriteByte(0x00793484, 0xEB); // Check for Magic DMG Miss
#endif

#endif

#if PATCH_MOVEMENT_SPEED_CAP
	// Configurable movement speed cap - General
	Memory::WriteValue(0x00780746, PatchSettings::MovementSpeedCap);
	Memory::WriteValue(0x008C4287, PatchSettings::MovementSpeedCap);
	Memory::WriteValue(0x0094D91F, PatchSettings::MovementSpeedCap);

#endif

#if PATCH_MOUNT_SPEED_CAP
	// Mount Speed Cap Removal - Teto (untested)
	Memory::WriteValue(0x0094D7EE + 1, PatchSettings::MountSpeedCap); // CUserLocal::SetShoeAttr
	Memory::WriteValue(0x008C41E1 + 1, PatchSettings::MountSpeedCap); // CUIStatDetail::Draw
	Memory::WriteValue(0x007AFDE7 + 1, PatchSettings::MountSpeedCap); // CTamingMobTemplate::RegisterTamingMob

#endif

#if PATCH_REMOVE_SP_JOB_CHECKS
	// Remove SP Checks Between Jobs - General
	Memory::WriteByte(0x008AD01A, 0xE9);
	Memory::WriteValue(0x008AD01A + 1, 0x008AD227 - (0x008AD01A + 5));

#endif

#if PATCH_LADDER_ROPE_SPEED
	// Ladder / Rope Climb Speed - General
	Memory::WriteValue(0x009CC6F9 + 2, 0x00C1CF80); // Switch Address
	Memory::WriteDouble(0x00C1CF80, PatchSettings::LadderRopeSpeed);

#endif

#if PATCH_REMOVE_RANGED_WHACK
	// Remove Whack from Ranged Weapons - General
	Memory::WriteByte(0x009516C2, 0xE9);
	Memory::WriteByte(0x009516C2 + 1, 0xc8);
	Memory::WriteByte(0x009516C2 + 2, 0xfc);
	Memory::WriteByte(0x009516C2 + 3, 0xff);
	Memory::WriteByte(0x009516C2 + 4, 0xff);

#endif

#if PATCH_SUPER_TUBI
	// Super Tubi
	Memory::FillBytes(0x00485C01, 0x90, 2); // NOP 2
	Memory::FillBytes(0x00485C21, 0x90, 2); // NOP 2
	Memory::FillBytes(0x00485C32, 0x90, 2); // NOP 2
#endif

	// Quality of life
#if PATCH_PASSWORD_LENGTH
	// Allow password length beyond 12 - Mr.Dead
	Memory::WriteByte(0x00620F43 + 2, static_cast<unsigned char>(PatchSettings::PasswordLength));
	Memory::WriteInt(0x00620A21 + 3, PatchSettings::PasswordLength);

#endif

#if PATCH_CASH_ITEMS_IN_STORAGE
	// Allow Cash Items in Storage - Pepa
	Memory::PatchNop(0x007C6EF7, 6);
	Memory::PatchNop(0x007C6F03, 6);

#endif

#if PATCH_CASH_ITEMS_DROPPABLE
	// Allow Cash Items to be dropped - Pepa
	Memory::PatchNop(0x004F350C, 6);
	Memory::PatchNop(0x004F351E, 6);
#endif

#if PATCH_CASH_ITEMS_TRADEABLE
	// Allow Cash Items to be Traded - Pepa
	Memory::PatchNop(0x004F3FB8, 6);
	Memory::PatchNop(0x004F3FC4, 6);

#endif

#if PATCH_CASH_ITEMS_SELLABLE
	// Allow Cash Shop Items to be Sold - Pepa
	Memory::WriteByte(0x004F427A, 0xEB);
	Memory::WriteByte(0x004F427A + 1, 0x17);
	Memory::WriteByte(0x006FDA67, 0xEB);
	Memory::WriteByte(0x006FDA67 + 1, 0x12);
	Memory::WriteByte(0x006FDA72, 0xEB);

#endif

#if PATCH_CASH_SHOP_BIRTHDAY_BYPASS
	// Remove call to check if there are cash items in the list, which prompts the birthday check (Put Cash Items in Shop) - Pepa
	Memory::WriteByte(0x006FCC33, 0xB8);
	Memory::WriteByte(0x006FCC33 + 1, 0x01);
	Memory::WriteByte(0x006FCC33 + 2, 0x00);
	Memory::WriteByte(0x006FCC33 + 3, 0x00);
	Memory::WriteByte(0x006FCC33 + 4, 0x00);

#endif

#if PATCH_CASH_ITEM_TIME_FORMAT
	// Remove Parens Formatting from Time Remaining Window (Put Cash Items in Shop) - Pepa
	Memory::WriteByte(0x00AF2450 + 2, 0x20);
	Memory::WriteByte(0x00AF2450 + 5, 0x00);

#endif

#if PATCH_CASH_ITEM_SALE_DATE_BYPASS
	// Remove Date / Time Check for Cash Item Sales - Pepa
	Memory::WriteByte(0x006FD63D, 0xEB);

#endif

#if PATCH_CASH_ITEM_NORMAL_SALE_MESSAGES
	// Skip Sending Messages for Cash Items, Pass a Normal Sale Experience - Pepa
	Memory::WriteByte(0x006FD31F, 0x90);
	Memory::WriteByte(0x006FD31F + 1, 0xE9);
#endif

#if PATCH_TYPEABLE_PIC
	// Allow PIC to be Typed - General
	Memory::FillBytes(0x0076F7DE, 0x90, 6);
#endif

#if PATCH_CENTER_WORLD_MAP
	// World Map Centered - leevccc
	wordMapX = (Client::m_nGameWidth - 666) / 2;
	wordMapY = (Client::m_nGameHeight - 524) / 2;
	Memory::CodeCave(wordMapUIcc, 0x009EB594, 13);

#endif

#if PATCH_REMOVE_SKILL_NOT_READY_MESSAGE
	// Remove: "You may not use this skill yet" message - General
	Memory::PatchNop(0x00967707, 12);

#endif

#if PATCH_REMOVE_CARD_FULL_MESSAGE
	// Remove: "This card is already full..." message - General
	Memory::PatchNop(0x00A08283, 18);

#endif

#if PATCH_REMOVE_CATCH_BREATH_MESSAGE
	// Remove: "You Must Catch Your Breath..." - General
	Memory::WriteByte(0x00452316, 0x7C);

#endif

#if PATCH_REMOVE_GENDER_CHECKS
	// Remove: Gender Checks on Gear - General
	Memory::PatchNop(0x00460AED, 2);

#endif

#if PATCH_REMOVE_CHAT_SPAM_MESSAGES
	// Remove: "Too much chatting..." - General
	Memory::WriteByte(0x00490651, 0xEB);
	Memory::WriteByte(0x00490652, 0x1D);

	// Remove: "Repeating the same line..." - General
	Memory::WriteByte(0x00490607, 0xEB);
	Memory::WriteByte(0x00490609, 0x27);

	// Remove: Block of saying the same line 3 times - General
	Memory::WriteByte(0x004905EB, 0xEB);
	Memory::WriteByte(0x004CAA09, 0xEB);
	Memory::WriteByte(0x004CAA84, 0xEB);

#endif

#if PATCH_REMOVE_FAMILY_SYSTEM
	// Remove Family System from Server - Shahar
	unsigned char skip_family_draw[] = { 0xE9, 0x32, 0x01, 0x00, 0x00 };
	Memory::WriteByteArray(0x00A061A3, skip_family_draw, 5);
	Memory::WriteByte(0x008FFF70, 0xEB); // remove blue family button
	Memory::WriteByte(0x009970ED, 0xEB); // remove blue 'add junior' panel on right click
	Memory::WriteByte(0x00996BE5 + 3, 0x0D); // smaller bar
	Memory::WriteByte(0x00996D12 + 2, 0x08); // smaller bar floor

#endif

#if PATCH_REMOVE_MAKER_CRAFT_TIME
	// Remove: Craft time from Maker - General
	Memory::WriteByte(0x826F92 + 2, 0x08);
	Memory::WriteByte(0x826F92 + 3, 0x01);
	Memory::WriteByte(0x826F92 + 4, 0x00);
	Memory::WriteByte(0x826F92 + 5, 0x00);

#endif

#if PATCH_PET_BEHIND_PLAYER
	// Move Pet Behind Player - Unknown
	Memory::WriteByte(0x0070451B + 2, 1);
#endif

	// Quality of life: expanded UIs
#if PATCH_EXPANDED_STORAGE
	// Expanded Storage - Goose
	Memory::WriteByte(0x007C7C27 + 3, 0xC9); // Draw Icons in Trunk Inv
	Memory::WriteByte(0x007C82C3 + 3, 0xBF); // Draw Tooltips in Trunk Inv
	Memory::WriteByte(0x007C8035 + 3, 0xC9); // Draw Icons in Player inv
	Memory::WriteByte(0x007C8385 + 3, 0xBF); // Draw Tooltips in Player Inv
	Memory::WriteByte(0x007C65B6 + 1, 0xC6); // Storage Meso Button Y
	Memory::WriteByte(0x007C6631 + 1, 0xC6); // Player Meso Button
	Memory::WriteByte(0x007C8197 + 1, 0xC8); // Player and Merchant Mesos Y offset

#endif

#if PATCH_EXPANDED_MERCHANT_SCROLLBAR
	// Merchant Scrollbar Length - Goose
	Memory::WriteByte(0x007C69DC + 1, 0x64);
	Memory::WriteByte(0x007C69DC + 2, 0x01);
	Memory::WriteByte(0x007C70B3 + 2, 0xF9); // Scrollbar Fix, ty Angel

#endif

#if PATCH_EXPANDED_PLAYER_SCROLLBAR
	// Player Scrollbar Length - Goose
	Memory::WriteByte(0x007C6A3A + 1, 0x40);
	Memory::WriteByte(0x007C6A3A + 2, 0x01);
	Memory::WriteByte(0x007C7081 + 2, 0xF8); // Scrollbar Fix, ty Angel

#endif

#if PATCH_EXPANDED_VENDOR
	// Expanded Vendor - Goose
	Memory::WriteByte(0x00753DB8 + 1, 0x64);
	Memory::WriteByte(0x00753DB8 + 2, 0x01);
	Memory::WriteByte(0x00754719 + 2, 0xF8);
	Memory::WriteByte(0x00753E19 + 1, 0x64);
	Memory::WriteByte(0x00753E19 + 2, 0x01);
	Memory::WriteByte(0x0075474B + 2, 0xF8);
	Memory::WriteByte(0x00755748 + 3, 0xDE);
	Memory::WriteByte(0x00755E44 + 2, 0x09);
	Memory::WriteByte(0x007560D5 + 3, 0xDE);
	Memory::WriteByte(0x00756197 + 3, 0xDE);
	Memory::WriteByte(0x007540A3 + 1, 0xD8);
	Memory::WriteByte(0x007557D6, 0xB8);
	Memory::WriteInt(0x007557D6 + 1, (DWORD)&shop_btn_Array);
	Memory::WriteByte(0x007557D6 + 5, 0x90);
	Memory::WriteByte(0x00755A2A, 0xB8);
	Memory::WriteInt(0x00755A2A + 1, (DWORD)&shop_btn_Array);
	Memory::WriteByte(0x00755A2A + 5, 0x90);
	Memory::WriteByte(0x00755ACC, 0xB8);
	Memory::WriteInt(0x00755ACC + 1, (DWORD)&shop_btn_Array);
	Memory::WriteByte(0x00755ACC + 5, 0x90);
	Memory::WriteByte(0x00755B0B, 0xB8);
	Memory::WriteInt(0x00755B0B + 1, (DWORD)&shop_btn_Array);
	Memory::WriteByte(0x00755B0B + 5, 0x90);
	Memory::WriteByte(0x00755AB9, 0xB9);
	Memory::WriteInt(0x00755AB9 + 1, (DWORD)&shop_btn_Array);
	Memory::WriteByte(0x00755AB9 + 5, 0x90);

#endif

#if PATCH_EXPANDED_SKILLS
	// Expanded Skills - Goose
	Memory::WriteByte(0x008AA86F + 1, 0x73); // Extend rendering area to fit modified wz
	Memory::WriteByte(0x008AACD5 + 1, 0xF0); // Extend scrollbar
	Memory::WriteByte(0x008AAE23 + 1, 0x59); // Move Macro button down
	Memory::WriteByte(0x008AD9F2 + 2, 0x4F); // Draw extra tooltips
	Memory::WriteByte(0x008ACE76 + 3, 0x66); // Draw extra icons
	Memory::WriteByte(0x008AD7B4 + 2, 0xFB); // Scrollbar fix
	Memory::WriteByte(0x008AC4DF + 1, 0x5B); // Skill-points Y offset
	Memory::WriteByte(0x008AADAC + 3, 0x67); // Expand level-up buttons
	Memory::WriteByte(0x008AB929 + 2, 0xE0); // Make new buttons clickable
	Memory::WriteByte(0x008AD903 + 2, 0x06); // Read six buttons in SetButton
	Memory::WriteByte(0x008AD7F8 + 2, 0x06); // Read six buttons in SetButton

	// CUISkill::OnCreate - Goose
	Memory::WriteByte(0x008AAD3C + 1, 5);
	Memory::WriteInt(0x008AAD3C + 2, (DWORD)&skillup_btn_Array + 12);

	// CUISkill::SetButton - Goose
	Memory::WriteByte(0x008AD920 + 1, 0x34);
	Memory::WriteByte(0x008AD920 + 2, 0xC5);
	Memory::WriteInt(0x008AD920 + 3, (DWORD)&skillup_btn_Array + 12);
#endif

	// Fixes
#if PATCH_PARTY_BLOCK_BUTTON_TEXT
	// Fix button in parties changing to "block" - YNWA
	Memory::WriteByte(0x0091DCEC + 2, 0x18);
	Memory::WriteByte(0x0091DDE8 + 2, 0x18);

#endif

#if PATCH_ARAN_COMBO_POSITION
	// Move Aran Combo and Skills to correct place - Crabo
	Memory::WriteInt(0x00960581 + 3, Client::m_nGameWidth - 42);
	Memory::WriteInt(0x00960839 + 1, Client::m_nGameWidth - 42);
	Memory::WriteInt(0x00960C67 + 1, Client::m_nGameWidth - 10);
	Memory::WriteInt(0x00960DED + 1, Client::m_nGameWidth - 10);

#endif

#if PATCH_PQ_CLEAR_MESSAGE_POSITION
	// Fix pq clear messaging - Unknown
	Memory::WriteInt(0x0053500F + 1, Client::m_nGameWidth / 2); // CField::ShowScreenEffect

#endif

#if PATCH_CHARACTER_SELECT_STAT_POSITION
	// Move Stat Text at Char Select - Pepa
	Memory::WriteByte(0x00602D80 + 1, 0x38); // Job
	Memory::WriteByte(0x00602E25 + 1, 0x38); // LV.
	Memory::WriteByte(0x00602F7F + 1, 0x38); // STR
	Memory::WriteByte(0x006030D4 + 1, 0x38); // DEX
	Memory::WriteByte(0x00602ED0 + 1, 0x92); // FAME, INT, LUK
	Memory::WriteByte(0x00603437 + 1, 0x9B); // World Ranking Change
	Memory::WriteByte(0x006038B6 + 1, 0x9B); // Job Ranking Change
#if PATCH_CHARACTER_SELECT_JOB_RANK_TEXT
	Memory::WriteByte(0x00603786 + 1, 0x69); // Job Ranking "Ranked at X"
#endif

#endif

#if PATCH_REMOVE_CHARACTER_SELECT_SERVER_NAME
	// Remove Server Name Text at Char Select Screen - Pepa
	Memory::WriteByte(0x0060DB9B, 0x90);
	Memory::WriteByte(0x0060DB9B + 1, 0xE9);

#endif

#if PATCH_MONSTER_MAGNET_STATIC_ENEMIES
	// Monster Magnet Works on Static Enemies - Shavit
	Memory::FillBytes(0x0096C554, 0x90, 4);

#endif

#if PATCH_ALIGN_CHAT_TEXT
	// Align Chat Box Text - Goose
	// Memory::WriteByte(0x008DD05A + 2, 0x4);
	// Memory::WriteByte(0x008DD067 + 2, 0x2);
	Memory::CodeCave(chat_Y_offset_cave, 0x008DD6B9, 5);

#endif

#if PATCH_MOUSE_WHEEL_CURSOR
	// Fix Mouse Cursor Scroll Position - LMT
	Memory::CodeCave(fixMouseWheelHook, fixMouseWheelAddr, 15);

#endif

#if PATCH_MONSTER_BOOK_TEXT
	// Monster Book Aligned Text - leevccc
	Memory::WriteByte(0x0086425B + 1, 82); // 5
	Memory::CodeCave(pos1, 0x00864378, 5); // 56
	Memory::CodeCave(pos2, 0x00864495, 5); // 89
	Memory::CodeCave(pos3, 0x008645B4, 5); // 108
	Memory::WriteInt(0x008646B8 + 1, 221); // 146

#endif

#if PATCH_GUILD_EMBLEM_BOTTOM_LINE
	// Fix Bottom Line of Guild Emblem Missing - MURIDA
	Memory::WriteByte(0x005F12EF + 2, 0x05);
#endif

	// Experimental and unused patches
#if PATCH_DISABLE_ALL_AP_BUTTONS
	// Disable All AP Up Buttons - Pepa
	Memory::PatchNop(0x008C4CB6, 5);
#endif

#if PATCH_TRINKET_SLOT
	// Add Trinket Slot (Shoulder) - FallRob1n
	Memory::WriteByte(0x005C973A + 2, 0x73);
	Memory::WriteByte(0x004F1CA9 + 2, 0x34);
	Memory::WriteByte(0x007FEFBC + 2, 0x33);
	Memory::WriteByte(0x007FEE93 + 3, 0x34);
	Memory::WriteByte(0x0077EDEF + 2, 0x34);
	Memory::WriteByte(0x0077E48D + 3, 0x34);
	Memory::WriteByte(0x0077EE07 + 2, 0x34);
	Memory::PatchNop(0x007FEF39, 0x007FEF58 - 0x007FEF39);
	Memory::CodeCave(putEquipPosition_Hook, 0x007FEF39, 5);
#endif

#if PATCH_DAMAGE_AND_ATTACK_SPEED_CAP
	// Damage and AttackSpeed Cap - Pepa
	Memory::WriteDouble(0x00AF8298, 5); // Double for DMG Calculations
	Memory::CodeCave(setAttSpdCap, 0x00765080, 1); // AttackSpeed0Cap // Needs Work

#endif

#if PATCH_REMOVE_QUICKSLOT_BUTTON
	// Remove Quickslot Button - Unknown
	Memory::PatchNop(0x008D3598, 3);
	Memory::WriteInt(0x008D3274, 0x288);
	Memory::WriteInt(0x008D32FB, 0x2A6);
	Memory::WriteInt(0x008D3382, 0x2C4);
	Memory::WriteInt(0x008D3409, 0x2E2);
	Memory::WriteInt(0x008D3490, 0x300);

#endif

#if PATCH_WORLD_MAP_CAP_INCREASE
	// WorldMap Cap Increase - Unknown
	Memory::WriteByte(0x009EA030, 0x81);
	Memory::WriteByte(0x009EA031, 0xFE);
	Memory::WriteByte(0x009EA032, 0xB4);

#endif

#if PATCH_HAIR_ID_FIX
	// Hair ID Fix - Davi
	Memory::WriteByte(0x005C94FC + 2, 7);
	Memory::WriteByte(0x005C94FF + 1, 0x8E);

#endif

#if PATCH_HAIR_FACE_UNCAP
	// Hair / Face Uncap - Davi
	Memory::CodeCave(faceHairCave, 0x005C94F3, 18); // High Version Hair & Face 1
	Memory::CodeCave(faceHairCave2, 0x009ACA9B, 5); // High Version Hair & Face 2
	Memory::CodeCave(faceHairCave3, 0x009ACAA6, 6); // High Version Hair & Face 3
	Memory::CodeCave(HairFaceIdUncap1, dwHairFaceUncap1, 18);
	Memory::CodeCave(HairFaceIdUncap2, dwHairFaceUncap2, 18);

#endif

#if PATCH_CASH_CAPE_EFFECT_IDS
	// Extend Cape (Cash) Effect ID's - Davi
	Memory::WriteValue(0x0093C144  + 1, 0x2710); // mov ecx, 10000
	Memory::WriteValue(0x0093C14F  + 1, 0x1F5); // cmp eax, 501
	Memory::WriteValue(0x0093C67E  + 1, 0x2710); // mov ecx, 10000
	Memory::WriteValue(0x0093C689  + 1, 0x1F5); // cmp eax, 501
	Memory::WriteValue(0x0095B112 + 1, 0x2710); // mov ecx, 10000
	Memory::WriteValue(0x0095B11F + 1, 0x1F5); // cmp eax, 501
	Memory::CodeCave(cash_effect_expansion_1, dwCashEffExpansion1, dwCashEffExpansion1NOPs);
	Memory::CodeCave(cash_effect_expansion_2, dwCashEffExpansion2, dwCashEffExpansion2NOPs);
#endif
}

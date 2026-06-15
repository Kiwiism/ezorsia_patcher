#pragma once

// This is the single configuration file for internal client behavior.
// Boolean patches use 1 (enabled) or 0 (disabled).
// Numeric values are kept separately so enabled patches can be tuned here.

namespace PatchSettings
{
	inline constexpr char ServerIp[] = "127.0.0.1";

	// Core client behavior
	inline constexpr int GainMessageCount = 26;
	inline constexpr bool UseCustomLoginFrame = false;
	inline constexpr bool UseCustomCashShopFrame = false;
	inline constexpr bool UseV62ExpTable = false;

	// Gameplay values
	inline constexpr int MovementSpeedCap = 160;
	inline constexpr double DamageCap = 999999.0;
	inline constexpr double LadderRopeSpeed = 4.0;
	inline constexpr int AranComboDurationMs = 7000;
	inline constexpr unsigned int KnockbackDistance = 700;
	inline constexpr unsigned int RushCooldownMs = 100;
	inline constexpr unsigned int RecoilShotCooldownMs = 500; // AdventureMS recommended; vanilla is 2000.
	inline constexpr unsigned int BattleshipAnimationSpeed = 28; // AdventureMS recommended.
	inline constexpr int ChainLightningHorizontalRange = 5000;
	inline constexpr int ChainLightningVerticalRange = 500;
	inline constexpr unsigned int PasswordLength = 20; // AdventureMS recommended.
	inline constexpr unsigned int MountSpeedCap = 200; // AdventureMS recommends 600; patch is untested.
	inline constexpr unsigned int MobStatCap = 999999; // AdventureMS recommended.
	inline constexpr unsigned int MakerDropdownSize = 20; // AdventureMS recommended.
	inline constexpr unsigned int MakerPurpleStatThreshold = 23; // Vanilla is 23.
	inline constexpr unsigned int MakerYellowStatThreshold = 40; // Vanilla is 40.

	inline constexpr double OneHandedAxeBwMultiplier = 3.2; // Vanilla is 3.2; AdventureMS recommends 3.6.
	inline constexpr double TwoHandedAxeBwMultiplier = 3.4; // Vanilla is 3.4; AdventureMS recommends 4.4.
}

// Gameplay: skills
#define PATCH_CUSTOM_ACTIVE_SKILLS                   0    // Enables the cleanv83 custom active-skill hook.
#define PATCH_LTRB_NON_BULLET_SKILLS                 0    // Enables LTRB areas for selected non-bullet skills.
#define PATCH_LTRB_CUSTOM_SHOOT_SKILLS               0    // Enables LTRB areas for selected custom shoot skills.
#define PATCH_ARAN_COMBO_UPTIME                      1    // Uses PatchSettings::AranComboDurationMs for combo duration.
#define PATCH_ARAN_BUFF_MODIFIERS                    0    // Applies the cleanv83 Aran PDD/MDD buff modifier.
#define PATCH_POWER_GUARD_CHANGES                    0    // Removes boss halving and Power Guard healing.
#define PATCH_INSTANT_FINAL_ATTACK                   1    // Removes the Final Attack activation delay.
#define PATCH_KNOCKBACK_DISTANCE                     0    // Uses PatchSettings::KnockbackDistance.
#define PATCH_RUSH_COOLDOWN                          0    // Uses PatchSettings::RushCooldownMs.
#define PATCH_RUSH_IN_AIR                            0    // Allows Rush to be cast while airborne.
#define PATCH_STUN_MASTERY_CRIT_PUNCH                0    // Enables critical punch behavior for Stun Mastery.
#define PATCH_ASSAULTER_WITHOUT_TARGET               0    // Allows Assaulter without a selected target.
#define PATCH_TRY_SHOOT_ATTACK_LTRB                  0    // Incomplete cleanv83 ranged LTRB experiment.
#define PATCH_MULTI_MOB_MAGIC_ARROWS                 0    // Adds multi-mob handling to selected arrow/ray skills.
#define PATCH_BATTLESHIP_ANIMATION_SPEED             1    // Uses PatchSettings::BattleshipAnimationSpeed.
#define PATCH_BATTLESHIP_ROPE_CLIMB                  1    // Allows climbing ropes while mounted on Battleship.
#define PATCH_CHAIN_LIGHTNING_RANGE                  0    // Uses the configurable horizontal and vertical bounce ranges.
#define PATCH_CHAIN_LIGHTNING_NO_DAMAGE_FALLOFF      0    // Prevents Chain Lightning and Piercing Arrow damage falloff.
#define PATCH_POTIONS_IN_DARKSIGHT                   1    // Allows potion use while in Dark Sight.
#define PATCH_BOOMERANG_STEP_IN_AIR                  1    // Allows Boomerang Step to be cast while airborne.
#define PATCH_BOOMERANG_STEP_TERRAIN                 1    // Allows Boomerang Step on slopes, edges, and small platforms.
#define PATCH_JUMP_ATTACK_MAGE_SHOOT                 1    // Allows mage and shoot attacks while jumping.
#define PATCH_RECOIL_SHOT_COOLDOWN                   1    // Uses PatchSettings::RecoilShotCooldownMs.
#define PATCH_MID_AIR_TELEPORT                       1    // Allows Teleport while airborne.
#define PATCH_NO_CHARGE_BIG_BANG                     0    // Removes charge time from all Big Bang variants.
#define PATCH_NO_CHARGE_PIERCING_ARROW               0    // Removes charge time from Piercing Arrow.
#define PATCH_NO_CHARGE_CORKSCREW_BLOW               0    // Removes charge time from Corkscrew Blow.
#define PATCH_MOVE_AND_ATTACK                        0    // Allows movement and attacking simultaneously.

// Gameplay: Maker and general systems
#define PATCH_MAKER_ITEM_COLORS                      0    // Uses the Maker purple/yellow stat threshold settings.
#define PATCH_MAKER_CATEGORIES                       0    // Incomplete cleanv83 Maker category experiment.
#define PATCH_MAKER_HIDE_MONSTER_CRYSTAL_CATEGORY    0    // Hides Monster Etc > Crystal from the Maker dropdown.
#define PATCH_MAKER_HIDE_DISASSEMBLE_CATEGORY        0    // Hides Disassemble Equipment from the Maker dropdown.
#define PATCH_MAKER_HIDE_UNKNOWN_CATEGORY            0    // Skips an unidentified Maker dropdown category.
#define PATCH_MAKER_ANY_STIMULATOR                   0    // Allows any stimulator type for any Maker equipment.
#define PATCH_MAKER_MESO_POSITION                    1    // Moves the Maker meso text position.
#define PATCH_MAKER_DROPDOWN_SIZE                    0    // Uses PatchSettings::MakerDropdownSize.
#define PATCH_MAKER_DRAW_FUNCTIONS                   0    // Expands gem drawing and skips catalyst drawing.
#define PATCH_MAKER_SLOT_RECTS                       0    // Changes Maker gem/recipe/catalyst slot rectangles.
#define PATCH_RECHARGEABLE_ARROWS                    0    // Treats arrows as rechargeable shop ammunition.
#define PATCH_DISABLE_AUTO_ASSIGN                    0    // Disables the stat Auto Assign button.
#define PATCH_ONE_HANDED_AXE_BW_MULTIPLIER           0    // Uses PatchSettings::OneHandedAxeBwMultiplier.
#define PATCH_TWO_HANDED_AXE_BW_MULTIPLIER           0    // Uses PatchSettings::TwoHandedAxeBwMultiplier.
#define PATCH_UNCAPPED_PLAYER_STATS                  1    // Raises player stat limits and uses PatchSettings::DamageCap.
#define PATCH_UNCAPPED_MOB_STATS                     1    // Uses PatchSettings::MobStatCap.
#define PATCH_FORCE_MOB_ATTACKS_TO_HIT               0    // Forces physical and magic mob attacks to hit.
#define PATCH_MOVEMENT_SPEED_CAP                     1    // Uses PatchSettings::MovementSpeedCap.
#define PATCH_MOUNT_SPEED_CAP                        0    // Uses PatchSettings::MountSpeedCap; untested.
#define PATCH_REMOVE_SP_JOB_CHECKS                   1    // Removes SP checks between job advancements.
#define PATCH_LADDER_ROPE_SPEED                      1    // Uses PatchSettings::LadderRopeSpeed.
#define PATCH_REMOVE_RANGED_WHACK                    0    // Prevents ranged weapons from using melee whack.
#define PATCH_SUPER_TUBI                             1    // Removes item pickup/drop processing delays at all three checks.

// Quality of life
#define PATCH_FORCE_WINDOWED_MODE                    1    // Forces the client to launch in windowed mode.
#define PATCH_REMOVE_STARTUP_LOGOS                   1    // Skips startup logo screens.
#define PATCH_PASSWORD_LENGTH                        1    // Uses PatchSettings::PasswordLength.
#define PATCH_CASH_ITEMS_IN_STORAGE                  1    // Allows cash items in storage.
#define PATCH_CASH_ITEMS_DROPPABLE                   0    // Allows cash items to be dropped.
#define PATCH_CASH_ITEMS_TRADEABLE                   1    // Allows cash items to be traded.
#define PATCH_CASH_ITEMS_SELLABLE                    0    // Allows cash items to be sold.
#define PATCH_CASH_SHOP_BIRTHDAY_BYPASS              1    // Removes the birthday prompt for shop cash items.
#define PATCH_CASH_ITEM_TIME_FORMAT                  1    // Removes parentheses from remaining-time display.
#define PATCH_CASH_ITEM_SALE_DATE_BYPASS             1    // Removes sale date/time checks for cash items.
#define PATCH_CASH_ITEM_NORMAL_SALE_MESSAGES         1    // Uses normal sale messages for cash items.
#define PATCH_TYPEABLE_PIC                           1    // Allows PIC entry by typing.
#define PATCH_CENTER_WORLD_MAP                       1    // Centers the world map at the configured resolution.
#define PATCH_REMOVE_SKILL_NOT_READY_MESSAGE         0    // Hides the skill-not-ready message.
#define PATCH_REMOVE_CARD_FULL_MESSAGE               0    // Hides the Monster Book card-full message.
#define PATCH_REMOVE_CATCH_BREATH_MESSAGE            0    // Removes the catch-your-breath restriction message.
#define PATCH_REMOVE_GENDER_CHECKS                   0    // Removes gender restrictions from equipment.
#define PATCH_REMOVE_CHAT_SPAM_MESSAGES              1    // Removes repeated-line and chat-spam restrictions.
#define PATCH_REMOVE_FAMILY_SYSTEM                   1    // Hides Family UI and related controls.
#define PATCH_REMOVE_MAKER_CRAFT_TIME                0    // Shortens/removes Maker crafting delay.
#define PATCH_PET_BEHIND_PLAYER                      1    // Draws pets behind the player.
#define PATCH_EXPANDED_STORAGE                       0    // Applies expanded storage layout coordinates.
#define PATCH_EXPANDED_MERCHANT_SCROLLBAR            0    // Extends the merchant inventory scrollbar.
#define PATCH_EXPANDED_PLAYER_SCROLLBAR              0    // Extends the player inventory scrollbar in storage.
#define PATCH_EXPANDED_VENDOR                        0    // Applies expanded NPC vendor layout and button storage.
#define PATCH_EXPANDED_SKILLS                        0    // Applies expanded skill window layout and buttons.

// Fixes
#define PATCH_PARTY_BLOCK_BUTTON_TEXT                1    // Fixes party buttons displaying "block".
#define PATCH_ARAN_COMBO_POSITION                    1    // Positions Aran combo UI for the configured resolution.
#define PATCH_PQ_CLEAR_MESSAGE_POSITION              1    // Centers party-quest clear effects.
#define PATCH_CHARACTER_SELECT_STAT_POSITION         1    // Repositions character-select stat text.
#define PATCH_CHARACTER_SELECT_JOB_RANK_TEXT         0    // Repositions the "Ranked at X" job-ranking text.
#define PATCH_REMOVE_CHARACTER_SELECT_SERVER_NAME    0    // Removes server-name text at character selection.
#define PATCH_MONSTER_MAGNET_STATIC_ENEMIES          1    // Allows Monster Magnet on static enemies.
#define PATCH_ALIGN_CHAT_TEXT                        1    // Aligns chat-box text vertically.
#define PATCH_MOUSE_WHEEL_CURSOR                     1    // Fixes cursor position during mouse-wheel input.
#define PATCH_MONSTER_BOOK_TEXT                      1    // Aligns Monster Book text.
#define PATCH_GUILD_EMBLEM_BOTTOM_LINE               1    // Restores the missing guild emblem bottom line.

// Experimental/unused in cleanv83
#define PATCH_DISABLE_ALL_AP_BUTTONS                 0    // Disables every AP increase button.
#define PATCH_TRINKET_SLOT                           0    // Incomplete shoulder/trinket equipment slot experiment.
#define PATCH_DAMAGE_AND_ATTACK_SPEED_CAP            0    // Incomplete and marked non-working in cleanv83.
#define PATCH_REMOVE_QUICKSLOT_BUTTON                0    // Removes and repositions quickslot controls.
#define PATCH_WORLD_MAP_CAP_INCREASE                 0    // Experimental world-map cap increase.
#define PATCH_HAIR_ID_FIX                            0    // Applies the cleanv83 high hair-ID fix.
#define PATCH_HAIR_FACE_UNCAP                        0    // Incomplete high-version hair/face experiment.
#define PATCH_CASH_CAPE_EFFECT_IDS                   0    // Incomplete expanded cash cape-effect ID experiment.

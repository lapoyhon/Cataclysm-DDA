#pragma once
#ifndef MUTATION_H
#define MUTATION_H

#include "json.h"
#include "enums.h" // tripoint
#include "bodypart.h"
#include "damage.h"
#include "string_id.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

class nc_color;
class vitamin;
using vitamin_id = string_id<vitamin>;
class martialart;
using matype_id = string_id<martialart>;
struct dream;
struct mutation_branch;
class item;
using trait_id = string_id<mutation_branch>;
using itype_id = std::string;
struct mutation_category_trait;

extern std::vector<dream> dreams;
extern std::map<std::string, std::vector<trait_id> > mutations_category;

struct dream {
    std::vector<std::string> messages; // The messages that the dream will give
    std::string category; // The category that will trigger the dream
    int strength; // The category strength required for the dream

    dream() {
        category = "";
        strength = 0;
    }
};

struct mut_attack {
    /** Text printed when the attack is proced by you */
    std::string attack_text_u;
    /** As above, but for npc */
    std::string attack_text_npc;
    /** Need all of those to qualify for this attack */
    std::set<trait_id> required_mutations;
    /** Need none of those to qualify for this attack */
    std::set<trait_id> blocker_mutations;

    /** If not num_bp, this body part needs to be uncovered for the attack to proc */
    body_part bp = num_bp;

    /** Chance to proc is one_in( chance - dex - unarmed ) */
    int chance = 0;

    damage_instance base_damage;
    /** Multiplied by strength and added to the above */
    damage_instance strength_damage;

    /** Should be true when and only when this attack needs hardcoded handling */
    bool hardcoded_effect = false;
};

struct mutation_branch {
    using MutationMap = std::unordered_map<trait_id, mutation_branch>;
    // True if this is a valid mutation (False for "unavailable from generic mutagen").
    bool valid = false;
    // True if Purifier can remove it (False for *Special* mutations).
    bool purifiable;
    // True if it's a threshold itself, and shouldn't be obtained *easily* (False by default).
    bool threshold;
    // True if this is a trait associated with professional training/experience, so profession/quest ONLY.
    bool profession;
    // Wheather it has positive as well as negative effects.
    bool mixed_effect  = false;
    bool startingtrait = false;
    bool activated     = false;
    // Should it activate as soon as it is gained?
    bool starts_active = false;
    // Should it destroy gear on restricted body parts? (otherwise just pushes it off)
    bool destroys_gear = false;
    // Allow soft (fabric) gear on restricted body parts
    bool allow_soft_gear  = false;
    // IF any of the three are true, it drains that as the "cost"
    bool fatigue       = false;
    bool hunger        = false;
    bool thirst        = false;
    // How many points it costs in character creation
    int points     = 0;
    int visibility = 0;
    int ugliness   = 0;
    int cost       = 0;
    // costs are consumed consumed every cooldown turns,
    int cooldown   = 0;
    // bodytemp elements:
    int bodytemp_min = 0;
    int bodytemp_max = 0;
    int bodytemp_sleep = 0;
    // Healing per turn
    float healing_awake = 0.0f;
    float healing_resting = 0.0f;
    // Bonus HP multiplier. That is, 1.0 doubles hp, -0.5 halves it.
    float hp_modifier = 0.0f;
    // Second HP modifier that stacks with first but is otherwise identical.
    float hp_modifier_secondary = 0.0f;
    // Flat bonus/penalty to hp.
    float hp_adjustment = 0.0f;

    // Extra metabolism rate multiplier. 1.0 doubles usage, -0.5 halves.
    float metabolism_modifier = 0.0f;
    // As above but for thirst.
    float thirst_modifier = 0.0f;
    // As above but for fatigue.
    float fatigue_modifier = 0.0f;
    // Modifier for the rate at which fatigue drops when resting.
    float fatigue_regen_modifier = 0.0f;
    // Modifier for the rate at which stamina regenerates.
    float stamina_regen_modifier = 0.0f;

    /** The item, if any, spawned by the mutation */
    itype_id spawn_item;
    std::string spawn_item_message;

    /** Attacks granted by this mutation */
    std::vector<mut_attack> attacks_granted;

    /** Mutations may adjust one or more of the default vitamin usage rates */
    std::map<vitamin_id, int> vitamin_rates;

    std::vector<trait_id> prereqs; // Prerequisites; Only one is required
    std::vector<trait_id> prereqs2; // Prerequisites; need one from here too
    std::vector<trait_id> threshreq; // Prerequisites; dedicated slot to needing thresholds
    std::vector<trait_id> cancels; // Mutations that conflict with this one
    std::vector<trait_id> replacements; // Mutations that replace this one
    std::vector<trait_id> additions; // Mutations that add to this one
    std::vector<std::string> category; // Mutation Categories
    std::set<std::string> flags; // Mutation flags
    std::map<body_part, tripoint> protection; // Mutation wet effects
    std::map<body_part, int> encumbrance_always; // Mutation encumbrance that always applies
    // Mutation encumbrance that applies when covered with unfitting item
    std::map<body_part, int> encumbrance_covered;
    // Body parts that now need OVERSIZE gear
    std::set<body_part> restricts_gear;
    /** Key pair is <active: bool, mod type: "STR"> */
    std::unordered_map<std::pair<bool, std::string>, int> mods; // Mutation stat mods
    std::map<body_part, resistances> armor;
    std::vector<matype_id>
    initial_ma_styles; // Martial art styles that can be chosen upon character generation
    std::string name;
    std::string description;
    /**
     * Returns the color to display the mutation name with.
     */
    nc_color get_display_color() const;
    /**
     * Returns true if a character with this mutation shouldn't be able to wear given item.
     */
    bool conflicts_with_item( const item &it ) const;
    /**
     * Returns damage resistance on a given body part granted by this mutation.
     */
    const resistances &damage_resistance( body_part bp ) const;
    /**
     * Shortcut for getting the name of a (translated) mutation, same as
     * @code get( mutation_id ).name @endcode
     */
    static const std::string &get_name( const trait_id &mutation_id );
    /**
     * All known mutations. Key is the mutation id, value is the mutation_branch that you would
     * also get by calling @ref get.
     */
    static const MutationMap &get_all();
    // For init.cpp: reset (clear) the mutation data
    static void reset_all();
    // For init.cpp: load mutation data from json
    static void load( JsonObject &jsobj );
    // For init.cpp: check internal consistency (valid ids etc.) of all mutations
    static void check_consistency();

    /**
     * Load a trait blacklist specified by the given JSON object.
     */
    static void load_trait_blacklist( JsonObject &jsobj );

    /**
     * Check if the trait with the given ID is blacklisted.
     */
    static bool trait_is_blacklisted( const trait_id &tid );

    /** called after all JSON has been read and performs any necessary cleanup tasks */
    static void finalize();
    static void finalize_trait_blacklist();
};

struct mutation_category_trait {
    std::string name;
    std::string id;
    // Mutation catagory i.e "BIRD", "CHIMERA"
    std::string category;
    // For some reason most code uses "MUTCAT_category" instead of just "category"
    // This exists only to prevent ugly string hacks
    // @todo Make this not exist
    std::string category_full;
    // The trait that you gain when you break the threshold for this category
    trait_id threshold_mut;

    // The flag a mutagen needs to target this category
    std::string mutagen_flag;
    std::string mutagen_message; // message when you consume mutagen
    int mutagen_hunger  = 10;//these are defaults
    int mutagen_thirst  = 10;
    int mutagen_pain    = 2;
    int mutagen_fatigue = 5;
    int mutagen_morale  = 0;
    std::string iv_message; //message when you inject an iv;
    int iv_min_mutations    = 1; //the minimum mutations an injection provides
    int iv_additional_mutations = 2;
    int iv_additional_mutations_chance = 3; //chance of additional mutations
    int iv_hunger   = 10;
    int iv_thirst   = 10;
    int iv_pain     = 2;
    int iv_fatigue  = 5;
    int iv_morale   = 0;
    int iv_morale_max = 0;
    bool iv_sound = false;  //determines if you make a sound when you inject mutagen
    std::string iv_sound_message = "NULL";
    int iv_noise = 0;    //the amount of noise produced by the sound
    bool iv_sleep = false;  //whether the iv has a chance of knocking you out.
    std::string iv_sleep_message = "NULL";
    int iv_sleep_dur = 0;
    std::string junkie_message;
    std::string memorial_message; //memorial message when you cross a threshold

    static const std::map<std::string, mutation_category_trait> &get_all();
    static void reset();
    static void check_consistency();
};

void load_mutation_category( JsonObject &jsobj );
void load_dream( JsonObject &jsobj );
bool mutation_category_is_valid( const std::string &cat );

bool trait_display_sort( const trait_id &a, const trait_id &b ) noexcept;

#endif

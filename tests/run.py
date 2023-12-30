#!/usr/bin/env python3

import sys
import argparse

try:
    from simc_support.game_data import WowSpec
    from simc_support.game_data import Trinket
    from simc_support.game_data import Season
except ImportError:
    raise Exception(
        "simc-support dependency missing. Please install using 'pip3 install -r requirements.txt'"
    )

from helper import Test, TestGroup, run, find_profiles

FIGHT_STYLES = (
    "Patchwerk",
    "DungeonSlice",
    "HeavyMovement",
)
SEASON = Season.Season.SEASON_2


# Test all trinkets
def test_trinkets(klass: str, path: str, enable: dict):
    spec = WowSpec.get_wow_spec_from_combined_simc_name(klass)
    trinkets = Trinket.get_trinkets_for_spec(spec)
    seasonal_trinkets = [t for t in trinkets if SEASON in t.seasons]
    fight_style = args.trinkets_fight_style
    grp = TestGroup(
        "{}/{}/trinkets".format(profile, fight_style),
        fight_style=fight_style,
        profile=path,
    )
    tests.append(grp)
    for trinket in seasonal_trinkets:
        Test(
            "{} ({})".format(trinket.name, trinket.item_id),
            group=grp,
            all_talents=enable['talents'],
            all_sets=enable['sets'],
            args=[(
                "trinket1",
                "{},id={},ilevel={}".format(trinket.simc_name, trinket.item_id,
                                            trinket.min_itemlevel),
            )],
        )


# Test baseline profile & no-talent profile
def test_baseline(klass: str, path: str, enable: dict):
    spec = WowSpec.get_wow_spec_from_combined_simc_name(klass)
    fight_style = "Patchwerk"
    grp = TestGroup(
        "{}/{}/baseline".format(profile, fight_style),
        fight_style=fight_style,
        profile=path,
    )
    tests.append(grp)
    Test(
        "baseline profile",
        group=grp,
    )
    Test(
        "no talents",
        group=grp,
        args=[(
            "talents",
            "",
        )],
    )


available_tests = {
    "trinket": test_trinkets,
    "baseline": test_baseline,
}

parser = argparse.ArgumentParser(description="Run simc tests.")
parser.add_argument(
    "specialization",
    metavar="spec",
    type=str,
    help="Simc specialization in the form of CLASS_SPEC, eg. Priest_Shadow",
)
parser.add_argument(
    "-tests",
    nargs="+",
    default=["trinket"],
    required=False,
    help="Tests to run. Available tests: {}".format(
        [t for t in available_tests.keys()]),
)
parser.add_argument(
    "--trinkets-fight-style",
    default="DungeonSlice",
    type=str,
    help="Fight style used for trinket simulations.",
)
parser.add_argument(
    "--enable-all-talents",
    action="store_true",
    help="Enable all talent at maximum rank",
)
parser.add_argument(
    "--enable-all-sets",
    action="store_true",
    help="Enable all set bonuses",
)
# parser.add_argument('--soulbind-fight-style', default='DungeonSlice', type=str,
#                     help='Fight style used for soulbind simulations.')
parser.add_argument(
    "--max-profiles-to-use",
    default="0",
    type=int,
    help=
    "Maximum number of profiles to use per spec. 0 means use all available profiles",
)
args = parser.parse_args()

klass = args.specialization

print(" ".join(klass.split("_")))

tests = []
enable = {'talents': args.enable_all_talents, 'sets': args.enable_all_sets}
profiles = list(find_profiles(klass))
if args.max_profiles_to_use != 0:
    profiles = profiles[:args.max_profiles_to_use]

if len(profiles) == 0:
    print("No profile found for {}".format(klass))

# for profile, path in profiles:
#     for test in args.tests:
#         if test in available_tests:
#             available_tests[test](klass, path, enable)
#         else:
#             print("Could not find test {}".format(test))

# print(tests)
# sys.exit(run(tests))

import json

with open('encounter-items.json', 'r') as handle:
    items = json.load(handle)


def p(obj):
    print(json.dumps(obj, indent=2))

keys = {key for entry in items for key in entry.keys()}

catalogue = {
    key: [item.get(key) for item in items]
    for key in keys
}

# p(catalogue)
print(keys)
# for key in keys:
#     if key in [
#             "allowableClasses", "itemLimit", "sources", "id", "bonusLists",
#             "socketInfo", "profession", "specs", "contains",
#             "slotItemLevelOffsets", "name", "stats", "icon"
#     ]:
#         continue
#     print(key)
#     try:
#         assert (isinstance(catalogue.get(key), list))
#         print(set(catalogue.get(key)))
#     except:
#         print(catalogue.get(key))

classes = [{
    'name':
    'monk',
    'specs': [{
        'name': 'brewmaster',
        'id': 268,
        'weapon_types': [0, 4, 6, 7, 10, 13],
        'dual_wield': True,
        'caster_offhand': False,
        'armor_types': [2]
    }, {
        'name': 'windwalker',
        'id': 269,
        'weapon_types': [0, 4, 6, 7, 10, 13],
        'dual_wield': True,
        'caster_offhand': False,
        'armor_types': [2]
    }, {
        'name': 'mistweaver',
        'id': 270,
        'weapon_types': [4, 7, 10, 15],
        'dual_wield': False,
        'caster_offhand': True,
        'armor_types': [2]
    }]
}, {
    'name':
    'monk',
    'specs': [{
        'name': 'vengeance-m',
        'weapon_types': [0, 4, 6, 7, 10, 13],
        'dual_wield': True,
        'caster_offhand': False,
        'armor_types': [2]
    }, {
        'name': 'havoc-m',
        'weapon_types': [0, 4, 6, 7, 10, 13],
        'dual_wield': True,
        'caster_offhand': False,
        'armor_types': [2]
    }]
}, {
    'name':
    'demon_hunter',
    'specs': [{
        'name': 'vengeance',
        'weapon_types': [0, 4, 6, 7, 10, 13],
        'dual_wield': True,
        'caster_offhand': False,
        'armor_types': [2]
    }, {
        'name': 'havoc',
        'weapon_types': [0, 4, 6, 7, 10, 13],
        'dual_wield': True,
        'caster_offhand': False,
        'armor_types': [2]
    }]
}]

Enum_InventoryType = {
    1: [1],
    2: [2],
    3: [3],
    5: [5],
    6: [6],
    7: [7],
    8: [8],
    9: [9],
    10: [10],
    11: [11, 12],
    12: [13, 14],
    13: [16, 17],
    14: [17],
    15: [16],
    16: [15],
    17: [16],
    20: [5],
    23: [17],
    26: [16],
}

Enum_SlotName = {
    'head': 1,
    'neck': 2,
    'shoulder': 3,
    'chest': 5,
    'waist': 6,
    'legs': 7,
    'feet': 8,
    'wrist': 9,
    'hands': 10,
    'finger1': 11,
    'finger2': 12,
    'trinket1': 13,
    'trinket2': 14,
    'back': 15,
    'main_hand': 16,
    'off_hand': 17
}


import itertools


def find_spec_in_string(string):
    # strip filetype
    if string[-5:] == '.simc':
        string = string[:-5]

    # search for two-token class spec combination in filename
    # TODO: extend to k-token
    tokens = [token.lower() for token in string.split('_')]

    class_candidates = [
        klass for klass in classes if klass.get('name') in tokens
    ]
    return [
        spec for spec in itertools.chain(*[  # pyright: ignore
            klass.get('specs') for klass in class_candidates
            if isinstance(klass, dict)
        ]) if spec.get('name') in tokens
    ]

def is_offhand(item):
    return item.get('itemClass') == 4 and item.get('itemSubClass') == 0 and item.get('inventoryType') == 17

def is_equippable(item, spec):
    # not allowed to equip this item based on class rule
    if item.get('allowableClasses') is not None and spec.get('id') not in item.get('allowableClasses'):
        return False
    # allowed to equip caster offhands
    if is_offhand(item) and spec.get('caster_offhand'):
        return True
    # allowed to equip this weapon type
    if item.get('itemClass') == 2 and item.get('itemSubClass') in spec.get('weapon_types'):
        return True
    # allowed to equip this armour type
    if item.get('itemClass') == 4 and item.get('itemSubClass') in spec.get('armor_types'):
        return True
    # allowed to equip any neck, ring, trinket, or cloak
    if item.get('inventoryType') in [2, 11, 12, 16]:
        return True
    return False

def fits_in_slot(item, slot):
    return item.get('inventorySlot')
    return any([entry == slot for entry in Enum_InventoryType.get(item.get('inventoryType'), [])])

tests = []
fight_style = 'Patchwerk'
for (profile, path) in profiles:
    spec_matches = find_spec_in_string(profile)
    assert len( spec_matches ) == 1, """Too many specs matched from profile names, refine your search.
    The following specs were matched:""" + str( itertools.chain( *['\t' + spec.get('name') + '\n' for spec in spec_matches])) # yapf: disable

    spec = spec_matches[0]
    grp = TestGroup(
        f'{profile}/{fight_style}/everything',
        fight_style=fight_style,
        profile=path
    )
    tests.append(grp)
    equippable_by_inventory_type = {
        inventory_type: [
            item for item in items
            if is_equippable(item, spec) and item.get('inventoryType') == inventory_type
        ]
        for inventory_type in Enum_InventoryType.keys()
    }
    # create default mappings which is sufficient for most items
    equippable_by_inventory_slot = {
        slot_name: [ equippable_by_inventory_type.get(inventory_type) for inventory_type, slot_ids in Enum_InventoryType.items() if slot_id in slot_ids ]
        for slot_name, slot_id in Enum_SlotName.keys()
    }
    # handle weapons
    # handle rings
    # handle trinkets
    print()

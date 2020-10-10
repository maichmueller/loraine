import json
import os


if __name__ == '__main__':
    fpath = os.path.join(os.getcwd(), "sets", "set1-en_us", "en_us", "data", "set1-en_us.json")
    with open(fpath, "rb") as json_file:
        file = json.load(json_file)
    for i in range(10):
        print(*(file[i].items()), sep="\n")
    cards = []
    for card in file:
        details = {
            "name": card["name"],
            "region": card["region"],
            "rarity": card["rarity"],
            "collectible": card["collectible"],
            "flavor_text": card["flavorText"].replace(r"\\", ""),
            "type": card["type"],
            "supertype": card["supertype"],
            "attack": card["attack"],
            "cost": card["cost"],
            "health": card["health"],
            "levelup_description_raw": card["levelupDescriptionRaw"],
            "desc": card["descriptionRaw"],
            "code": card["cardCode"],
            "keywords": [kword.replace("'", "").replace(" ", "_").upper() for kword in card["keywords"]],
            "associated_card_codes": card["associatedCardRefs"]
        }
        cards.append(details)
    print(*cards)

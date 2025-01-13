import xmltodict
import json
import sys, os

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("arg1: xml")
    else:
        xml_name = sys.argv[1]
        f = open(xml_name, 'r')
        xml_data = f.read()
        f.close()
        data_dict = xmltodict.parse(xml_data)
        json_data = json.dumps(data_dict, indent=4)
        json_data = json_data.replace('"@', '"')
        filename, ext = os.path.splitext(xml_name)
        json_name = xml_name[:-len(ext)] + ".json"
        f = open(json_name, "w")
        f.write(json_data)
        f.close()
        print(f"save to {json_name}")

        dict_data = json.loads(json_data)
        chip_data = dict_data['DediProgChipDatabase']['Portofolio']['Chip']
        print(f"{len(chip_data)} chips.")
        chip_key_list = set()
        for chip in chip_data:
            l = list(chip.keys())
            chip_key_list.update(l)
        print(f"full keys: {list(chip_key_list)}")


import sys, os, re

if __name__ == '__main__':
    xml_name_in = sys.argv[1]
    xml_name_out = sys.argv[2]
    f_out = open(xml_name_out, 'w')
    line_num = 0
    with open(xml_name_in, 'r') as f_in:
        xml_data = f_in.readlines()
        for line in xml_data:
            if line.count('<!--') == 0:
                new_line = line.replace('&', 'and')
                f_out.write(new_line)
            else:
                pattern = r"-{3,}(.*?)-{3,}"
                matches = re.findall(pattern, line)
                for i, match in enumerate(matches, 1):
                    # print(f"Block {i}: {match.strip()}")
                    f_out.write(f"<!-- {match.strip()} -->\n")
            line_num += 1
    f_out.close()
    print(f"save to {xml_name_out} with {line_num} lines")

